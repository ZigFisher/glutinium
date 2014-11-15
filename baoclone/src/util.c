/*
 * Auxiliary functions.
 *
 * Copyright (C) 2013 Serge Vakulenko, KK6ABQ
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef MINGW32
#   include <windows.h>
#else
#   include <termios.h>
#   include <sys/stat.h>
#endif
#include "util.h"

#ifdef MINGW32
static DCB saved_mode;                  // Mode of serial port, Windows
#else
static struct termios oldtio, newtio;   // Mode of serial port, Unix
#endif

//
// CTCSS tones, Hz*10.
//
const int CTCSS_TONES [NCTCSS] = {
     670,  693,  719,  744,  770,  797,  825,  854,  885,  915,
     948,  974, 1000, 1035, 1072, 1109, 1148, 1188, 1230, 1273,
    1318, 1365, 1413, 1462, 1514, 1567, 1598, 1622, 1655, 1679,
    1713, 1738, 1773, 1799, 1835, 1862, 1899, 1928, 1966, 1995,
    2035, 2065, 2107, 2181, 2257, 2291, 2336, 2418, 2503, 2541,
};

//
// DCS codes.
//
const int DCS_CODES [NDCS] = {
     23,  25,  26,  31,  32,  36,  43,  47,  51,  53,
     54,  65,  71,  72,  73,  74, 114, 115, 116, 122,
    125, 131, 132, 134, 143, 145, 152, 155, 156, 162,
    165, 172, 174, 205, 212, 223, 225, 226, 243, 244,
    245, 246, 251, 252, 255, 261, 263, 265, 266, 271,
    274, 306, 311, 315, 325, 331, 332, 343, 346, 351,
    356, 364, 365, 371, 411, 412, 413, 423, 431, 432,
    445, 446, 452, 454, 455, 462, 464, 465, 466, 503,
    506, 516, 523, 526, 532, 546, 565, 606, 612, 624,
    627, 631, 632, 654, 662, 664, 703, 712, 723, 731,
    732, 734, 743, 754,
};


//
// Check for a regular file.
//
int is_file (char *filename)
{
#ifdef MINGW32
    // Treat COM* as a device.
    return strncasecmp (filename, "com", 3) != 0;
#else
    struct stat st;

    if (stat (filename, &st) < 0) {
        // File not exist: treat it as a regular file.
        return 1;
    }
    return (st.st_mode & S_IFMT) == S_IFREG;
#endif
}

//
// Print data in hex format.
//
void print_hex (const unsigned char *data, int len)
{
    int i;

    printf ("%02x", (unsigned char) data[0]);
    for (i=1; i<len; i++)
        printf ("-%02x", (unsigned char) data[i]);
}

//
// Open the serial port.
//
int serial_open (char *portname)
{
#ifdef MINGW32
    HANDLE fd;
    DCB new_mode;
    COMMTIMEOUTS timo;

    /* Open port */
    fd = CreateFile (portname, GENERIC_READ | GENERIC_WRITE,
        0, 0, OPEN_EXISTING, 0, 0);
    if (fd == INVALID_HANDLE_VALUE) {
        fprintf (stderr, "%s: Cannot open\n", portname);
        exit (-1);
    }

    /* Set serial attributes */
    memset (&saved_mode, 0, sizeof(saved_mode));
    if (! GetCommState (fd, &saved_mode)) {
        fprintf (stderr, "%s: Cannot get state\n", portname);
        exit (-1);
    }

    new_mode = saved_mode;

    new_mode.BaudRate = CBR_9600;
    new_mode.ByteSize = 8;
    new_mode.StopBits = ONESTOPBIT;
    new_mode.Parity = 0;
    new_mode.fParity = FALSE;
    new_mode.fOutX = FALSE;
    new_mode.fInX = FALSE;
    new_mode.fOutxCtsFlow = FALSE;
    new_mode.fOutxDsrFlow = FALSE;
    new_mode.fRtsControl = RTS_CONTROL_DISABLE;
    new_mode.fNull = FALSE;
    new_mode.fAbortOnError = FALSE;
    new_mode.fBinary = TRUE;
    if (! SetCommState (fd, &new_mode)) {
        fprintf (stderr, "%s: Cannot set state\n", portname);
        exit (-1);
    }

    timo.ReadIntervalTimeout = 0;
    timo.ReadTotalTimeoutMultiplier = 0;
    timo.ReadTotalTimeoutConstant = 200;
    timo.WriteTotalTimeoutMultiplier = 1;
    timo.WriteTotalTimeoutConstant = 1000;
    SetCommTimeouts (fd, &timo);

    // Flush received data pending on the port.
    PurgeComm (fd, PURGE_RXCLEAR);
    return (int) fd;
#else
    int fd;

    // Use non-block flag to ignore carrier (DCD).
    fd = open (portname, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        perror (portname);
        exit (-1);
    }

    // Get terminal modes.
    tcgetattr (fd, &oldtio);
    newtio = oldtio;

    newtio.c_cflag &= ~CSIZE;
    newtio.c_cflag |= CS8;              // 8 data bits
    newtio.c_cflag |= CLOCAL | CREAD;   // enable receiver, set local mode
    newtio.c_cflag &= ~PARENB;          // no parity
    newtio.c_cflag &= ~CSTOPB;          // 1 stop bit
    newtio.c_cflag &= ~CRTSCTS;         // no h/w handshake
    newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // raw input
    newtio.c_oflag &= ~OPOST;           // raw output
    newtio.c_iflag &= ~IXON;            // software flow control disabled
    newtio.c_iflag &= ~ICRNL;           // do not translate CR to NL

    cfsetispeed(&newtio, B9600);        // Set baud rate.
    cfsetospeed(&newtio, B9600);

    // Set terminal modes.
    tcsetattr (fd, TCSANOW, &newtio);

    // Clear the non-block flag.
    int flags = fcntl (fd, F_GETFL, 0);
    if (flags < 0) {
        perror("F_GETFL");
        exit (-1);
    }
    flags &= ~O_NONBLOCK;
    if (fcntl (fd, F_SETFL, flags) < 0) {
        perror("F_SETFL");
        exit (-1);
    }

    // Flush received data pending on the port.
    tcflush (fd, TCIFLUSH);
    return fd;
#endif
}

//
// Purge all received data.
//
void serial_flush (int fd)
{
#ifdef MINGW32
    PurgeComm ((HANDLE) fd, PURGE_RXCLEAR);
#else
    tcflush (fd, TCIFLUSH);
#endif
}

//
// Close the serial port.
//
void serial_close (int fd)
{
    // Restore the port mode.
#ifdef MINGW32
    SetCommState ((HANDLE) fd, &saved_mode);
    CloseHandle ((HANDLE) fd);
#else
    tcsetattr (fd, TCSANOW, &oldtio);
    close (fd);
#endif
}

//
// Read data from serial port.
// Return 0 when no data available.
// Use 200-msec timeout.
//
int serial_read (int fd, unsigned char *data, int len)
{
#ifdef MINGW32
    DWORD nbytes;
    int len0 = len;

    for (;;) {
        if (! ReadFile ((HANDLE)fd, data, len, &nbytes, 0) || nbytes <= 0)
        if (nbytes <= 0)
            return 0;

        len -= nbytes;
        if (len <= 0)
            return len0;

        data += nbytes;
    }
#else
    fd_set rset, wset, xset;
    int nbytes, len0 = len;

    for (;;) {
        // Initialize file descriptor sets.
        FD_ZERO (&rset);
        FD_ZERO (&wset);
        FD_ZERO (&xset);
        FD_SET (fd, &rset);

        // Set timeout to 100 msec.
        struct timeval timo;
        timo.tv_sec = 0;
        timo.tv_usec = 200000;

        // Wait for input to become ready or until the time out.
        if (select (fd + 1, &rset, &wset, &xset, &timo) != 1)
            return 0;

        nbytes = read (fd, (unsigned char*) data, len);
        if (nbytes <= 0)
            return 0;

        len -= nbytes;
        if (len <= 0)
            return len0;

        data += nbytes;
    }
#endif
}

//
// Write data to serial port.
//
void serial_write (int fd, const void *data, int len)
{
#ifdef MINGW32
    DWORD count;

    WriteFile ((HANDLE)fd, data, len, &count, 0);
#else
    if (write (fd, data, len) != len) {
        perror ("Serial port");
        exit (-1);
    }
#endif
}

//
// Delay in milliseconds.
//
void mdelay (unsigned msec)
{
#ifdef MINGW32
    Sleep (msec);
#else
    usleep (msec * 1000);
#endif
}

//
// Convert 32-bit value from binary coded decimal
// to integer format (8 digits).
//
int bcd_to_int (int bcd)
{
    return ((bcd >> 28) & 15) * 10000000 +
           ((bcd >> 24) & 15) * 1000000 +
           ((bcd >> 20) & 15) * 100000 +
           ((bcd >> 16) & 15) * 10000 +
           ((bcd >> 12) & 15) * 1000 +
           ((bcd >> 8)  & 15) * 100 +
           ((bcd >> 4)  & 15) * 10 +
           (bcd         & 15);
}

//
// Convert 32-bit value from integer
// binary coded decimal format (8 digits).
//
int int_to_bcd (int val)
{
    return ((val / 10000000) % 10) << 28 |
           ((val / 1000000)  % 10) << 24 |
           ((val / 100000)   % 10) << 20 |
           ((val / 10000)    % 10) << 16 |
           ((val / 1000)     % 10) << 12 |
           ((val / 100)      % 10) << 8 |
           ((val / 10)       % 10) << 4 |
           (val              % 10);
}

//
// Get a binary value of the parameter: On/Off,
// Ignore case.
// For invlid value, print a message and halt.
//
int on_off (char *param, char *value)
{
    if (strcasecmp ("On", value) == 0)
        return 1;
    if (strcasecmp ("Off", value) == 0)
        return 0;
    fprintf (stderr, "Bad value for %s: %s\n", param, value);
    exit(-1);
}

//
// Get integer value, or "Off" as 0,
// Ignore case.
//
int atoi_off (const char *value)
{
    if (strcasecmp ("Off", value) == 0)
        return 0;
    return atoi (value);
}

//
// Copy a text string to memory image.
// Clear unused part with spaces.
//
void copy_str (unsigned char *dest, const char *src, int nbytes)
{
    int i;

    for (i=0; i<nbytes; i++) {
        *dest++ = (*src ? *src++ : ' ');
    }
}

//
// Find a string in a table of size nelem, ignoring case.
// Return -1 when not found.
//
int string_in_table (const char *value, const char *tab[], int nelem)
{
    int i;

    for (i=0; i<nelem; i++) {
        if (strcasecmp (tab[i], value) == 0) {
            return i;
        }
    }
    return -1;
}

//
// Print description of the parameter.
//
void print_options (FILE *out, const char **tab, int num, const char *info)
{
    int i;

    fprintf (out, "\n");
    if (info)
        fprintf (out, "# %s\n", info);
    fprintf (out, "# Options:");
    for (i=0; i<num; i++) {
        if (i > 0)
            fprintf (out, ",");
        fprintf (out, " %s", tab[i]);
    }
    fprintf (out, "\n");
}

//
// Print list of all squelch tones.
//
void print_squelch_tones (FILE *out, int normal_only)
{
    int i;

    fprintf (out, "#\n");
    fprintf (out, "# Squelch tones:");
    for (i=0; i<NCTCSS; i++) {
        fprintf (out, " %.1f", CTCSS_TONES[i] / 10.0);
        if ((i + 1) % 12 == 0)
            fprintf (out, "\n#");
    }
    for (i=0; i<NDCS; i++) {
        fprintf (out, " D%03d%s", DCS_CODES[i], normal_only ? "" : "N");
        if ((i + 3) % 12 == 0)
            fprintf (out, "\n#");
    }
    if (! normal_only) {
        for (i=0; i<NDCS; i++) {
            fprintf (out, " D%03dI", DCS_CODES[i]);
            if ((i + 11) % 12 == 0)
                fprintf (out, "\n#");
        }
    }
    fprintf (out, "\n");
}
