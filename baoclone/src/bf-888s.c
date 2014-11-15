/*
 * Interface to Baofeng BF-888S and compatibles.
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

#define NCHAN 16

static const char *SIDEKEY_NAME[] = { "Off", "Monitor", "TX Power", "Alarm" };

static const char *OFF_ON[] = { "Off", "On" };

//
// Print a generic information about the device.
//
static void bf888s_print_version (FILE *out)
{
    // Nothing to print.
}

//
// Read block of data, up to 8 bytes.
// Halt the program on any error.
//
static void read_block (int fd, int start, unsigned char *data, int nbytes)
{
    unsigned char cmd[4], reply[4];
    int addr, len;

    // Send command.
    cmd[0] = 'R';
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
    if (reply[0] != 'W' || addr != start || reply[3] != nbytes) {
        fprintf (stderr, "Bad reply for block 0x%04x of %d bytes: %02x-%02x-%02x-%02x\n",
            start, nbytes, reply[0], reply[1], reply[2], reply[3]);
        exit(-1);
    }

    // Read data.
    len = serial_read (fd, data, 8);
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
        if (radio_progress % 4 == 0) {
            fprintf (stderr, "#");
            fflush (stderr);
        }
    }
}

//
// Write block of data, up to 8 bytes.
// Halt the program on any error.
//
static void write_block (int fd, int start, const unsigned char *data, int nbytes)
{
    unsigned char cmd[4], reply;

    // Send command.
    cmd[0] = 'W';
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
        if (radio_progress % 4 == 0) {
            fprintf (stderr, "#");
            fflush (stderr);
        }
    }
}

//
// Read memory image from the device.
//
static void bf888s_download()
{
    int addr;

    memset (radio_mem, 0xff, 0x400);
    for (addr=0x10; addr<0x110; addr+=8)
        read_block (radio_port, addr, &radio_mem[addr], 8);
    for (addr=0x2b0; addr<0x2c0; addr+=8)
        read_block (radio_port, addr, &radio_mem[addr], 8);
    for (addr=0x3c0; addr<0x3e0; addr+=8)
        read_block (radio_port, addr, &radio_mem[addr], 8);
}

//
// Write memory image to the device.
//
static void bf888s_upload (int cont_flag)
{
    int addr;

    for (addr=0x10; addr<0x110; addr+=8)
        write_block (radio_port, addr, &radio_mem[addr], 8);
    for (addr=0x2b0; addr<0x2c0; addr+=8)
        write_block (radio_port, addr, &radio_mem[addr], 8);
    for (addr=0x3c0; addr<0x3e0; addr+=8)
        write_block (radio_port, addr, &radio_mem[addr], 8);
}

static void decode_squelch (uint16_t bcd, int *ctcs, int *dcs)
{
    if (bcd == 0 || bcd == 0xffff) {
        // Squelch disabled.
        return;
    }
    int index = ((bcd >> 12) & 15) * 1000 +
                ((bcd >> 8)  & 15) * 100 +
                ((bcd >> 4)  & 15) * 10 +
                (bcd         & 15);

    if (index < 8000) {
        // CTCSS value is Hz multiplied by 10.
        *ctcs = index;
        *dcs = 0;
        return;
    }
    // DCS mode.
    if (index < 12000)
        *dcs = index - 8000;
    else
        *dcs = - (index - 12000);
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
        if (val < 1 || val >= 999)
            return 0;

        if (*e == 'N' || *e == 'n') {
            val += 8000;
        } else if (*e == 'I' || *e == 'i') {
            val += 12000;
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
    } else {
        // Disabled
        return 0;
    }

    int bcd = ((val / 1000) % 16) << 12 |
              ((val / 100)  % 10) << 8 |
              ((val / 10)   % 10) << 4 |
              (val          % 10);
    return bcd;
}

typedef struct {
    uint32_t    rxfreq;     // binary coded decimal, 8 digits
    uint32_t    txfreq;     // binary coded decimal, 8 digits
    uint16_t    rxtone;
    uint16_t    txtone;
    uint8_t     nobcl     : 1,
                noscr     : 1,
                narrow    : 1,
                highpower : 1,
                noscan    : 1,
                _u1       : 3;
    uint8_t     _u3[3];
} memory_channel_t;

static void decode_channel (int i, int *rx_hz, int *tx_hz,
    int *rx_ctcs, int *tx_ctcs, int *rx_dcs, int *tx_dcs,
    int *lowpower, int *wide, int *scan, int *bcl, int *scramble)
{
    memory_channel_t *ch = i + (memory_channel_t*) &radio_mem[0x10];

    *rx_hz = *tx_hz = *rx_ctcs = *tx_ctcs = *rx_dcs = *tx_dcs = 0;
    if (ch->rxfreq == 0 || ch->rxfreq == 0xffffffff)
        return;

    // Decode channel frequencies.
    *rx_hz = bcd_to_int (ch->rxfreq) * 10;
    *tx_hz = bcd_to_int (ch->txfreq) * 10;

    // Decode squelch modes.
    decode_squelch (ch->rxtone, rx_ctcs, rx_dcs);
    decode_squelch (ch->txtone, tx_ctcs, tx_dcs);

    // Other parameters.
    *lowpower = ! ch->highpower;
    *wide = ! ch->narrow;
    *scan = ! ch->noscan;
    *bcl = ! ch->nobcl;
    *scramble = ! ch->noscr;
}

static void setup_channel (int i, double rx_mhz, double tx_mhz,
    int rq, int tq, int highpower, int wide, int scan, int bcl, int scramble)
{
    memory_channel_t *ch = i + (memory_channel_t*) &radio_mem[0x10];

    ch->rxfreq = int_to_bcd ((int) (rx_mhz * 100000.0));
    ch->txfreq = int_to_bcd ((int) (tx_mhz * 100000.0));
    ch->rxtone = rq;
    ch->txtone = tq;
    ch->highpower = highpower;
    ch->narrow = ! wide;
    ch->noscan = ! scan;
    ch->nobcl = ! bcl;
    ch->noscr = ! scramble;
    ch->_u1 = 7;
    ch->_u3[0] = ch->_u3[1] = ch->_u3[2] = ~0;
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

//
// Generic settings at 0x2b0.
//
typedef struct {
    uint8_t voice;      // Voice Prompt
    uint8_t chinese;    // Voice Language
    uint8_t scan;       // Scan
    uint8_t vox;        // VOX Function
    uint8_t voxgain;    // VOX Level (0='1' ... 4='5')
    uint8_t voxinhrx;   // VOX Inhibit On Receive
    uint8_t lowinhtx;   // Low Vol Inhibit Tx
    uint8_t highinhtx;  // High Vol Inhibit Tx
    uint8_t alarm;      // Alarm
    uint8_t fm;         // FM Radio
} settings_t;

//
// Extra settings at 0x3c0.
//
typedef struct {
    uint8_t beep  : 1,  // Beep
            saver : 1,  // Battery Saver
            _u1   : 6;
    uint8_t squelch;    // Carrier Squelch Level (0-9)
    uint8_t sidekey;    // Side Key (0=Off, 1=Monitor, 2=TX Power, 3=Alarm)
    uint8_t timeout;    // TX Timer (0-10 multiply 30 sec)
} extra_settings_t;

//
// Print full information about the device configuration.
//
static void bf888s_print_config (FILE *out, int verbose)
{
    int i;

    // Print memory channels.
    fprintf (out, "\n");
    if (verbose) {
        fprintf (out, "# Table of preprogrammed channels.\n");
        fprintf (out, "# 1) Channel number: 1-%d\n", NCHAN);
        fprintf (out, "# 2) Receive frequency in MHz\n");
        fprintf (out, "# 3) Offset of transmit frequency in MHz\n");
        fprintf (out, "# 4) Squelch tone for receive, or '-' to disable\n");
        fprintf (out, "# 5) Squelch tone for transmit, or '-' to disable\n");
        fprintf (out, "# 6) Transmit power: Low, High\n");
        fprintf (out, "# 7) Modulation width: Wide, Narrow\n");
        fprintf (out, "# 8) Add this channel to scan list\n");
        fprintf (out, "# 9) Busy channel lockout\n");
        fprintf (out, "# 10) Enable scrambler\n");
        fprintf (out, "#\n");
    }
    fprintf (out, "Channel Receive  TxOffset R-Squel T-Squel Power FM     Scan BCL Scramble\n");
    for (i=0; i<NCHAN; i++) {
        int rx_hz, tx_hz, rx_ctcs, tx_ctcs, rx_dcs, tx_dcs;
        int lowpower, wide, scan, bcl, scramble;

        decode_channel (i, &rx_hz, &tx_hz, &rx_ctcs, &tx_ctcs,
            &rx_dcs, &tx_dcs, &lowpower, &wide, &scan, &bcl, &scramble);
        if (rx_hz == 0) {
            // Channel is disabled
            continue;
        }

        fprintf (out, "%5d   %8.4f ", i+1, rx_hz / 1000000.0);
        print_offset (out, tx_hz - rx_hz);
        fprintf (out, " ");
        print_squelch (out, rx_ctcs, rx_dcs);
        fprintf (out, "   ");
        print_squelch (out, tx_ctcs, tx_dcs);

        fprintf (out, "   %-4s  %-6s %-4s %-3s %s\n", lowpower ? "Low" : "High",
            wide ? "Wide" : "Narrow", scan ? "+" : "-",
            bcl ? "+" : "-", scramble ? "+" : "-");
    }
    if (verbose)
        print_squelch_tones (out, 0);

    // Print other settings.
    settings_t *mode = (settings_t*) &radio_mem[0x2b0];
    extra_settings_t *extra = (extra_settings_t*) &radio_mem[0x3c0];
    fprintf (out, "\n");

    if (verbose) {
        fprintf (out, "# Mute the speaker when a received signal is below this level.\n");
        fprintf (out, "# Options: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9\n");
    }
    fprintf (out, "Squelch Level: %u\n", extra->squelch);

    if (verbose)
        print_options (out, SIDEKEY_NAME, 4, "Function of the monitor button.");
    fprintf (out, "Side Key: %s\n", SIDEKEY_NAME[extra->sidekey & 3]);

    if (verbose) {
        fprintf (out, "\n# Stop tramsmittion after specified number of seconds.\n");
        fprintf (out, "# Options: Off, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300\n");
    }
    fprintf (out, "TX Timer: ");
    if (extra->timeout == 0) fprintf (out, "Off\n");
    else                     fprintf (out, "%u\n", extra->timeout * 30);

    if (verbose)
        print_options (out, OFF_ON, 2, "Use channel 16 as scan mode.");
    fprintf (out, "Scan Function: %s\n", mode->scan ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Enable voice messages.");
    fprintf (out, "Voice Prompt: %s\n", mode->voice ? "On" : "Off");

    if (verbose) {
        fprintf (out, "\n# Select the language of voice messages.\n");
        fprintf (out, "# Options: English, Chinese\n");
    }
    fprintf (out, "Voice Language: %s\n", mode->chinese ? "Chinese" : "English");

    if (verbose)
        print_options (out, OFF_ON, 2, "Send alarm signal when side key pressed.");
    fprintf (out, "Alarm: %s\n", mode->alarm ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Unidentified parameter.");
    fprintf (out, "FM Radio: %s\n", mode->fm ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Voice operated transmission.");
    fprintf (out, "VOX Function: %s\n", mode->vox ? "On" : "Off");

    if (verbose) {
        fprintf (out, "\n# Microphone sensitivity for VOX control.\n");
        fprintf (out, "# Options: 1, 2, 3, 4, 5\n");
    }
    fprintf (out, "VOX Level: %u\n", mode->voxgain + 1);

    if (verbose)
        print_options (out, OFF_ON, 2, "No transmittion when signal is received.");
    fprintf (out, "VOX Inhibit On Receive: %s\n", mode->voxinhrx ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Decrease the amount of power used when idle.");
    fprintf (out, "Battery Saver: %s\n", extra->saver ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Keypad beep sound.");
    fprintf (out, "Beep: %s\n", extra->beep ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Unidentified parameter.");
    fprintf (out, "High Vol Inhibit TX: %s\n", mode->highinhtx ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Disable transmitter when battery low.");
    fprintf (out, "Low Vol Inhibit TX: %s\n", mode->lowinhtx ? "On" : "Off");
}

//
// Read memory image from the binary file.
// Try to be compatible with Baofeng BF-480 software.
//
static void bf888s_read_image (FILE *img, unsigned char *ident)
{
    char buf[8];

    if (fread (ident, 1, 8, img) != 8) {
        fprintf (stderr, "Error reading image header.\n");
        exit (-1);
    }
    // Ignore next 8 bytes.
    if (fread (buf, 1, 8, img) != 8) {
        fprintf (stderr, "Error reading header.\n");
        exit (-1);
    }
    if (fread (&radio_mem[0x10], 1, 0x3d0, img) != 0x3d0) {
        fprintf (stderr, "Error reading image data.\n");
        exit (-1);
    }

    // Move 16 bytes from 0x370 to 0x2b0.
    memcpy (radio_mem+0x2b0, radio_mem+0x370, 0x10);
    memset (radio_mem+0x370, 0xff, 0x10);
}

//
// Save memory image to the binary file.
// Try to be compatible with Baofeng BF-480 software.
//
static void bf888s_save_image (FILE *img)
{
    fwrite (radio_ident, 1, 8, img);
    fwrite ("\xff\xff\xff\xff\xff\xff\xff\xff", 1, 8, img);
    fwrite (&radio_mem[0x10], 1, 0x2b0-0x10, img);
    fwrite ("\xff\xff\xff\xff\xff\xff\xff\xff", 1, 8, img);
    fwrite ("\xff\xff\xff\xff\xff\xff\xff\xff", 1, 8, img);
    fwrite (&radio_mem[0x2c0], 1, 0x370-0x2c0, img);
    fwrite (&radio_mem[0x2b0], 1, 0x10, img);
    fwrite (&radio_mem[0x380], 1, 0x3e0-0x380, img);
}

static void bf888s_parse_parameter (char *param, char *value)
{
    settings_t *mode = (settings_t*) &radio_mem[0x2b0];
    extra_settings_t *extra = (extra_settings_t*) &radio_mem[0x3c0];
    int i;

    if (strcasecmp ("Radio", param) == 0) {
        if (strcasecmp ("Baofeng BF-888S", value) != 0) {
bad:        fprintf (stderr, "Bad value for %s: %s\n", param, value);
            exit(-1);
        }
        return;
    }
    if (strcasecmp ("Squelch Level", param) == 0) {
        extra->squelch = atoi (value);
        return;
    }
    if (strcasecmp ("Side Key", param) == 0) {
        for (i=0; i<4; i++) {
            if (strcasecmp (SIDEKEY_NAME[i], value) == 0) {
                extra->sidekey = i;
                return;
            }
        }
        goto bad;
    }
    if (strcasecmp ("TX Timer", param) == 0) {
        if (strcasecmp ("Off", value) == 0) {
            extra->timeout = 0;
        } else {
            extra->timeout = atoi (value) / 30;
        }
        return;
    }
    if (strcasecmp ("Scan Function", param) == 0) {
        mode->scan = on_off (param, value);
        return;
    }
    if (strcasecmp ("Voice Prompt", param) == 0) {
        mode->voice = on_off (param, value);
        return;
    }
    if (strcasecmp ("Voice Language", param) == 0) {
        if (strcasecmp ("English", value) == 0) {
            mode->chinese = 0;
            return;
        }
        if (strcasecmp ("Chinese", value) == 0) {
            mode->chinese = 1;
            return;
        }
        goto bad;
    }
    if (strcasecmp ("Alarm", param) == 0) {
        mode->alarm = on_off (param, value);
        return;
    }
    if (strcasecmp ("FM Radio", param) == 0) {
        mode->fm = on_off (param, value);
        return;
    }
    if (strcasecmp ("VOX Function", param) == 0) {
        mode->vox = on_off (param, value);
        return;
    }
    if (strcasecmp ("VOX Level", param) == 0) {
        mode->voxgain = atoi (value);
        if (mode->voxgain > 0)
             mode->voxgain -= 1;
        return;
    }
    if (strcasecmp ("VOX Inhibit On Receive", param) == 0) {
        mode->voxinhrx = on_off (param, value);
        return;
    }
    if (strcasecmp ("Battery Saver", param) == 0) {
        extra->saver = on_off (param, value);
        return;
    }
    if (strcasecmp ("Beep", param) == 0) {
        extra->beep = on_off (param, value);
        return;
    }
    if (strcasecmp ("High Vol Inhibit TX", param) == 0) {
        mode->highinhtx = on_off (param, value);
        return;
    }
    if (strcasecmp ("Low Vol Inhibit TX", param) == 0) {
        mode->lowinhtx = on_off (param, value);
        return;
    }
    fprintf (stderr, "Unknown parameter: %s = %s\n", param, value);
    exit(-1);
}

//
// Check that the radio does support this frequency.
//
static int is_valid_frequency (int mhz)
{
    if (mhz >= 400 && mhz <= 470)
        return 1;
    return 0;
}

//
// Parse table header.
// Return table id, or 0 in case of error.
//
static int bf888s_parse_header (char *line)
{
    if (strncasecmp (line, "Channel", 7) == 0)
        return 'C';

    return 0;
}

//
// Parse one line of table data.
// Start_flag is 1 for the first table row.
// Return 0 on failure.
//
static int bf888s_parse_row (int table_id, int first_row, char *line)
{
    char num_str[256], rxfreq_str[256], offset_str[256], rq_str[256];
    char tq_str[256], power_str[256], wide_str[256], scan_str[256];
    char bcl_str[256], scramble_str[256];
    int num, rq, tq, highpower, wide, scan, bcl, scramble;
    float rx_mhz, txoff_mhz;

    if (sscanf (line, "%s %s %s %s %s %s %s %s %s %s",
        num_str, rxfreq_str, offset_str, rq_str, tq_str, power_str,
        wide_str, scan_str, bcl_str, scramble_str) != 10)
        return 0;

    num = atoi (num_str);
    if (num < 1 || num > NCHAN) {
        fprintf (stderr, "Bad channel number.\n");
        return 0;
    }
    if (sscanf (rxfreq_str, "%f", &rx_mhz) != 1 ||
        ! is_valid_frequency (rx_mhz))
    {
        fprintf (stderr, "Bad receive frequency.\n");
        return 0;
    }
    if (sscanf (offset_str, "%f", &txoff_mhz) != 1 ||
        ! is_valid_frequency (rx_mhz + txoff_mhz))
    {
        fprintf (stderr, "Bad transmit offset.\n");
        return 0;
    }
    rq = encode_squelch (rq_str);
    tq = encode_squelch (tq_str);

    if (strcasecmp ("High", power_str) == 0) {
        highpower = 1;
    } else if (strcasecmp ("Low", power_str) == 0) {
        highpower = 0;
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

    if (*scramble_str == '+') {
        scramble = 1;
    } else if (*scramble_str == '-') {
        scramble = 0;
    } else {
        fprintf (stderr, "Bad scramble flag.\n");
        return 0;
    }

    if (first_row) {
        // On first entry, erase the channel table.
        int i;
        for (i=0; i<NCHAN; i++) {
            setup_channel (i, 0, 0, 0, 0, 1, 1, 0, 0, 0);
        }
    }
    setup_channel (num-1, rx_mhz, rx_mhz + txoff_mhz, rq, tq,
        highpower, wide, scan, bcl, scramble);
    return 1;
}

//
// Baofeng BF-888S
//
radio_device_t radio_bf888s = {
    "Baofeng BF-888S",
    bf888s_download,
    bf888s_upload,
    bf888s_read_image,
    bf888s_save_image,
    bf888s_print_version,
    bf888s_print_config,
    bf888s_parse_parameter,
    bf888s_parse_header,
    bf888s_parse_row,
};
