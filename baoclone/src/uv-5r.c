/*
 * Interface to Baofeng UV-5R and compatibles.
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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "radio.h"
#include "util.h"

#define NCHAN 128

static const char *PTTID_NAME[] = { "-", "Begin", "End", "Both" };

static const char *STEP_NAME[] = { "2.5",  "5.0",  "6.25", "10.0",
                                   "12.5", "20.0", "25.0", "50.0" };

static const char *SAVER_NAME[] = { "Off", "1", "2", "3", "4", "?5?", "?6?", "?7?" };

static const char *VOX_NAME[] = { "Off", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                           "10", "?11?", "?12?", "?13?", "?14?", "?15?" };

static const char *ABR_NAME[] = { "Off", "1", "2", "3", "4", "5", "?6?", "?7?" };

static const char *DTMF_SIDETONE_NAME[] = { "Off", "DTMF Only", "ANI Only", "DTMF+ANI" };

static const char *SCAN_RESUME_NAME[] = { "TO", "CO", "SE", "??" };

static const char *DISPLAY_MODE_NAME[] = { "Channel", "Name", "Frequency", "??" };

static const char *COLOR_NAME[] = { "Off", "Blue", "Orange", "Purple" };

static const char *ALARM_NAME[] = { "Site", "Tone", "Code", "??" };

static const char *RPSTE_NAME[] = { "Off", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                             "10", "?11?", "?12?", "?13?", "?14?", "?15?" };

static const char *VOICE_NAME[] = { "Off", "English", "Chinese", "??" };

static const char *OFF_ON[] = { "Off", "On" };

//
// Print a generic information about the device.
//
static void uv5r_print_version (FILE *out)
{
    char buf[17], *version = buf, *p;

    // Copy the string, trim spaces.
    strncpy (version, (char*)&radio_mem[0x1EC0+0x30], 16);
    version [16] = 0;
    while (*version == ' ')
        version++;
    p = version + strlen(version);
    while (p > version && p[-1]==' ')
        *--p = 0;

    // Don't print firmware and serial number to file,
    // to prevent the user to copy them from one radio to another.
    if (out == stdout) {
        // 3+poweron message
        fprintf (out, "Firmware: %s\n", version);

        // 6+poweron message
        fprintf (out, "Serial: %.16s\n", &radio_mem[0x1EC0+0x10]);
    }
}

static void aged_print_version (FILE *out)
{
    // Nothing to print.
}

//
// Check that the radio does support this frequency.
//
static int is_valid_frequency (int mhz)
{
    if (mhz >= 136 && mhz <= 174)
        return 1;
    if (mhz >= 400 && mhz <= 520)
        return 1;
    return 0;
}


//
// Read block of data, up to 64 bytes.
// Halt the program on any error.
//
static void read_block (int fd, int start, unsigned char *data, int nbytes)
{
    unsigned char cmd[4], reply[4];
    int addr, len;

    // Send command.
    cmd[0] = 'S';
    cmd[1] = start >> 8;
    cmd[2] = start;
    cmd[3] = nbytes;
    serial_write (fd, cmd, 4);

    // Read reply.
    if (serial_read (fd, reply, 4) != 4) {
        fprintf (stderr, "Radio refused to send block 0x%04x.\n", start);
        exit(-1);
    }
    addr = reply[1] << 8 | reply[2];
    if (reply[0] != 'X' || addr != start || reply[3] != nbytes) {
        fprintf (stderr, "Bad reply for block 0x%04x of %d bytes: %02x-%02x-%02x-%02x\n",
            start, nbytes, reply[0], reply[1], reply[2], reply[3]);
        exit(-1);
    }

    // Read data.
    len = serial_read (fd, data, 0x40);
    if (len != nbytes) {
        fprintf (stderr, "Reading block 0x%04x: got only %d bytes.\n", start, len);
        exit(-1);
    }

    // Get acknowledge.
    serial_write (fd, "\x06", 1);
    if (serial_read (fd, reply, 1) != 1) {
        fprintf (stderr, "No acknowledge after block 0x%04x.\n", start);
        exit(-1);
    }
    if (reply[0] != 0x06) {
        fprintf (stderr, "Bad acknowledge after block 0x%04x: %02x\n", start, reply[0]);
        exit(-1);
    }
    if (verbose) {
        printf ("# Read 0x%04x: ", start);
        print_hex (data, nbytes);
        printf ("\n");
    } else {
        ++radio_progress;
        if (radio_progress % 2 == 0) {
            fprintf (stderr, "#");
            fflush (stderr);
        }
    }
}

//
// Write block of data, up to 16 bytes.
// Halt the program on any error.
//
static void write_block (int fd, int start, const unsigned char *data, int nbytes)
{
    unsigned char cmd[4], reply;

    // Send command.
    cmd[0] = 'X';
    cmd[1] = start >> 8;
    cmd[2] = start;
    cmd[3] = nbytes;
    serial_write (fd, cmd, 4);
    serial_write (fd, data, nbytes);

    // Get acknowledge.
    if (serial_read (fd, &reply, 1) != 1) {
        fprintf (stderr, "No acknowledge after block 0x%04x.\n", start);
        exit(-1);
    }
    if (reply != 0x06) {
        fprintf (stderr, "Bad acknowledge after block 0x%04x: %02x\n", start, reply);
        exit(-1);
    }

    if (verbose) {
        printf ("# Write 0x%04x: ", start);
        print_hex (data, nbytes);
        printf ("\n");
    } else {
        ++radio_progress;
        if (radio_progress % 8 == 0) {
            fprintf (stderr, "#");
            fflush (stderr);
        }
    }
}

//
// Read memory image from the device.
//
static void uv5r_download()
{
    int addr;

    // Main block.
    for (addr=0; addr<0x1800; addr+=0x40)
        read_block (radio_port, addr, &radio_mem[addr], 0x40);

    // Auxiliary block starts at 0x1EC0.
    for (addr=0x1EC0; addr<0x2000; addr+=0x40)
        read_block (radio_port, addr, &radio_mem[addr], 0x40);
}

static void aged_download()
{
    int addr;

    // Main block only.
    for (addr=0; addr<0x1800; addr+=0x40)
        read_block (radio_port, addr, &radio_mem[addr], 0x40);
}

//
// Write memory image to the device.
//
static void uv5r_upload (int cont_flag)
{
    int addr;

    // Main block.
    for (addr=0; addr<0x1800; addr+=0x10)
        write_block (radio_port, addr, &radio_mem[addr], 0x10);

    // Auxiliary block starts at 0x1EC0.
    for (addr=0x1EC0; addr<0x2000; addr+=0x10)
        write_block (radio_port, addr, &radio_mem[addr], 0x10);
}

static void aged_upload()
{
    int addr;

    // Main block only.
    for (addr=0; addr<0x1800; addr+=0x10)
        write_block (radio_port, addr, &radio_mem[addr], 0x10);
}

static void decode_squelch (uint16_t index, int *ctcs, int *dcs)
{
    if (index == 0 || index == 0xffff) {
        // Squelch disabled.
        return;
    }
    if (index >= 0x0258) {
        // CTCSS value is Hz multiplied by 10.
        *ctcs = index;
        *dcs = 0;
        return;
    }
    // DCS mode.
    if (index - 1 < NDCS)
        *dcs = DCS_CODES[index - 1];
    else
        *dcs = - DCS_CODES[index - 1 - NDCS];
    *ctcs = 0;
}

//
// Convert squelch string to tone value in BCD format.
// Four possible formats:
// nnn.n - CTCSS frequency
// DnnnN - DCS normal
// DnnnI - DCS inverted
// '-'   - Disabled
//
static int encode_squelch (char *str)
{
    unsigned val;

    if (*str == 'D' || *str == 'd') {
        // DCS tone
        char *e;
        val = strtol (++str, &e, 10);

        // Find a valid index in DCS table.
        int i;
        for (i=1; i<=NDCS; i++)
            if (DCS_CODES[i-1] == val)
                break;
        if (i > NDCS)
            return 0;

        if (*e == 'N' || *e == 'n') {
            val = i;
        } else if (*e == 'I' || *e == 'i') {
            val = i + NDCS + 1;
        } else {
            return 0;
        }
    } else if (*str >= '0' && *str <= '9') {
        // CTCSS tone
        float hz;
        if (sscanf (str, "%f", &hz) != 1)
            return 0;

        // Round to integer.
        val = hz * 10.0 + 0.5;
        if (val < 0x0258)
            return 0;
    } else {
        // Disabled
        return 0;
    }

    return val;
}

typedef struct {
    uint32_t    rxfreq; // binary coded decimal, 8 digits
    uint32_t    txfreq; // binary coded decimal, 8 digits
    uint16_t    rxtone;
    uint16_t    txtone;
    uint8_t     scode    : 4,
                _u1      : 4;
    uint8_t     _u2;
    uint8_t     lowpower : 1,
                _u3      : 7;
    uint8_t     pttidbot : 1,
                pttideot : 1,
                scan     : 1,
                bcl      : 1,
                _u5      : 2,
                wide     : 1,
                _u4      : 1;
} memory_channel_t;

static void decode_channel (int i, char *name, int *rx_hz, int *tx_hz,
    int *rx_ctcs, int *tx_ctcs, int *rx_dcs, int *tx_dcs,
    int *lowpower, int *wide, int *scan, int *bcl, int *pttid, int *scode)
{
    memory_channel_t *ch = i + (memory_channel_t*) radio_mem;

    *rx_hz = *tx_hz = *rx_ctcs = *tx_ctcs = *rx_dcs = *tx_dcs = 0;
    *name = 0;
    if (ch->rxfreq == 0 || ch->rxfreq == 0xffffffff)
        return;

    // Extract channel name; strip trailing FF's.
    char *p;
    strncpy (name, (char*) &radio_mem[0x1000 + i*16], 7);
    name[7] = 0;
    for (p=name+6; p>=name && *p=='\xff'; p--)
        *p = 0;

    // Decode channel frequencies.
    *rx_hz = bcd_to_int (ch->rxfreq) * 10;
    *tx_hz = bcd_to_int (ch->txfreq) * 10;

    // Decode squelch modes.
    decode_squelch (ch->rxtone, rx_ctcs, rx_dcs);
    decode_squelch (ch->txtone, tx_ctcs, tx_dcs);

    // Other parameters.
    *lowpower = ch->lowpower;
    *wide = ch->wide;
    *scan = ch->scan;
    *bcl = ch->bcl;
    *scode = ch->scode;
    *pttid = ch->pttidbot | (ch->pttideot << 1);
}

static void setup_channel (int i, char *name, double rx_mhz, double tx_mhz,
    int rq, int tq, int lowpower, int wide, int scan, int bcl, int scode,
    int pttid)
{
    memory_channel_t *ch = i + (memory_channel_t*) radio_mem;

    ch->rxfreq = int_to_bcd ((int) (rx_mhz * 100000.0 + 0.5));

    if (is_valid_frequency (tx_mhz)) {
        ch->txfreq = int_to_bcd ((int) (tx_mhz * 100000.0 + 0.5));
    } else {
        // disable TX
        ch->txfreq = 0xffffffff;
    }

    ch->rxtone = rq;
    ch->txtone = tq;
    ch->lowpower = lowpower;
    ch->wide = wide;
    ch->scan = scan;
    ch->bcl = bcl;
    ch->scode = scode;
    ch->pttidbot = pttid & 1;
    ch->pttideot = pttid >> 1;
    ch->_u1 = 0;
    ch->_u2 = 0;
    ch->_u3 = 0;
    ch->_u4 = 0;
    ch->_u5 = 0;

    // Copy channel name.
    if (! name || ! *name || *name == '-')
        name = "\xff\xff\xff\xff\xff\xff\xff";
    strncpy ((char*) &radio_mem[0x1000 + i*16], name, 7);
}

static void erase_channel (int i)
{
    memory_channel_t *ch = i + (memory_channel_t*) radio_mem;

    // Erase channel name.
    memset (ch, 0xff, 16);
    memset (&radio_mem[0x1000 + i*16], 0xff, 7);
}

typedef struct {
    uint8_t     enable;
    uint8_t     lower_msb; // binary coded decimal, 4 digits
    uint8_t     lower_lsb;
    uint8_t     upper_msb; // binary coded decimal, 4 digits
    uint8_t     upper_lsb;
} limits_t;

//
// Looks like limits are not implemented on old firmware
// (prior to version 291).
//
static void decode_limits (char band, int *enable, int *lower, int *upper)
{
    int offset = (band == 'V') ? 0x1EC0+0x100 : 0x1EC0+0x105;

    limits_t *limits = (limits_t*) (radio_mem + offset);
    *enable = limits->enable;
    *lower = ((limits->lower_msb >> 4) & 15) * 1000 +
             (limits->lower_msb        & 15) * 100 +
             ((limits->lower_lsb >> 4) & 15) * 10 +
             (limits->lower_lsb        & 15);
    *upper = ((limits->upper_msb >> 4) & 15) * 1000 +
             (limits->upper_msb        & 15) * 100 +
             ((limits->upper_lsb >> 4) & 15) * 10 +
             (limits->upper_lsb        & 15);
}

static void setup_limits (char band, int enable, int lower, int upper)
{
    int offset = (band == 'V') ? 0x1EC0+0x100 : 0x1EC0+0x105;

    limits_t *limits = (limits_t*) (radio_mem + offset);
    limits->enable = enable;

    limits->lower_msb = ((lower / 1000) % 10) << 4 |
                        ((lower / 100)  % 10);
    limits->lower_lsb = ((lower / 10)   % 10) << 4 |
                        (lower          % 10);
    limits->upper_msb = ((upper / 1000) % 10) << 4 |
                        ((upper / 100)  % 10);
    limits->upper_lsb = ((upper / 10)   % 10) << 4 |
                        (upper          % 10);
}

static void fetch_ani (char *ani)
{
    int i;

    for (i=0; i<5; i++)
        ani[i] = "0123456789ABCDEF" [radio_mem[0x0CAA+i] & 0x0f];
}

static void setup_ani (char *ani)
{
    int i, v;

    for (i=0; i<5; i++) {
        v = ani[i];

        // Get next hex digit.
        if (v >= '0' && v <= '9')
            v -= '0';
        else if (v >= 'A' && v <= 'F')
            v -= 'A' - 10;
        else if (v >= 'a' && v <= 'f')
            v -= 'a' - 10;
        else
            v = 0;

        radio_mem[0x0CAA+i] = v;
    }
}

typedef struct {
    uint8_t     freq[8];    // binary coded decimal, 8 digits
    uint8_t     _u1;
    uint8_t     offset[4];  // binary coded decimal, 8 digits
    uint8_t     _u2;
    uint16_t    rxtone;
    uint16_t    txtone;
    uint8_t     band     : 1,
                _u3      : 7;
    uint8_t     _u4;
    uint8_t     scode    : 4,
                _u5      : 4;
    uint8_t     _u6;
    uint8_t     _u7      : 4,
                step     : 3,
                _u8      : 1;
    uint8_t     _u9      : 6,
                narrow   : 1,
                lowpower : 1;
} vfo_t;

static void decode_vfo (int index, int *band, int *hz, int *offset,
    int *rx_ctcs, int *tx_ctcs, int *rx_dcs, int *tx_dcs,
    int *lowpower, int *wide, int *step, int *scode)
{
    vfo_t *vfo = (vfo_t*) &radio_mem[index ? 0x0F28 : 0x0F08];

    *band = *hz = *offset = *rx_ctcs = *tx_ctcs = *rx_dcs = *tx_dcs = 0;
    *lowpower = *wide = *step = *scode = 0;

    *band = vfo->band;
    *hz = (vfo->freq[0] & 15) * 100000000 +
          (vfo->freq[1] & 15) * 10000000 +
          (vfo->freq[2] & 15) * 1000000 +
          (vfo->freq[3] & 15) * 100000 +
          (vfo->freq[4] & 15) * 10000 +
          (vfo->freq[5] & 15) * 1000 +
          (vfo->freq[6] & 15) * 100 +
          (vfo->freq[7] & 15) * 10;
    *offset = (vfo->offset[0] & 15) * 100000000 +
              (vfo->offset[1] & 15) * 10000000 +
              (vfo->offset[2] & 15) * 1000000 +
              (vfo->offset[3] & 15) * 100000;
    decode_squelch (vfo->rxtone, rx_ctcs, rx_dcs);
    decode_squelch (vfo->txtone, tx_ctcs, tx_dcs);
    *lowpower = vfo->lowpower;
    *wide = ! vfo->narrow;
    *step = vfo->step;
    *scode = vfo->scode;
}

static void setup_vfo (int index, int band, int hz, int offset,
    int rxtone, int txtone, int step, int lowpower, int wide, int scode)
{
    vfo_t *vfo = (vfo_t*) &radio_mem[index ? 0x0F28 : 0x0F08];

    vfo->band = (band == 'U');
    vfo->freq[0] = (hz / 100000000) % 10;
    vfo->freq[1] = (hz / 10000000) % 10;
    vfo->freq[2] = (hz / 1000000) % 10;
    vfo->freq[3] = (hz / 100000) % 10;
    vfo->freq[4] = (hz / 10000) % 10;
    vfo->freq[5] = (hz / 1000) % 10;
    vfo->freq[6] = (hz / 100) % 10;
    vfo->freq[7] = (hz / 10) % 10;
    vfo->offset[0] = (offset / 100000000) % 10;
    vfo->offset[1] = (offset / 10000000) % 10;
    vfo->offset[2] = (offset / 1000000) % 10;
    vfo->offset[3] = (offset / 100000) % 10;
    vfo->rxtone = rxtone;
    vfo->txtone = txtone;
    vfo->lowpower = lowpower;
    vfo->narrow = ! wide;
    vfo->step = step;
    vfo->scode = scode;
}

static void print_offset (FILE *out, int delta)
{
    if (delta == 0) {
        fprintf (out, " 0      ");
    } else {
        if (delta > 0) {
            fprintf (out, "+");;
        } else {
            fprintf (out, "-");;
            delta = - delta;
        }
        if (delta % 1000000 == 0)
            fprintf (out, "%-7u", delta / 1000000);
        else
            fprintf (out, "%-7.3f", delta / 1000000.0);
    }
}

static void print_squelch (FILE *out, int ctcs, int dcs)
{
    if      (ctcs)    fprintf (out, "%5.1f", ctcs / 10.0);
    else if (dcs > 0) fprintf (out, "D%03dN", dcs);
    else if (dcs < 0) fprintf (out, "D%03dI", -dcs);
    else              fprintf (out, "   - ");
}

static void print_vfo (FILE *out, char name, int band, int hz, int offset,
    int rx_ctcs, int tx_ctcs, int rx_dcs, int tx_dcs,
    int lowpower, int wide, int step, int scode)
{
    fprintf (out, " %c  %3s  %8.4f ", name, band ? "UHF" : "VHF", hz / 1000000.0);
    print_offset (out, offset);
    fprintf (out, " ");
    print_squelch (out, rx_ctcs, rx_dcs);
    fprintf (out, "   ");
    print_squelch (out, tx_ctcs, tx_dcs);

    char sgroup [8];
    if (scode == 0)
        strcpy (sgroup, "-");
    else
        sprintf (sgroup, "%u", scode);

    fprintf (out, "   %-4s %-4s  %-6s %-3s\n", STEP_NAME[step],
        lowpower ? "Low" : "High", wide ? "Wide" : "Narrow", sgroup);
}

//
// Generic settings.
//
typedef struct {
    uint8_t squelch;    // Carrier Squelch Level
    uint8_t step;
    uint8_t _u1;
    uint8_t save;       // Battery Saver
    uint8_t vox;        // VOX Level
    uint8_t _u2;
    uint8_t abr;        // Backlight Timeout
    uint8_t tdr;        // Dual Watch
    uint8_t beep;       // Beep
    uint8_t timeout;    // Timeout Timer
    uint8_t _u3 [4];
    uint8_t voice;      // Voice
    uint8_t _u4;
    uint8_t dtmfst;     // DTMF Sidetone
    uint8_t _u5;
    uint8_t screv;      // Scan Resume
    uint8_t pttid;
    uint8_t pttlt;
    uint8_t mdfa;       // Display Mode (A)
    uint8_t mdfb;       // Display Mode (B)
    uint8_t bcl;        // Busy Channel Lockout
    uint8_t autolk;     // Automatic Key Lock
    uint8_t sftd;
    uint8_t _u6 [3];
    uint8_t wtled;      // Standby LED Color
    uint8_t rxled;      // RX LED Color
    uint8_t txled;      // TX LED Color
    uint8_t almod;      // Alarm Mode
    uint8_t band;
    uint8_t tdrab;      // Dual Watch Priority
    uint8_t ste;        // Squelch Tail Eliminate (HT to HT)
    uint8_t rpste;      // Squelch Tail Eliminate (repeater)
    uint8_t rptrl;      // STE Repeater Delay
    uint8_t ponmsg;     // Power-On Message
    uint8_t roger;      // Roger Beep
} settings_t;

//
// Transient modes.
//
typedef struct {
    uint8_t displayab  : 1,     // Display
            _u1        : 2,
            fmradio    : 1,     // Broadcast FM Radio
            alarm      : 1,
            _u2        : 1,
            reset      : 1,     // RESET Menu
            menu       : 1;     // All Menus
    uint8_t _u3;
    uint8_t workmode;           // VFO/MR Mode
    uint8_t keylock;            // Keypad Lock
} extra_settings_t;

//
// Print full information about the device configuration.
//
static void print_config (FILE *out, int verbose, int is_aged)
{
    int i;

    // Power-on message.
    if (verbose) {
        fprintf (out, "\n");
        fprintf (out, "# Display this message on power-on.\n");
        fprintf (out, "# 14 characters split into two lines of 7 symbols each.\n");
    }
    fprintf (out, "Message: %.16s\n", &radio_mem[0x1EC0+0x20]);

    // Print memory channels.
    fprintf (out, "\n");
    if (verbose) {
        fprintf (out, "# Table of preprogrammed channels.\n");
        fprintf (out, "# 1) Channel number: 0-%d\n", NCHAN-1);
        fprintf (out, "# 2) Name: up to 7 characters, no spaces\n");
        fprintf (out, "# 3) Receive frequency in MHz\n");
        fprintf (out, "# 4) Offset of transmit frequency in MHz, or '-' to disable transmit\n");
        fprintf (out, "# 5) Squelch tone for receive, or '-' to disable\n");
        fprintf (out, "# 6) Squelch tone for transmit, or '-' to disable\n");
        fprintf (out, "# 7) Transmit power: Low, High\n");
        fprintf (out, "# 8) Modulation width: Wide, Narrow\n");
        fprintf (out, "# 9) Add this channel to scan list: -, +\n");
        fprintf (out, "# 10) Busy channel lockout: -, +\n");
        fprintf (out, "# 11) Last (6-th) character of ANI code, or '-'\n");
        fprintf (out, "# 12) Transmit PTT ID (ANI code) on this channel: -, +\n");
        fprintf (out, "#\n");
    }
    fprintf (out, "Channel Name    Receive  TxOffset R-Squel T-Squel Power FM     Scan BCL Scode PTTID\n");
    for (i=0; i<NCHAN; i++) {
        int rx_hz, tx_hz, rx_ctcs, tx_ctcs, rx_dcs, tx_dcs;
        int lowpower, wide, scan, bcl, pttid, scode;
        char name[17];

        decode_channel (i, name, &rx_hz, &tx_hz, &rx_ctcs, &tx_ctcs,
            &rx_dcs, &tx_dcs, &lowpower, &wide, &scan, &bcl, &pttid, &scode);
        if (rx_hz == 0) {
            // Channel is disabled
            continue;
        }

        fprintf (out, "%5d   %-7s %8.4f ",
            i, name[0] ? name : "-", rx_hz / 1000000.0);

        if (is_valid_frequency (tx_hz / 1000000.0)) {
            print_offset (out, tx_hz - rx_hz);
        } else {
            fprintf (out, " -      ");
        }

        fprintf (out, " ");
        print_squelch (out, rx_ctcs, rx_dcs);
        fprintf (out, "   ");
        print_squelch (out, tx_ctcs, tx_dcs);

        char sgroup [8];
        if (scode == 0)
            strcpy (sgroup, "-");
        else
            sprintf (sgroup, "%u", scode);

        fprintf (out, "   %-4s  %-6s %-4s %-3s %-5s %-4s\n", lowpower ? "Low" : "High",
            wide ? "Wide" : "Narrow", scan ? "+" : "-", bcl ? "+" : "-",
            sgroup, PTTID_NAME[pttid]);
    }
    if (verbose)
        print_squelch_tones (out, 0);

    // Print frequency mode VFO settings.
    int band, hz, offset, rx_ctcs, tx_ctcs, rx_dcs, tx_dcs;
    int lowpower, wide, step, scode;
    fprintf (out, "\n");
    if (verbose) {
        fprintf (out, "# Table of VFO settings.\n");
        fprintf (out, "# 1) VFO index: A, B\n");
        fprintf (out, "# 2) Band: VHF, UHF\n");
        fprintf (out, "# 3) Receive frequency in MHz\n");
        fprintf (out, "# 4) Offset of transmit frequency in MHz\n");
        fprintf (out, "# 5) Squelch tone for receive, or '-' to disable\n");
        fprintf (out, "# 6) Squelch tone for transmit, or '-' to disable\n");
        fprintf (out, "# 7) Frequency step in kHz: 2.5, 5.0, 6.25, 10.0, 12.5, 20.0, 25.0, 50.0\n");
        fprintf (out, "# 8) Transmit power: Low, High\n");
        fprintf (out, "# 9) Modulation width: Wide, Narrow\n");
        fprintf (out, "# 10) Last (6-th) character of ANI code, or '-'\n");
        fprintf (out, "#\n");
    }
    decode_vfo (0, &band, &hz, &offset, &rx_ctcs, &tx_ctcs,
        &rx_dcs, &tx_dcs, &lowpower, &wide, &step, &scode);
    fprintf (out, "VFO Band Receive  TxOffset R-Squel T-Squel Step Power FM     Scode\n");
    print_vfo (out, 'A', band, hz, offset, rx_ctcs, tx_ctcs,
        rx_dcs, tx_dcs, lowpower, wide, step, scode);
    decode_vfo (1, &band, &hz, &offset, &rx_ctcs, &tx_ctcs,
        &rx_dcs, &tx_dcs, &lowpower, &wide, &step, &scode);
    print_vfo (out, 'B', band, hz, offset, rx_ctcs, tx_ctcs,
        rx_dcs, tx_dcs, lowpower, wide, step, scode);

    if (! is_aged) {
        // Print band limits.
        int vhf_enable, vhf_lower, vhf_upper, uhf_enable, uhf_lower, uhf_upper;
        decode_limits ('V', &vhf_enable, &vhf_lower, &vhf_upper);
        decode_limits ('U', &uhf_enable, &uhf_lower, &uhf_upper);
        fprintf (out, "\n");
        if (verbose) {
            fprintf (out, "# Table of band limits.\n");
            fprintf (out, "# 1) Band: VHF, UHF\n");
            fprintf (out, "# 2) Lower frequency in MHz\n");
            fprintf (out, "# 3) Upper frequency in MHz\n");
            fprintf (out, "# 4) Enable flag: -, +\n");
            fprintf (out, "#\n");
        }
        fprintf (out, "Limit Lower Upper Enable\n");
        fprintf (out, " VHF  %4d  %4d  %s\n", vhf_lower, vhf_upper,
            vhf_enable ? "+" : "-");
        fprintf (out, " UHF  %4d  %4d  %s\n", uhf_lower, uhf_upper,
            uhf_enable ? "+" : "-");
    }

    // Get atomatic number identifier.
    char ani[5];
    fetch_ani (ani);

    // Print other settings.
    settings_t *mode = (settings_t*) &radio_mem[0x0E20];
    fprintf (out, "\n");

    if (verbose) {
        fprintf (out, "# Mute the speaker when a received signal is below this level.\n");
        fprintf (out, "# Options: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9\n");
    }
    fprintf (out, "Squelch Level: %u\n", mode->squelch);

    if (verbose)
        print_options (out, SAVER_NAME, 5, "Decrease the amount of power used when idle.");
    fprintf (out, "Battery Saver: %s\n", SAVER_NAME[mode->save & 7]);

    if (verbose)
        print_options (out, VOX_NAME, 11, "Microphone sensitivity for VOX control.");
    fprintf (out, "VOX Level: %s\n", VOX_NAME[mode->vox & 15]);

    if (verbose)
        print_options (out, ABR_NAME, 6, "Number of seconds for display backlight.");
    fprintf (out, "Backlight Timeout: %s\n", ABR_NAME[mode->abr & 7]);

    if (verbose)
        print_options (out, OFF_ON, 2, "Automatically switch A/B when signal is received on another frequency.");
    fprintf (out, "Dual Watch: %s\n", mode->tdr ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Keypad beep sound.");
    fprintf (out, "Keypad Beep: %s\n", mode->beep ? "On" : "Off");

    if (verbose) {
        fprintf (out, "\n# Stop tramsmittion after specified number of seconds.\n");
        fprintf (out, "# Options: 15, 30, 45, 60, ... 585, 600\n");
    }
    fprintf (out, "TX Timer: %u\n", (mode->timeout + 1) * 15);

    if (verbose)
        print_options (out, VOICE_NAME, 3, "Enable voice messages.");
    fprintf (out, "Voice Prompt: %s\n", VOICE_NAME[mode->voice & 3]);

    if (verbose) {
        fprintf (out, "\n# Automatic number identification: first 5 characters of\n");
        fprintf (out, "# PTT ID code, which is transmitted on PTT button press and/or release.\n");
        fprintf (out, "# Last, 6-th character of ANI code is programmed individually\n");
        fprintf (out, "# for every channel (see Scode above).\n");
        fprintf (out, "# Characters allowed: 0 1 2 3 4 5 6 7 8 9 A B C D E F\n");
    }
    fprintf (out, "ANI Code: %c%c%c%c%c\n", ani[0], ani[1], ani[2], ani[3], ani[4]);

    if (verbose)
        print_options (out, DTMF_SIDETONE_NAME, 4, "Play DTMF tones when keycode or PTT ID is transmitted.");
    fprintf (out, "DTMF Sidetone: %s\n", DTMF_SIDETONE_NAME[mode->dtmfst & 3]);

    if (verbose) {
        fprintf (out, "\n# Method of resuming the scan after stop on active channel.\n");
        fprintf (out, "# TO - resume after a timeout.\n");
        fprintf (out, "# CO - resume after a carrier dropped off.\n");
        fprintf (out, "# SE - search and stop on next active frequency.\n");
    }
    fprintf (out, "Scan Resume: %s\n", SCAN_RESUME_NAME[mode->screv & 3]);

    if (verbose)
        print_options (out, DISPLAY_MODE_NAME, 3, "What information to display for channel A.");
    fprintf (out, "Display Mode A: %s\n", DISPLAY_MODE_NAME[mode->mdfa & 3]);

    if (verbose)
        print_options (out, DISPLAY_MODE_NAME, 3, "What information to display for channel B.");
    fprintf (out, "Display Mode B: %s\n", DISPLAY_MODE_NAME[mode->mdfb & 3]);

    if (verbose)
        print_options (out, OFF_ON, 2, "Prevent transmittion when a signal is received.");
    fprintf (out, "Busy Channel Lockout: %s\n", mode->bcl ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Lock keypad automatically.");
    fprintf (out, "Auto Key Lock: %s\n", mode->autolk ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Color of display backlight when idle.");
    fprintf (out, "Standby LED Color: %s\n", COLOR_NAME[mode->wtled & 3]);

    if (verbose)
        print_options (out, OFF_ON, 2, "Color of display backlight when signal is received.");
    fprintf (out, "RX LED Color: %s\n", COLOR_NAME[mode->rxled & 3]);

    if (verbose)
        print_options (out, OFF_ON, 2, "Color of display backlight when transmitting.");
    fprintf (out, "TX LED Color: %s\n", COLOR_NAME[mode->txled & 3]);

    if (verbose) {
        fprintf (out, "\n# When alarm button is pressed:\n");
        fprintf (out, "# Site - play local alarm sound, no transmit.\n");
        fprintf (out, "# Tone - transmit an intermittent sound to remote station.\n");
        fprintf (out, "# Code - transmit a DTMF code (PTT ID) to remote station.\n");
    }
    fprintf (out, "Alarm Mode: %s\n", ALARM_NAME[mode->almod & 3]);

    if (verbose)
        print_options (out, OFF_ON, 2, "Reduce the squelch tail when communicating with simplex station.");
    fprintf (out, "Squelch Tail Eliminate: %s\n", mode->ste ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Reduce the squelch tail when communicating via repeater.");
    fprintf (out, "Squelch Tail Eliminate for Repeater: %s\n", RPSTE_NAME[mode->rpste & 15]);

    if (verbose)
        print_options (out, OFF_ON, 2, "Delay the squelch tail for repeater.");
    fprintf (out, "Squelch Tail Repeater Delay: %s\n", RPSTE_NAME[mode->rptrl & 15]);

    if (verbose)
        print_options (out, OFF_ON, 2, "Display the power-on message (see above).");
    fprintf (out, "Power-On Message: %s\n", mode->ponmsg ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Transmit 'roger' tone when PTT released.");
    fprintf (out, "Roger Beep: %s\n", mode->roger ? "On" : "Off");
}

//
// Print full information about the device configuration.
//
static void uv5r_print_config (FILE *out, int verbose)
{
    print_config (out, verbose, 0);
}

static void aged_print_config (FILE *out, int verbose)
{
    print_config (out, verbose, 1);
}

//
// Read memory image from the binary file.
//
static void uv5r_read_image (FILE *img, unsigned char *ident)
{
    if (fread (ident, 1, 8, img) != 8) {
        fprintf (stderr, "Error reading image header.\n");
        exit (-1);
    }
    if (fread (&radio_mem[0], 1, 0x1800, img) != 0x1800) {
        fprintf (stderr, "Error reading image data.\n");
        exit (-1);
    }
    if (fread (&radio_mem[0x1EC0], 1, 0x2000-0x1EC0, img) != 0x2000-0x1EC0) {
        fprintf (stderr, "Error reading image footer.\n");
        exit (-1);
    }
}

static void aged_read_image (FILE *img, unsigned char *ident)
{
    if (fread (ident, 1, 8, img) != 8) {
        fprintf (stderr, "Error reading image header.\n");
        exit (-1);
    }
    if (fread (&radio_mem[0], 1, 0x1800, img) != 0x1800) {
        fprintf (stderr, "Error reading image data.\n");
        exit (-1);
    }
}

//
// Save memory image to the binary file.
//
static void uv5r_save_image (FILE *img)
{
    fwrite (radio_ident, 1, 8, img);
    fwrite (&radio_mem[0], 1, 0x1800, img);
    fwrite (&radio_mem[0x1EC0], 1, 0x2000-0x1EC0, img);
}

static void aged_save_image (FILE *img)
{
    fwrite (radio_ident, 1, 8, img);
    fwrite (&radio_mem[0], 1, 0x1800, img);
}

//
// Read the configuration from text file, and modify the image.
//
static void parse_parameter (char *param, char *value, int is_aged)
{
    settings_t *mode = (settings_t*) &radio_mem[0x0E20];
    int i;

    if (strcasecmp ("Radio", param) == 0) {
        if (strcasecmp ("Baofeng UV-5R", value) != 0) {
bad:        fprintf (stderr, "Bad value for %s: %s\n", param, value);
            exit(-1);
        }
        return;
    }
    if (! is_aged) {
        // Only new firmware has power-on messages.
        if (strcasecmp ("Serial", param) == 0) {
            copy_str (&radio_mem[0x1EC0+0x10], value, 14);
            return;
        }
        if (strcasecmp ("Firmware", param) == 0) {
            // Do nmot try to change firmware string.
            // It will reset all the settings to defaults.
            return;
        }
        if (strcasecmp ("Message", param) == 0) {
            copy_str (&radio_mem[0x1EC0+0x20], value, 14);
            return;
        }
    }
    if (strcasecmp ("Squelch Level", param) == 0) {
        mode->squelch = atoi (value);
        return;
    }
    if (strcasecmp ("Battery Saver", param) == 0) {
        mode->save = atoi_off (value);
        return;
    }
    if (strcasecmp ("VOX Level", param) == 0) {
        mode->vox = atoi_off (value);
        return;
    }
    if (strcasecmp ("Backlight Timeout", param) == 0) {
        mode->abr = atoi_off (value);
        return;
    }
    if (strcasecmp ("Dual Watch", param) == 0) {
        mode->tdr = on_off (param, value);
        return;
    }
    if (strcasecmp ("Keypad Beep", param) == 0) {
        mode->beep = on_off (param, value);
        return;
    }
    if (strcasecmp ("TX Timer", param) == 0) {
        mode->timeout = atoi (value) / 15;
        if (mode->timeout > 0)
             mode->timeout -= 1;
        return;
    }
    if (strcasecmp ("Voice Prompt", param) == 0) {
        i = string_in_table (value, VOICE_NAME, 3);
        if (i < 0)
            goto bad;
        mode->voice = i;
        return;
    }
    if (strcasecmp ("ANI Code", param) == 0) {
        if (strlen (value) != 5) {
            fprintf (stderr, "Five hex digits expected.\n");
            goto bad;
        }
        setup_ani (value);
        return;
    }
    if (strcasecmp ("DTMF Sidetone", param) == 0) {
        i = string_in_table (value, DTMF_SIDETONE_NAME, 4);
        if (i < 0)
            goto bad;
        mode->dtmfst = i;
        return;
    }
    if (strcasecmp ("Scan Resume", param) == 0) {
        i = string_in_table (value, SCAN_RESUME_NAME, 3);
        if (i < 0)
            goto bad;
        mode->screv = i;
        return;
    }
    if (strcasecmp ("Display Mode A", param) == 0) {
        i = string_in_table (value, DISPLAY_MODE_NAME, 3);
        if (i < 0)
            goto bad;
        mode->mdfa = i;
        return;
    }
    if (strcasecmp ("Display Mode B", param) == 0) {
        i = string_in_table (value, DISPLAY_MODE_NAME, 3);
        if (i < 0)
            goto bad;
        mode->mdfb = i;
        return;
    }
    if (strcasecmp ("Busy Channel Lockout", param) == 0) {
        mode->bcl = on_off (param, value);
        return;
    }
    if (strcasecmp ("Auto Key Lock", param) == 0) {
        mode->autolk = on_off (param, value);
        return;
    }
    if (strcasecmp ("Standby LED Color", param) == 0) {
        i = string_in_table (value, COLOR_NAME, 4);
        if (i < 0)
            goto bad;
        mode->wtled = i;
        return;
    }
    if (strcasecmp ("RX LED Color", param) == 0) {
        i = string_in_table (value, COLOR_NAME, 4);
        if (i < 0)
            goto bad;
        mode->rxled = i;
        return;
    }
    if (strcasecmp ("TX LED Color", param) == 0) {
        i = string_in_table (value, COLOR_NAME, 4);
        if (i < 0)
            goto bad;
        mode->txled = i;
        return;
    }
    if (strcasecmp ("Alarm Mode", param) == 0) {
        i = string_in_table (value, ALARM_NAME, 3);
        if (i < 0)
            goto bad;
        mode->almod = i;
        return;
    }
    if (strcasecmp ("Squelch Tail Eliminate", param) == 0) {
        mode->ste = on_off (param, value);
        return;
    }
    if (strcasecmp ("Squelch Tail Eliminate for Repeater", param) == 0) {
        mode->rpste = atoi_off (value);
        return;
    }
    if (strcasecmp ("Squelch Tail Repeater Delay", param) == 0) {
        mode->rptrl = atoi_off (value);
        return;
    }
    if (strcasecmp ("Power-On Message", param) == 0) {
        mode->ponmsg = on_off (param, value);
        return;
    }
    if (strcasecmp ("Roger Beep", param) == 0) {
        mode->roger = on_off (param, value);
        return;
    }
    fprintf (stderr, "Unknown parameter: %s = %s\n", param, value);
    exit(-1);
}

static void uv5r_parse_parameter (char *param, char *value)
{
    parse_parameter (param, value, 0);
}

static void aged_parse_parameter (char *param, char *value)
{
    parse_parameter (param, value, 1);
}

//
// Parse table header.
// Return table id, or 0 in case of error.
//
static int uv5r_parse_header (char *line)
{
    if (strncasecmp (line, "Channel", 7) == 0)
        return 'C';
    if (strncasecmp (line, "VFO", 3) == 0)
        return 'V';
    if (strncasecmp (line, "Limit", 5) == 0)
        return 'L';
    return 0;
}

//
// Parse one row in the Channels table.
// Return 0 on failure.
// Channel Name    Receive  TxOffset R-Squel T-Squel Power FM     Scan BCL ID6 PTTID
//     0   WR6ABD  442.9000 +5       162.2   162.2   High  Wide   +    -   -   -
//    93   K6GL    145.1700 -0.600    94.8    94.8   High  Wide   +    -   -   -
//
static int parse_channel (int first_row, char *line)
{
    char num_str[256], name_str[256], rxfreq_str[256], offset_str[256];
    char rq_str[256], tq_str[256], power_str[256], wide_str[256];
    char scan_str[256], bcl_str[256], scode_str[256], pttid_str[256];
    int num, rq, tq, lowpower, wide, scan, bcl, scode, pttid;
    double rx_mhz, txoff_mhz;

    if (sscanf (line, "%s %s %s %s %s %s %s %s %s %s %s %s",
        num_str, name_str, rxfreq_str, offset_str, rq_str, tq_str,
        power_str, wide_str, scan_str, bcl_str, scode_str, pttid_str) != 12)
        return 0;

    num = atoi (num_str);
    if (num < 0 || num >= NCHAN) {
        fprintf (stderr, "Bad channel number.\n");
        return 0;
    }
    if (sscanf (rxfreq_str, "%lf", &rx_mhz) != 1 ||
        ! is_valid_frequency (rx_mhz))
    {
        fprintf (stderr, "Bad receive frequency.\n");
        return 0;
    }
    if (strcmp("-", offset_str) == 0) {
        // tx disabled; set offset outside range so setup_channel will disable it.
        txoff_mhz = -999.9;
    } else if (sscanf (offset_str, "%lf", &txoff_mhz) != 1 ||
        ! is_valid_frequency (rx_mhz + txoff_mhz))
    {
        fprintf (stderr, "Bad transmit offset.\n");
        return 0;
    }
    rq = encode_squelch (rq_str);
    tq = encode_squelch (tq_str);

    if (strcasecmp ("High", power_str) == 0) {
        lowpower = 0;
    } else if (strcasecmp ("Low", power_str) == 0) {
        lowpower = 1;
    } else {
        fprintf (stderr, "Bad power level.\n");
        return 0;
    }

    if (strcasecmp ("Wide", wide_str) == 0) {
        wide = 1;
    } else if (strcasecmp ("Narrow", wide_str) == 0) {
        wide = 0;
    } else {
        fprintf (stderr, "Bad modulation width.\n");
        return 0;
    }

    if (*scan_str == '+') {
        scan = 1;
    } else if (*scan_str == '-') {
        scan = 0;
    } else {
        fprintf (stderr, "Bad scan flag.\n");
        return 0;
    }

    if (*bcl_str == '+') {
        bcl = 1;
    } else if (*bcl_str == '-') {
        bcl = 0;
    } else {
        fprintf (stderr, "Bad BCL flag.\n");
        return 0;
    }

    if (*scode_str == '-') {
        scode = 0;
    } else if (*scode_str >= '0' && *scode_str <= '9') {
        scode = *scode_str - '0';
    } else if (*scode_str >= 'A' && *scode_str <= 'A') {
        scode = *scode_str - 'A' + 10;
    } else if (*scode_str >= 'a' && *scode_str <= 'a') {
        scode = *scode_str - 'a' + 10;
    } else {
        fprintf (stderr, "Bad scode value.\n");
        return 0;
    }

    pttid = string_in_table (pttid_str, PTTID_NAME, 4);
    if (pttid < 0) {
        fprintf (stderr, "Bad pttid mode.\n");
        return 0;
    }

    if (first_row) {
        // On first entry, erase the channel table.
        int i;
        for (i=0; i<NCHAN; i++) {
            erase_channel (i);
        }
    }
    setup_channel (num, name_str, rx_mhz, rx_mhz + txoff_mhz,
        rq, tq, lowpower, wide, scan, bcl, scode, pttid);
    return 1;
}

//
// Parse one row in the VFO table.
// VFO Band Receive  TxOffset R-Squel T-Squel Step Power FM     Scode
//  A  UHF  443.9300  0          -       -    2.5  High  Wide   -
//  B  VHF  145.2300 +6          -       -    5.0  High  Wide   -
//
static int parse_vfo (int first_row, char *line)
{
    char num_str[256], band_str[256], rxfreq_str[256], offset_str[256];
    char rq_str[256], tq_str[256], step_str[256];
    char power_str[256], wide_str[256], scode_str[256];
    int num, band, rq, tq, step, lowpower, wide, scode;
    double rx_mhz, txoff_mhz;

    if (sscanf (line, "%s %s %s %s %s %s %s %s %s %s",
        num_str, band_str, rxfreq_str, offset_str, rq_str, tq_str,
        step_str, power_str, wide_str, scode_str) != 10)
        return 0;

    if (*num_str == 'A' || *num_str == 'a')
        num = 0;
    else if (*num_str == 'B' || *num_str == 'b')
        num = 1;
    else  {
        fprintf (stderr, "Bad VFO number.\n");
        return 0;
    }

    if (strcasecmp ("VHF", band_str) == 0) {
        band = 'V';
    } else if (strcasecmp ("UHF", band_str) == 0) {
        band = 'U';
    } else {
        fprintf (stderr, "Unknown band.\n");
        return 0;
    }

    if (sscanf (rxfreq_str, "%lf", &rx_mhz) != 1 ||
        ! is_valid_frequency (rx_mhz))
    {
        fprintf (stderr, "Bad receive frequency.\n");
        return 0;
    }
    if (sscanf (offset_str, "%lf", &txoff_mhz) != 1 ||
        ! is_valid_frequency (rx_mhz + txoff_mhz))
    {
        fprintf (stderr, "Bad transmit offset.\n");
        return 0;
    }
    rq = encode_squelch (rq_str);
    tq = encode_squelch (tq_str);

    step = string_in_table (step_str, STEP_NAME, 8);
    if (step < 0) {
        fprintf (stderr, "Bad step.\n");
        return 0;
    }

    if (strcasecmp ("High", power_str) == 0) {
        lowpower = 0;
    } else if (strcasecmp ("Low", power_str) == 0) {
        lowpower = 1;
    } else {
        fprintf (stderr, "Bad power level.\n");
        return 0;
    }

    if (strcasecmp ("Wide", wide_str) == 0) {
        wide = 1;
    } else if (strcasecmp ("Narrow", wide_str) == 0) {
        wide = 0;
    } else {
        fprintf (stderr, "Bad modulation width.\n");
        return 0;
    }

    if (*scode_str == '-') {
        scode = 0;
    } else if (*scode_str >= '0' && *scode_str <= '9') {
        scode = *scode_str - '0';
    } else if (*scode_str >= 'A' && *scode_str <= 'A') {
        scode = *scode_str - 'A' + 10;
    } else if (*scode_str >= 'a' && *scode_str <= 'a') {
        scode = *scode_str - 'a' + 10;
    } else {
        fprintf (stderr, "Bad scode value.\n");
        return 0;
    }

    setup_vfo (num, band, rx_mhz * 1000000, txoff_mhz * 1000000,
        rq, tq, step, lowpower, wide, scode);
    return 1;
}

//
// Parse one row in the Limits table.
// Limit Lower Upper Enable
//  VHF   136   174  +
//  UHF   400   520  +
//
static int parse_limit (int first_row, char *line)
{
    char band_str[256], lower_str[256], upper_str[256], enable_str[256];
    int lower, upper, enable;

    if (sscanf (line, "%s %s %s %s",
        band_str, lower_str, upper_str, enable_str) != 4)
        return 0;

    lower = atoi (lower_str);
    upper = atoi (upper_str);
    if (*enable_str == '+') {
        enable = 1;
    } else if (*enable_str == '-') {
        enable = 0;
    } else {
        fprintf (stderr, "Bad enable flag.\n");
        return 0;
    }

    if (strcasecmp ("VHF", band_str) == 0) {
        setup_limits ('V', enable, lower, upper);
    } else if (strcasecmp ("UHF", band_str) == 0) {
        setup_limits ('U', enable, lower, upper);
    } else {
        fprintf (stderr, "Unknown band.\n");
        return 0;
    }
    return 1;
}

static int uv5r_parse_row (int table_id, int first_row, char *line)
{
    switch (table_id) {
    case 'C': return parse_channel (first_row, line);
    case 'V': return parse_vfo (first_row, line);
    case 'L': return parse_limit (first_row, line);
    }
    return 0;
}

//
// Baofeng UV-5R, UV-5RA
//
radio_device_t radio_uv5r = {
    "Baofeng UV-5R",
    uv5r_download,
    uv5r_upload,
    uv5r_read_image,
    uv5r_save_image,
    uv5r_print_version,
    uv5r_print_config,
    uv5r_parse_parameter,
    uv5r_parse_header,
    uv5r_parse_row,
};

//
// Baofeng UV-5R with old firmware
//
radio_device_t radio_uv5r_aged = {
    "Baofeng UV-5R Aged",
    aged_download,
    aged_upload,
    aged_read_image,
    aged_save_image,
    aged_print_version,
    aged_print_config,
    aged_parse_parameter,
    uv5r_parse_header,      // Use the same routines
    uv5r_parse_row,         // for tables
};
