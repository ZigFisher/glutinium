/*
 * Interface to Baofeng UV-B5 and compatibles.
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

#define NCHAN 99

static const char CHARSET[] = "0123456789- ABCDEFGHIJKLMNOPQRSTUVWXYZ/_+*";

static const char *PTTID_NAME[] = { "Off", "Begin", "End", "Both" };

static const char *STEP_NAME[] = { "5.0",  "6.25", "10.0", "12.5",
                                   "20.0", "25.0", "????", "????" };

static const char *VOX_NAME[] = { "Off", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                           "10", "?11?", "?12?", "?13?", "?14?", "?15?" };

static const char *TIMER_NAME[] = { "Off", "1", "2", "3", "4", "5", "6", "7" };

static const char *SCAN_NAME[] = { "Time", "Carrier", "Seek", "??" };

static const char *TXTDR_NAME[] = { "Current Frequency", "Frequency A", "Frequency B", "??" };

static const char *DISPLAY_MODE_NAME[] = { "Frequency", "Name", "Channel", "??" };

static const char *LANGUAGE_NAME[] = { "English", "Chinese" };

static const char *OFF_ON[] = { "Off", "On" };

//
// Print a generic information about the device.
//
static void uvb5_print_version (FILE *out)
{
}

//
// Read block of data, up to 16 bytes.
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
    len = serial_read (fd, data, 0x10);
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
    if (reply[0] != 0x74 && reply[0] != 0x78 && reply[0] != 0x1f) {
        fprintf (stderr, "Bad acknowledge after block 0x%04x: %02x\n", start, reply[0]);
        exit(-1);
    }
    if (verbose) {
        printf ("# Read 0x%04x: ", start);
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
// Write block of data, up to 16 bytes.
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
        if (radio_progress % 8 == 0) {
            fprintf (stderr, "#");
            fflush (stderr);
        }
    }
}

//
// Read memory image from the device.
//
static void uvb5_download()
{
    int addr;

    for (addr=0; addr<0x1000; addr+=0x10)
        read_block (radio_port, addr, &radio_mem[addr], 0x10);
}

//
// Write memory image to the device.
//
static void uvb5_upload (int cont_flag)
{
    int addr;

    for (addr=0; addr<0x1000; addr+=0x10)
        write_block (radio_port, addr, &radio_mem[addr], 0x10);
}

//
// Convert squelch index and polarity to CTCSS or DCS value.
// Index=0 - squelch disabled.
// Index=1...50 - CTCSS
// Index=51...154 - DCS
//
static void decode_squelch (uint8_t index, int pol, int *ctcs, int *dcs)
{
    if (index == 0) {
        // Squelch disabled.
        return;
    }
    if (index <= NCTCSS) {
        // CTCSS value is Hz multiplied by 10.
        *ctcs = CTCSS_TONES[index - 1];
        *dcs = 0;
        return;
    }
    // DCS mode.
    *dcs = DCS_CODES[index - 51];
    if (pol)
        *dcs = - *dcs;
    *ctcs = 0;
}

//
// Convert squelch string to polarity/tone value in BCD format.
// Four possible formats:
// nnn.n - CTCSS frequency
// DnnnN - DCS normal
// DnnnI - DCS inverted
// '-'   - Disabled
//
static int encode_squelch (char *str, int *pol)
{
    unsigned val;

    if (*str == 'D' || *str == 'd') {
        // DCS tone
        char *e;
        val = strtol (++str, &e, 10);

        // Find a valid index in DCS table.
        int i;
        for (i=0; i<NDCS; i++)
            if (DCS_CODES[i] == val)
                break;
        if (i >= NDCS)
            return 0;

        val = i + 51;
        if (*e == 'N' || *e == 'n') {
            *pol = 0;
        } else if (*e == 'I' || *e == 'i') {
            *pol = 1;
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

        // Find a valid index in CTCSS table.
        int i;
        for (i=0; i<NCTCSS; i++)
            if (CTCSS_TONES[i] == val)
                break;
        if (i >= NCTCSS)
            return 0;
        val = i + 1;
        *pol = 0;
    } else {
        // Disabled
        return 0;
    }

    return val;
}

typedef struct {
    uint32_t    rxfreq;     // binary coded decimal, 8 digits
    uint32_t    txoff;      // binary coded decimal, 8 digits
    uint8_t     step      : 3,
                compander : 1,
                rxpol     : 1,
                txpol     : 1,
                _u1       : 2;
    uint8_t     rxtone;
    uint8_t     txtone;
    uint8_t     shift_dir : 2,
                revfreq   : 1,
                highpower : 1,
                bcl       : 1,
                isnarrow  : 1,
                scanadd   : 1,
                pttid     : 1;
    uint8_t     _u2 [4];
} memory_channel_t;

static void decode_channel (int i, char *name, int *rx_hz, int *txoff_hz,
    int *rx_ctcs, int *tx_ctcs, int *rx_dcs, int *tx_dcs, int *step,
    int *lowpower, int *wide, int *scan, int *pttid, int *bcl,
    int *compander, int *revfreq)
{
    memory_channel_t *ch = i + (memory_channel_t*) radio_mem;

    *rx_hz = *txoff_hz = *rx_ctcs = *tx_ctcs = *rx_dcs = *tx_dcs = 0;
    *lowpower = *wide = *scan = *pttid = *bcl = *compander = 0;
    *step = *revfreq = 0;
    if (name)
        *name = 0;
    if (ch->rxfreq == 0 || ch->rxfreq == 0xffffffff)
        return;

    // Extract channel name; strip trailing FF's.
    if (name && i >= 1 && i <= NCHAN) {
        unsigned char *p = (unsigned char*) &radio_mem[0x0A00 + (i-1)*5];
        int n;
        for (n=0; n<5; n++) {
            name[n] = (*p < 42) ? CHARSET[*p++]: 0;
        }
        name[5] = 0;
    }

    // Decode channel frequencies.
    *rx_hz = bcd_to_int (ch->rxfreq) * 10;
    if (ch->shift_dir != 0) {
        *txoff_hz = bcd_to_int (ch->txoff) * 10;
        if (ch->shift_dir == 1) {
            *txoff_hz = - *txoff_hz;
        }
    }

    // Decode squelch modes.
    decode_squelch (ch->rxtone, ch->rxpol, rx_ctcs, rx_dcs);
    decode_squelch (ch->txtone, ch->txpol, tx_ctcs, tx_dcs);

    // Other parameters.
    *step = ch->step;
    *lowpower = ! ch->highpower;
    *wide = ! ch->isnarrow;
    *scan = ch->scanadd;
    *pttid = ch->pttid;
    *bcl = ch->bcl;
    *compander = ch->compander;
    *revfreq = ch->revfreq;
}

static void setup_channel (int chan_num, char *name, double rx_mhz, double txoff_mhz,
    int rq, int tq, int rpol, int tpol, int step, int lowpower, int wide,
    int scan, int pttid, int bcl, int compander, int revfreq)
{
    memory_channel_t *ch = chan_num + (memory_channel_t*) radio_mem;

    // Compute offset direction.
    if (txoff_mhz < 0) {
        txoff_mhz = - txoff_mhz;
        ch->shift_dir = 1;
    } else if (txoff_mhz > 0) {
        ch->shift_dir = 2;
    } else {
        ch->shift_dir = 0;
    }
    ch->rxfreq = int_to_bcd ((int) (rx_mhz * 100000.0 + 0.5));
    ch->txoff = int_to_bcd ((int) (txoff_mhz * 100000.0 + 0.5));
    ch->rxtone = rq;
    ch->txtone = tq;
    ch->rxpol = rpol;
    ch->txpol = tpol;
    ch->step = step;
    ch->highpower = ! lowpower;
    ch->isnarrow = ! wide;
    ch->scanadd = scan;
    ch->pttid = pttid;
    ch->bcl = bcl;
    ch->compander = compander;
    ch->revfreq = revfreq;
    ch->_u1 = 0;
    ch->_u2[0] = ch->_u2[1] = ch->_u2[2] = ch->_u2[3] = 0;

    // Encode channel name.
    uint8_t *dest = &radio_mem[0x0A00 + (chan_num-1)*5];
    int i;
    memset (dest, 0xff, 5);
    for (i=0; i<5 && *name; i++) {
        char *p;
        int c = *name++;

        if (c >= 'a' && c <= 'z')
            c += 'A' - 'a';
        p = strchr (CHARSET, c);
        if (p)
            *dest++ = p - CHARSET;
        else
            *dest++ = 11;       // Unknown character - use space
    }
}

static void erase_channel (int i)
{
    memory_channel_t *ch = i + (memory_channel_t*) radio_mem;

    // Erase channel name.
    memset (ch, 0xff, 16);
    memset (&radio_mem[0x0A00 + (i-1)*5], 0xff, 5);
}

typedef struct {
    uint8_t     lower_lsb;  // binary coded decimal, 4 digits
    uint8_t     lower_msb;
    uint8_t     upper_lsb;  // binary coded decimal, 4 digits
    uint8_t     upper_msb;
} limits_t;

//
// Looks like limits are not implemented on old firmware
// (prior to version 291).
//
static void decode_limits (char band, int *lower, int *upper)
{
    int offset = (band == 'V') ? 0xF00 : 0xF04;

    limits_t *limits = (limits_t*) (radio_mem + offset);
    *lower = ((limits->lower_msb >> 4) & 15) * 1000 +
             (limits->lower_msb        & 15) * 100 +
             ((limits->lower_lsb >> 4) & 15) * 10 +
             (limits->lower_lsb        & 15);
    *upper = ((limits->upper_msb >> 4) & 15) * 1000 +
             (limits->upper_msb        & 15) * 100 +
             ((limits->upper_lsb >> 4) & 15) * 10 +
             (limits->upper_lsb        & 15);
}

static void setup_limits (char band, double lower_mhz, double upper_mhz)
{
    int offset = (band == 'V') ? 0xF00 : 0xF04;
    limits_t *limits = (limits_t*) (radio_mem + offset);
    int lower = lower_mhz * 10 + 0.5;
    int upper = upper_mhz * 10 + 0.5;

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

    for (i=0; i<6; i++)
        ani[i] = "0123456789ABCDEF" [radio_mem[0x0D20+i] & 0x0f];
}

static void setup_ani (char *ani)
{
    int i, v;

    for (i=0; i<6; i++) {
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

        radio_mem[0x0D20+i] = v;
    }
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

static void print_vfo (FILE *out, char name, int hz, int offset,
    int rx_ctcs, int tx_ctcs, int rx_dcs, int tx_dcs, int step,
    int lowpower, int wide, int pttid, int bcl, int revfreq, int compander)
{
    fprintf (out, " %c  %8.4f ", name, hz / 1000000.0);
    print_offset (out, offset);
    fprintf (out, " ");
    print_squelch (out, rx_ctcs, rx_dcs);
    fprintf (out, " ");
    print_squelch (out, tx_ctcs, tx_dcs);

    fprintf (out, " %-4s %-4s  %-6s %-4s %-3s %-4s %s\n",
        STEP_NAME[step & 7], lowpower ? "Low" : "High",
        wide ? "Wide" : "Narr", pttid ? "+" : "-", bcl ? "+" : "-",
        revfreq ? "+" : "-", compander ? "+" : "-");
}

typedef struct {
    uint8_t     msb;
    uint8_t     lsb;
} fm_t;

//
// Generic settings.
//
typedef struct {
    uint8_t squelch;            // Carrier Squelch Level
    uint8_t scantype    : 2,    // Scan Resume
            tdr         : 1,    // Dual Watch
            roger       : 1,    // Roger Beep
            nobeep      : 1,    // Keypad Beep Disable
            backlight   : 1,    // Backlight
            save_funct  : 1,    // Battery Saver
            freqmode_ab : 1;    // Frequency mode
    uint8_t pttid       : 2,    // PTTID mode
            fm          : 1,    // FM Radio
            voice_prompt: 1,    // Voice Enable
            workmode_fm : 1,
            workmode_a  : 1,    // Freq/channel Mode A
            workmode_b  : 1,    // Freq/channel Mode B
            language    : 1;    // Voice Language
    uint8_t timeout;            // Timeout Timer
    uint8_t txtdr       : 2,
            _u1         : 2,
            mdf_a       : 2,    // Display Mode A
            mdf_b       : 2;    // Display Mode B
    uint8_t sidetone    : 1,    // DTMF Sidetone
            _u2         : 2,
            sqtail      : 1,    // Squelch tail enable
            _u3         : 4;
    uint8_t vox;                // VOX Level
} settings_t;

//
// Print full information about the device configuration.
//
static void uvb5_print_config (FILE *out, int verbose)
{
    int i;

    // Print memory channels.
    fprintf (out, "\n");
    if (verbose) {
        fprintf (out, "# Table of preprogrammed channels.\n");
        fprintf (out, "# 1) Channel number: 1-%d\n", NCHAN);
        fprintf (out, "# 2) Name: up to 5 characters, no spaces\n");
        fprintf (out, "# 3) Receive frequency in MHz\n");
        fprintf (out, "# 4) Offset of transmit frequency in MHz\n");
        fprintf (out, "# 5) Squelch tone for receive, or '-' to disable\n");
        fprintf (out, "# 6) Squelch tone for transmit, or '-' to disable\n");
        fprintf (out, "# 7) Transmit power: Low, High\n");
        fprintf (out, "# 8) Modulation width: Wide, Narrow\n");
        fprintf (out, "# 9) Add this channel to scan list: -, +\n");
        fprintf (out, "# 10) Transmit PTT ID (ANI code) on this channel: -, +\n");
        fprintf (out, "# 11) Busy channel lockout: -, +\n");
        fprintf (out, "# 12) Reverse RX/TX frequencies: -, +\n");
        fprintf (out, "# 13) Audio compander enable: -, +\n");
        fprintf (out, "#\n");
    }
    fprintf (out, "Channel Name   Receive  TxOffset Rx-Sq Tx-Sq Power FM   Scan PTTID BCL Rev Compand\n");
    for (i=1; i<=NCHAN; i++) {
        int rx_hz, txoff_hz, rx_ctcs, tx_ctcs, rx_dcs, tx_dcs;
        int step, lowpower, wide, scan, pttid;
        int bcl, compander, revfreq;
        char name[17];

        decode_channel (i, name, &rx_hz, &txoff_hz, &rx_ctcs, &tx_ctcs,
            &rx_dcs, &tx_dcs, &step, &lowpower, &wide, &scan, &pttid,
            &bcl, &compander, &revfreq);

        if (rx_hz == 0) {
            // Channel is disabled
            continue;
        }

        fprintf (out, "%5d   %-6s %8.4f ",
            i, name[0] ? name : "-", rx_hz / 1000000.0);
        print_offset (out, txoff_hz);
        fprintf (out, " ");
        print_squelch (out, rx_ctcs, rx_dcs);
        fprintf (out, " ");
        print_squelch (out, tx_ctcs, tx_dcs);

        fprintf (out, " %-4s  %-6s %-4s %-4s %-3s %-4s %s\n",
            lowpower ? "Low" : "High", wide ? "Wide" : "Narr",
            scan ? "+" : "-", pttid ? "+" : "-",
            bcl ? "+" : "-", revfreq ? "+" : "-", compander ? "+" : "-");
    }
    if (verbose)
        print_squelch_tones (out, 0);

    // Print frequency mode VFO settings.
    int hz, offset, rx_ctcs, tx_ctcs, rx_dcs, tx_dcs;
    int step, lowpower, wide, scan, pttid;
    int bcl, compander, revfreq;;
    fprintf (out, "\n");
    if (verbose) {
        fprintf (out, "# Table of VFO settings.\n");
        fprintf (out, "# 1) VFO index: A, B\n");
        fprintf (out, "# 2) Receive frequency in MHz\n");
        fprintf (out, "# 3) Offset of transmit frequency in MHz\n");
        fprintf (out, "# 4) Squelch tone for receive, or '-' to disable\n");
        fprintf (out, "# 5) Squelch tone for transmit, or '-' to disable\n");
        fprintf (out, "# 6) Frequency step in kHz: 2.5, 5.0, 6.25, 10.0, 12.5, 20.0, 25.0, 50.0\n");
        fprintf (out, "# 7) Transmit power: Low, High\n");
        fprintf (out, "# 8) Modulation width: Wide, Narrow\n");
        fprintf (out, "# 9) Transmit PTT ID (ANI code) on this channel: -, +\n");
        fprintf (out, "# 10) Busy channel lockout: -, +\n");
        fprintf (out, "# 11) Reverse RX/TX frequencies: -, +\n");
        fprintf (out, "# 12) Audio compander enable: -, +\n");
        fprintf (out, "#\n");
    }

    decode_channel (0, 0, &hz, &offset, &rx_ctcs, &tx_ctcs,
        &rx_dcs, &tx_dcs, &step, &lowpower, &wide, &scan, &pttid,
        &bcl, &compander, &revfreq);
    fprintf (out, "VFO Receive  TxOffset Rx-Sq Tx-Sq Step Power FM   PTTID BCL Rev Compand\n");
    print_vfo (out, 'A', hz, offset, rx_ctcs, tx_ctcs, rx_dcs, tx_dcs,
        step, lowpower, wide, pttid, bcl, revfreq, compander);
    decode_channel (130, 0, &hz, &offset, &rx_ctcs, &tx_ctcs,
        &rx_dcs, &tx_dcs, &step, &lowpower, &wide, &scan, &pttid,
        &bcl, &compander, &revfreq);
    print_vfo (out, 'B', hz, offset, rx_ctcs, tx_ctcs, rx_dcs, tx_dcs,
        step, lowpower, wide, pttid, bcl, revfreq, compander);

    // Print band limits.
    int vhf_lower, vhf_upper, uhf_lower, uhf_upper;
    decode_limits ('V', &vhf_lower, &vhf_upper);
    decode_limits ('U', &uhf_lower, &uhf_upper);
    fprintf (out, "\n");
    if (verbose) {
        fprintf (out, "# Table of band limits.\n");
        fprintf (out, "# 1) Band: VHF, UHF\n");
        fprintf (out, "# 2) Lower frequency in MHz\n");
        fprintf (out, "# 3) Upper frequency in MHz\n");
        fprintf (out, "#\n");
    }
    fprintf (out, "Limit Lower  Upper \n");
    fprintf (out, " VHF  %5.1f  %5.1f\n", vhf_lower/10.0, vhf_upper/10.0);
    fprintf (out, " UHF  %5.1f  %5.1f\n", uhf_lower/10.0, uhf_upper/10.0);

    // Broadcast FM.
    fm_t *fm = (fm_t*) &radio_mem[0x09A0];
    fprintf (out, "\n");
    if (verbose) {
        fprintf (out, "# Table of FM radio channels.\n");
        fprintf (out, "# 1) Channel number: 1-16\n");
        fprintf (out, "# 2) Frequency in MHz\n");
        fprintf (out, "#\n");
    }
    fprintf (out, "FM   Frequency\n");
    for (i=0; i<16; i++) {
        int freq = (fm[i].msb << 8) + fm[i].lsb + 650;
        if (freq <= 1080)
            fprintf (out, " %-2d  %5.1f\n", i+1, freq / 10.0);
    }

    // Get atomatic number identifier.
    char ani[6];
    fetch_ani (ani);

    // Print other settings.
    settings_t *mode = (settings_t*) &radio_mem[0x0D00];
    fprintf (out, "\n");
    if (verbose) {
        fprintf (out, "# Mute the speaker when a received signal is below this level.\n");
        fprintf (out, "# Options: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9\n");
    }
    fprintf (out, "Squelch Level: %u\n", mode->squelch);

    if (verbose)
        print_options (out, OFF_ON, 2, "Decrease the amount of power used when idle.");
    fprintf (out, "Battery Saver: %s\n", mode->save_funct ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Transmit 'roger' tone when PTT released.");
    fprintf (out, "Roger Beep: %s\n", mode->roger ? "On" : "Off");

    if (verbose)
        print_options (out, TIMER_NAME, 8, "Stop tramsmittion after specified number of seconds.");
    fprintf (out, "TX Timer: %s\n", TIMER_NAME[mode->timeout & 7]);

    if (verbose)
        print_options (out, VOX_NAME, 11, "Microphone sensitivity for VOX control.");
    fprintf (out, "VOX Level: %s\n", VOX_NAME[mode->vox & 15]);

    if (verbose)
        print_options (out, OFF_ON, 2, "Keypad beep sound.");
    fprintf (out, "Keypad Beep: %s\n", !mode->nobeep ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Enable voice messages.");
    fprintf (out, "Voice Prompt: %s\n", mode->voice_prompt ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Automatically switch A/B when signal is received on another frequency.");
    fprintf (out, "Dual Watch: %s\n", mode->tdr ? "On" : "Off");

    if (verbose)
        print_options (out, OFF_ON, 2, "Enable display backlight.");
    fprintf (out, "Backlight: %s\n", mode->backlight ? "On" : "Off");

    if (verbose)
        print_options (out, PTTID_NAME, 4, "Transmit ANI code when PTT button pressed and/or released.");
    fprintf (out, "PTT ID Transmit: %s\n", PTTID_NAME[mode->pttid & 3]);

    if (verbose) {
        fprintf (out, "\n# Automatic number identification: 6 characters of PTT ID code,\n");
        fprintf (out, "# which is transmitted on PTT button press and/or release.\n");
        fprintf (out, "# Characters allowed: 0 1 2 3 4 5 6 7 8 9 A B C D E F\n");
    }
    fprintf (out, "ANI Code: %c%c%c%c%c%c\n", ani[0], ani[1], ani[2], ani[3], ani[4], ani[5]);

    if (verbose)
        print_options (out, OFF_ON, 4, "Play DTMF tones when keycode or PTT ID is transmitted.");
    fprintf (out, "DTMF Sidetone: %s\n", mode->sidetone ? "On" : "Off");

    if (verbose)
        print_options (out, DISPLAY_MODE_NAME, 3, "What information to display for channel A.");
    fprintf (out, "Display A Mode: %s\n", DISPLAY_MODE_NAME[mode->mdf_a & 3]);

    if (verbose)
        print_options (out, DISPLAY_MODE_NAME, 3, "What information to display for channel B.");
    fprintf (out, "Display B Mode: %s\n", DISPLAY_MODE_NAME[mode->mdf_b & 3]);

    if (verbose) {
        fprintf (out, "\n# Method of resuming the scan after stop on active channel.\n");
        fprintf (out, "# TO - resume after a timeout.\n");
        fprintf (out, "# CO - resume after a carrier dropped off.\n");
        fprintf (out, "# SE - search and stop on next active frequency.\n");
    }
    fprintf (out, "Scan Resume: %s\n", SCAN_NAME[mode->scantype & 3]);

    if (verbose)
        print_options (out, TXTDR_NAME, 3, "Which frequency to use for transmit in dual watch mode.");
    fprintf (out, "TX Dual Watch: %s\n", TXTDR_NAME[mode->txtdr & 3]);

    if (verbose)
        print_options (out, OFF_ON, 2, "Reduce the squelch tail.");
    fprintf (out, "Squelch Tail Eliminate: %s\n", !mode->sqtail ? "On" : "Off");

    if (verbose)
        print_options (out, LANGUAGE_NAME, 2, "Select voice language.");
    fprintf (out, "Voice Language: %s\n", mode->language ? "Chinese" : "English");

    // Transient modes: no need to backup or configure.
    //fprintf (out, "Radio A Mode: %s\n", mode->workmode_a ? "Channel" : "Frequency");
    //fprintf (out, "Radio B Mode: %s\n", mode->workmode_b ? "Channel" : "Frequency");
    //fprintf (out, "Frequency Mode: %s\n", mode->freqmode_ab ? "B" : "A");
    //fprintf (out, "FM Radio: %s\n", mode->fm ? "On" : "Off");
    //fprintf (out, "FM Radio Mode: %s\n", mode->workmode_fm ? "Channel" : "Frequency");
}

//
// Read memory image from the binary file.
//
static void uvb5_read_image (FILE *img, unsigned char *ident)
{
    char buf[40];

    if (fread (ident, 1, 8, img) != 8) {
        fprintf (stderr, "Error reading image header.\n");
        exit (-1);
    }
    // Ignore next 40 bytes.
    if (fread (buf, 1, 40, img) != 40) {
        fprintf (stderr, "Error reading header.\n");
        exit (-1);
    }
    if (fread (&radio_mem[0], 1, 0x1000, img) != 0x1000) {
        fprintf (stderr, "Error reading image data.\n");
        exit (-1);
    }
}

//
// Save memory image to the binary file.
// Try to be compatible with Chirp.
//
static void uvb5_save_image (FILE *img)
{
    fwrite (radio_ident, 1, 8, img);
    fwrite ("Radio Program data v1.08\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 1, 40, img);
    fwrite (&radio_mem[0], 1, 0x1000, img);
}

static void uvb5_parse_parameter (char *param, char *value)
{
    settings_t *mode = (settings_t*) &radio_mem[0x0E20];
    int i;

    if (strcasecmp ("Radio", param) == 0) {
        if (strcasecmp ("Baofeng UV-B5", value) != 0) {
bad:        fprintf (stderr, "Bad value for %s: %s\n", param, value);
            exit(-1);
        }
        return;
    }
    if (strcasecmp ("Squelch Level", param) == 0) {
        mode->squelch = atoi (value);
        return;
    }
    if (strcasecmp ("Battery Saver", param) == 0) {
        mode->save_funct = atoi_off (value);
        return;
    }
    if (strcasecmp ("Roger Beep", param) == 0) {
        mode->roger = on_off (param, value);
        return;
    }
    if (strcasecmp ("TX Timer", param) == 0) {
        mode->timeout = atoi_off (value);
        return;
    }
    if (strcasecmp ("VOX Level", param) == 0) {
        mode->vox = atoi_off (value);
        return;
    }
    if (strcasecmp ("Keypad Beep", param) == 0) {
        mode->nobeep = ! on_off (param, value);
        return;
    }
    if (strcasecmp ("Voice Prompt", param) == 0) {
        mode->voice_prompt = on_off (param, value);
        return;
    }
    if (strcasecmp ("Dual Watch", param) == 0) {
        mode->tdr = on_off (param, value);
        return;
    }
    if (strcasecmp ("Backlight", param) == 0) {
        mode->backlight = on_off (param, value);
        return;
    }
    if (strcasecmp ("PTT ID Transmit", param) == 0) {
        i = string_in_table (value, PTTID_NAME, 4);
        if (i < 0)
            goto bad;
        mode->pttid = i;
        return;
    }
    if (strcasecmp ("ANI Code", param) == 0) {
        if (strlen (value) != 6) {
            fprintf (stderr, "Six hex digits expected.\n");
            goto bad;
        }
        setup_ani (value);
        return;
    }
    if (strcasecmp ("DTMF Sidetone", param) == 0) {
        mode->sidetone = on_off (param, value);
        return;
    }
    if (strcasecmp ("Display A Mode", param) == 0) {
        i = string_in_table (value, DISPLAY_MODE_NAME, 3);
        if (i < 0)
            goto bad;
        mode->mdf_a = i;
        return;
    }
    if (strcasecmp ("Display B Mode", param) == 0) {
        i = string_in_table (value, DISPLAY_MODE_NAME, 3);
        if (i < 0)
            goto bad;
        mode->mdf_b = i;
        return;
    }
    if (strcasecmp ("Scan Resume", param) == 0) {
        i = string_in_table (value, SCAN_NAME, 3);
        if (i < 0)
            goto bad;
        mode->scantype = i;
        return;
    }
    if (strcasecmp ("TX Dual Watch", param) == 0) {
        i = string_in_table (value, TXTDR_NAME, 3);
        if (i < 0)
            goto bad;
        mode->txtdr = i;
        return;
    }
    if (strcasecmp ("Squelch Tail Eliminate", param) == 0) {
        mode->sqtail = ! on_off (param, value);
        return;
    }
    if (strcasecmp ("Voice Language", param) == 0) {
        i = string_in_table (value, LANGUAGE_NAME, 2);
        if (i < 0)
            goto bad;
        mode->language = i;
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
    if (mhz >= 136 && mhz <= 174)
        return 1;
    if (mhz >= 400 && mhz <= 520)
        return 1;
    return 0;
}

//
// Parse one row in the Channels table.
// Return 0 on failure.
// Channel Name  Receive  TxOffset Rx-Sq Tx-Sq Power FM   Scan PTTID BCL Rev Compand
//     2   TWO   453.2250  0        91.5  91.5 High  Wide   -    -    -   -    -
//    13   -     465.5250  0       D703I D703I High  Wide   -    -    -   -    -
//
static int parse_channel (int first_row, char *line)
{
    char num_str[256], name[256], rxfreq_str[256], offset_str[256];
    char rq_str[256], tq_str[256], power_str[256], wide_str[256];
    char scan_str[256], pttid_str[256], bcl_str[256], rev_str[256], compand_str[256];
    int num, rq, tq, rpol, tpol, lowpower, wide, scan, pttid, bcl, rev, compand;
    double rx_mhz, txoff_mhz;

    if (sscanf (line, "%s %s %s %s %s %s %s %s %s %s %s %s %s",
        num_str, name, rxfreq_str, offset_str, rq_str, tq_str, power_str,
        wide_str, scan_str, pttid_str, bcl_str, rev_str, compand_str) != 13)
        return 0;

    num = atoi (num_str);
    if (num < 1 || num > NCHAN) {
        fprintf (stderr, "Bad channel number.\n");
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
    rq = encode_squelch (rq_str, &rpol);
    tq = encode_squelch (tq_str, &tpol);

    if (name[0] == '-' && name[1] == 0)
        name[0] = 0;

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

    if (*pttid_str == '+') {
        pttid = 1;
    } else if (*pttid_str == '-') {
        pttid = 0;
    } else {
        fprintf (stderr, "Bad PTTID mode.\n");
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

    if (*rev_str == '+') {
        rev = 1;
    } else if (*rev_str == '-') {
        rev = 0;
    } else {
        fprintf (stderr, "Bad RevFreq flag.\n");
        return 0;
    }

    if (*compand_str == '+') {
        compand = 1;
    } else if (*compand_str == '-') {
        compand = 0;
    } else {
        fprintf (stderr, "Bad Compander flag.\n");
        return 0;
    }

    if (first_row) {
        // On first entry, erase the channel table.
        int i;
        for (i=0; i<NCHAN; i++) {
            erase_channel (i);
        }
    }
    if (name[0] == '-')
        name[0] = 0;
    setup_channel (num, name, rx_mhz, txoff_mhz, rq, tq, rpol, tpol,
        0, lowpower, wide, scan, pttid, bcl, compand, rev);
    return 1;
}

//
// Parse one row in the VFO table.
// VFO Receive  TxOffset Rx-Sq Tx-Sq Step Power FM   PTTID BCL Rev Compand
//  A  443.0750  0          -     -  25.0 High  Wide   -    -   -    -
//  B  145.2300  0          -     -  5.0  High  Wide   -    -   -    -
//
static int parse_vfo (int first_row, char *line)
{
    char num_str[256], rxfreq_str[256], offset_str[256];
    char rq_str[256], tq_str[256], step_str[256];
    char power_str[256], wide_str[256], pttid_str[256];
    char bcl_str[256], rev_str[256], compand_str[256];
    int num, rq, tq, rpol, tpol, step, lowpower, wide, pttid, bcl, rev, compand;
    double rx_mhz, txoff_mhz;

    if (sscanf (line, "%s %s %s %s %s %s %s %s %s %s %s %s",
        num_str, rxfreq_str, offset_str, rq_str, tq_str, step_str,
        power_str, wide_str, pttid_str, bcl_str, rev_str, compand_str) != 12)
        return 0;

    if (*num_str == 'A' || *num_str == 'a')
        num = 0;
    else if (*num_str == 'B' || *num_str == 'b')
        num = 130;
    else  {
        fprintf (stderr, "Bad VFO number.\n");
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
    rq = encode_squelch (rq_str, &rpol);
    tq = encode_squelch (tq_str, &tpol);

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

    if (*pttid_str == '+') {
        pttid = 1;
    } else if (*pttid_str == '-') {
        pttid = 0;
    } else {
        fprintf (stderr, "Bad PTTID mode.\n");
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

    if (*rev_str == '+') {
        rev = 1;
    } else if (*rev_str == '-') {
        rev = 0;
    } else {
        fprintf (stderr, "Bad RevFreq flag.\n");
        return 0;
    }

    if (*compand_str == '+') {
        compand = 1;
    } else if (*compand_str == '-') {
        compand = 0;
    } else {
        fprintf (stderr, "Bad Compander flag.\n");
        return 0;
    }

    setup_channel (num, "", rx_mhz, txoff_mhz, rq, tq, rpol, tpol,
        step, lowpower, wide, 0, pttid, bcl, compand, rev);
    return 1;
}

//
// Parse one row in the Limits table.
// Limit Lower  Upper
//  VHF  136.0  174.0
//  UHF  400.0  480.0
//
static int parse_limit (int first_row, char *line)
{
    char band_str[256];
    double lower, upper;

    if (sscanf (line, "%s %lf %lf", band_str, &lower, &upper) != 3)
        return 0;

    if (strcasecmp ("VHF", band_str) == 0) {
        setup_limits ('V', lower, upper);
    } else if (strcasecmp ("UHF", band_str) == 0) {
        setup_limits ('U', lower, upper);
    } else {
        fprintf (stderr, "Unknown band.\n");
        return 0;
    }
    return 1;
}

//
// Parse one row in the FM table.
// FM   Frequency
//  1    91.5
//  10  100.9
//
static int parse_fm (int first_row, char *line)
{
    fm_t *fm = (fm_t*) &radio_mem[0x09A0];
    int num, freq;
    double mhz;

    if (sscanf (line, "%u %lf", &num, &mhz) != 2)
        return 0;

    if (num < 1 || num > 16) {
        fprintf (stderr, "Bad channel number.\n");
        return 0;
    }

    if (mhz < 65.0 || mhz > 108) {
        fprintf (stderr, "Bad FM frequency.\n");
        return 0;
    }

    freq = (mhz - 65.0) * 10 + 0.5;
    fm[num-1].msb = freq >> 8;
    fm[num-1].lsb = freq;
    return 1;
}

static int uvb5_parse_header (char *line)
{
    if (strncasecmp (line, "Channel", 7) == 0)
        return 'C';
    if (strncasecmp (line, "VFO", 3) == 0)
        return 'V';
    if (strncasecmp (line, "Limit", 5) == 0)
        return 'L';
    if (strncasecmp (line, "FM", 2) == 0)
        return 'F';
    return 0;
}

static int uvb5_parse_row (int table_id, int first_row, char *line)
{
    switch (table_id) {
    case 'C': return parse_channel (first_row, line);
    case 'V': return parse_vfo (first_row, line);
    case 'L': return parse_limit (first_row, line);
    case 'F': return parse_fm (first_row, line);
    }
    return 0;
}

//
// Baofeng UV-B5, UV-B6
//
radio_device_t radio_uvb5 = {
    "Baofeng UV-B5",
    uvb5_download,
    uvb5_upload,
    uvb5_read_image,
    uvb5_save_image,
    uvb5_print_version,
    uvb5_print_config,
    uvb5_parse_parameter,
    uvb5_parse_header,
    uvb5_parse_row,
};
