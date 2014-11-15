/*
 * Interface to Yaesu FT-60R.
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

#define NCHAN           1000
#define NPMS            50
#define MEMSZ           0x6fc8

#define OFFSET_VFO      0x0048
#define OFFSET_HOME     0x01c8
#define OFFSET_CHANNELS 0x0248
#define OFFSET_PMS      0x40c8
#define OFFSET_NAMES    0x4708
#define OFFSET_BANKS    0x69c8
#define OFFSET_SCAN     0x6ec8

static const char CHARSET[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ !`o$%&'()*+,-./|;/=>?@[~]^__";
#define NCHARS  65
#define SPACE   36
#define OPENBOX 64

static const char *BAND_NAME[5] = { "144", "250", "350", "430", "850" };

static const char *POWER_NAME[] = { "High", "Med", "Low", "??" };

static const char *SCAN_NAME[] = { "+", "-", "Only", "??" };

enum {
    STEP_5 = 0,
    STEP_10,
    STEP_12_5,
    STEP_15,
    STEP_20,
    STEP_25,
    STEP_50,
    STEP_100,
};

//
// Data structure for a memory channel.
//
typedef struct {
    uint8_t     duplex    : 4,  // Repeater mode
#define D_SIMPLEX       0
#define D_NEG_OFFSET    2
#define D_POS_OFFSET    3
#define D_CROSS_BAND    4
                isam      : 1,  // Amplitude modulation
                isnarrow  : 1,  // Narrow FM modulation
                _u1       : 1,
                used      : 1;  // Channel is used
    uint8_t     rxfreq [3];     // Receive frequency
    uint8_t     tmode     : 3,  // CTCSS/DCS mode
#define T_OFF           0
#define T_TONE          1
#define T_TSQL          2
#define T_TSQL_REV      3
#define T_DTCS          4
#define T_D             5
#define T_T_DCS         6
#define T_D_TSQL        7
                step      : 3,  // Frequency step
                _u2       : 2;
    uint8_t     txfreq [3];     // Transmit frequency when cross-band
    uint8_t     tone      : 6,  // CTCSS tone select
#define TONE_DEFAULT    12

                power     : 2;  // Transmit power level
    uint8_t     dtcs      : 7,  // DCS code select
                _u3       : 1;
    uint8_t     _u4 [2];
    uint8_t     offset;         // TX offset, in 50kHz steps
    uint8_t     _u5 [3];
} memory_channel_t;

//
// Data structure for a channel name.
//
typedef struct {
    uint8_t     name[6];
    uint8_t     _u1       : 7,
                used      : 1;
    uint8_t     _u2       : 7,
                valid     : 1;
} memory_name_t;

//
// Print a generic information about the device.
//
static void ft60r_print_version (FILE *out)
{
    // Nothing to print.
}

//
// Read block of data, up to 64 bytes.
// When start==0, return non-zero on success or 0 when empty.
// When start!=0, halt the program on any error.
//
static int read_block (int fd, int start, unsigned char *data, int nbytes)
{
    unsigned char reply;
    int len;

    // Read data.
    len = serial_read (fd, data, nbytes);
    if (len != nbytes) {
        if (start == 0)
            return 0;
        fprintf (stderr, "Reading block 0x%04x: got only %d bytes.\n", start, len);
        exit(-1);
    }

    // Get acknowledge.
    serial_write (fd, "\x06", 1);
    if (serial_read (fd, &reply, 1) != 1) {
        fprintf (stderr, "No acknowledge after block 0x%04x.\n", start);
        exit(-1);
    }
    if (reply != 0x06) {
        fprintf (stderr, "Bad acknowledge after block 0x%04x: %02x\n", start, reply);
        exit(-1);
    }
    if (verbose) {
        printf ("# Read 0x%04x: ", start);
        print_hex (data, nbytes);
        printf ("\n");
    } else {
        ++radio_progress;
        if (radio_progress % 16 == 0) {
            fprintf (stderr, "#");
            fflush (stderr);
        }
    }
    return 1;
}

//
// Write block of data, up to 64 bytes.
// Halt the program on any error.
// Return 0 on error.
//
static int write_block (int fd, int start, const unsigned char *data, int nbytes)
{
    unsigned char reply[64];
    int len;

    serial_write (fd, data, nbytes);

    // Get echo.
    len = serial_read (fd, reply, nbytes);
    if (len != nbytes) {
        fprintf (stderr, "! Echo for block 0x%04x: got only %d bytes.\n", start, len);
        return 0;
    }

    // Get acknowledge.
    if (serial_read (fd, reply, 1) != 1) {
        fprintf (stderr, "! No acknowledge after block 0x%04x.\n", start);
        return 0;
    }
    if (reply[0] != 0x06) {
        fprintf (stderr, "! Bad acknowledge after block 0x%04x: %02x\n", start, reply[0]);
        return 0;
    }
    if (verbose) {
        printf ("# Write 0x%04x: ", start);
        print_hex (data, nbytes);
        printf ("\n");
    } else {
        ++radio_progress;
        if (radio_progress % 16 == 0) {
            fprintf (stderr, "#");
            fflush (stderr);
        }
    }
    return 1;
}

//
// Read memory image from the device.
//
static void ft60r_download()
{
    int addr, sum;

    if (verbose)
        fprintf (stderr, "\nPlease follow the procedure:\n");
    else
        fprintf (stderr, "please follow the procedure.\n");
    fprintf (stderr, "\n");
    fprintf (stderr, "1. Power Off the FT60.\n");
    fprintf (stderr, "2. Hold down the MONI switch and Power On the FT60.\n");
    fprintf (stderr, "3. Rotate the right DIAL knob to select F8 CLONE.\n");
    fprintf (stderr, "4. Briefly press the [F/W] key. The display should go blank then show CLONE.\n");
    fprintf (stderr, "5. Press and hold the PTT switch until the radio starts to send.\n");
    fprintf (stderr, "-- Or enter ^C to abort the memory read.\n");
again:
    fprintf (stderr, "\n");
    fprintf (stderr, "Waiting for data... ");
    fflush (stderr);

    // Wait for the first 8 bytes.
    while (read_block (radio_port, 0, radio_ident, 8) == 0)
        continue;

    // Get the rest of data.
    for (addr=8; addr<MEMSZ; addr+=64)
        read_block (radio_port, addr, &radio_mem[addr], 64);

    // Get the checksum.
    read_block (radio_port, MEMSZ, &radio_mem[MEMSZ], 1);

    // Verify the checksum.
    sum = 0;
    for (addr=0; addr<8; addr++)
        sum += radio_ident[addr];
    for (addr=8; addr<MEMSZ; addr++)
        sum += radio_mem[addr];
    sum = sum & 0xff;
    if (sum != radio_mem[MEMSZ]) {
        if (verbose) {
            printf ("Checksum = %02x (BAD)\n", radio_mem[MEMSZ]);
            fprintf (stderr, "BAD CHECKSUM!\n");
        } else
            fprintf (stderr, "[BAD CHECKSUM]\n");
        fprintf (stderr, "Please, repeat the procedure:\n");
        fprintf (stderr, "Press and hold the PTT switch until the radio starts to send.\n");
        fprintf (stderr, "Or enter ^C to abort the memory read.\n");
        goto again;
    }
    if (verbose)
        printf ("Checksum = %02x (OK)\n", radio_mem[MEMSZ]);
}

//
// Write memory image to the device.
//
static void ft60r_upload (int cont_flag)
{
    int addr, sum;
    char buf[80];

    if (verbose)
        fprintf (stderr, "\nPlease follow the procedure:\n");
    else
        fprintf (stderr, "please follow the procedure.\n");
    fprintf (stderr, "\n");
    if (cont_flag) {
        fprintf (stderr, "1. Press the MONI switch until the radio starts to receive.\n");
        fprintf (stderr, "2. Press <Enter> to continue.\n");
    } else {
        fprintf (stderr, "1. Power Off the FT60.\n");
        fprintf (stderr, "2. Hold down the MONI switch and Power On the FT60.\n");
        fprintf (stderr, "3. Rotate the right DIAL knob to select F8 CLONE.\n");
        fprintf (stderr, "4. Briefly press the [F/W] key. The display should go blank then show CLONE.\n");
        fprintf (stderr, "5. Press the MONI switch until the radio starts to receive.\n");
        fprintf (stderr, "6. Press <Enter> to continue.\n");
    }
    fprintf (stderr, "-- Or enter ^C to abort the memory write.\n");
again:
    fprintf (stderr, "\n");
    fprintf (stderr, "Press <Enter> to continue: ");
    fflush (stderr);
    serial_flush (radio_port);
    if (! fgets (buf, sizeof(buf), stdin))
	/*ignore*/;
    fprintf (stderr, "Sending data... ");
    fflush (stderr);

    if (! write_block (radio_port, 0, radio_ident, 8)) {
error:  fprintf (stderr, "\nPlease, repeat the procedure:\n");
        fprintf (stderr, "1. Briefly press the [F/W] key to clear the ERROR status.\n");
        fprintf (stderr, "2. Press the MONI switch until the radio starts to receive.\n");
        fprintf (stderr, "3. Press <Enter> to continue.\n");
        fprintf (stderr, "-- Or enter ^C to abort the memory write.\n");
        goto again;
    }
    for (addr=8; addr<MEMSZ; addr+=64)
        if (! write_block (radio_port, addr, &radio_mem[addr], 64))
            goto error;

    // Compute the checksum.
    sum = 0;
    for (addr=0; addr<8; addr++)
        sum += radio_ident[addr];
    for (addr=8; addr<MEMSZ; addr++)
        sum += radio_mem[addr];
    radio_mem[MEMSZ] = sum;

    // Send a checksum.
    if (! write_block (radio_port, MEMSZ, &radio_mem[MEMSZ], 1))
        goto error;
}

//
// Convert squelch string to CTCSS tone index.
// Return -1 on error.
// Format: nnn.n
//
static int encode_tone (char *str)
{
    unsigned val;

    // CTCSS tone
    float hz;
    if (sscanf (str, "%f", &hz) != 1)
        return -1;

    // Round to integer.
    val = hz * 10.0 + 0.5;
    if (val < 0x0258)
        return -1;

    // Find a valid index in CTCSS table.
    int i;
    for (i=0; i<NCTCSS; i++)
        if (CTCSS_TONES[i] == val)
            return i;
    return -1;
}

//
// Convert squelch string to DCS code index.
// Return -1 on error.
// Format: Dnnn
//
static int encode_dcs (char *str)
{
    unsigned val;

    // DCS tone
    if (sscanf (++str, "%u", &val) != 1)
        return -1;

    // Find a valid index in DCS table.
    int i;
    for (i=0; i<NDCS; i++)
        if (DCS_CODES[i] == val)
            return i;
    return -1;
}

//
// Convert squelch strings to tmode value, tone index and dtcs index.
//
static int encode_squelch (char *rx, char *tx, int *tone, int *dtcs)
{
    int rx_tone = -1, tx_tone = -1, rx_dcs = -1, tx_dcs = -1, rx_rev = 0;

    if (*rx == 'D' || *rx == 'd') {             // Receive DCS code
        rx_dcs = encode_dcs (rx);
    } else if (*rx >= '0' && *rx <= '9') {      // Receive CTCSS tone
        rx_tone = encode_tone (rx);
    } else if (*rx == '-' && rx[1] >= '0' && rx[1] <= '9') {
        rx_tone = encode_tone (rx+1);
        rx_rev = 1;
    }
    if (*tx == 'D' || *tx == 'd') {             // Transmit DCS code
        tx_dcs = encode_dcs (tx);
    } else if (*tx >= '0' && *tx <= '9') {      // Transmit CTCSS tone
        tx_tone = encode_tone (tx);
    }

    // Encode tmode.
    *tone = TONE_DEFAULT;
    *dtcs = 0;
    if (rx_dcs >= 0) {
        *dtcs = rx_dcs;
        if (tx_tone >= 0) {
            *tone = tx_tone;
            return T_T_DCS;
        }
        return T_DTCS;
    }
    if (tx_dcs >= 0) {
        *dtcs = tx_dcs;
        if (rx_tone >= 0) {
            *tone = rx_tone;
            return T_D_TSQL;
        }
        return T_D;
    }
    if (tx_tone >= 0) {
        *tone = tx_tone;
        if (rx_tone < 0)
            return T_TONE;
        if (rx_rev)
            return T_TSQL_REV;
        return T_TSQL;
    }
    return T_OFF;
}

//
// Convert a 3-byte frequency value from binary coded decimal
// to integer format (in Hertz).
//
static int freq_to_hz (uint8_t *bcd)
{
    int hz;

    hz = (bcd[0]       & 15) * 100000000 +
        ((bcd[1] >> 4) & 15) * 10000000 +
         (bcd[1]       & 15) * 1000000 +
        ((bcd[2] >> 4) & 15) * 100000 +
         (bcd[2]       & 15) * 10000;
    hz += (bcd[0] >> 6) * 2500;
    return hz;
}

//
// Convert an integet frequency value (in Hertz)
// to a 3-byte binary coded decimal format.
//
static void hz_to_freq (int hz, uint8_t *bcd)
{
    bcd[0] = (hz / 2500      % 4)  << 6 |
             (hz / 100000000 % 10);
    bcd[1] = (hz / 10000000  % 10) << 4 |
             (hz / 1000000   % 10);
    bcd[2] = (hz / 100000    % 10) << 4 |
             (hz / 10000     % 10);
}

//
// Get a bitmask of banks for a given channel.
//
static int decode_banks (int i)
{
    int b, mask, data;

    mask = 0;
    for (b=0; b<10; b++) {
        data = radio_mem [OFFSET_BANKS + b * 0x80 + i/8];
        if ((data >> (i & 7)) & 1)
            mask |= 1 << b;
    }
    return mask;
}

//
// Set the bitmask of banks for a given channel.
//
static void setup_banks (int i, int mask)
{
    int b;
    unsigned char *data;

    for (b=0; b<10; b++) {
        data = &radio_mem [OFFSET_BANKS + b * 0x80 + i/8];
        if ((mask >> b) & 1)
            *data |= 1 << (i & 7);
        else
            *data &= ~(1 << (i & 7));
    }
}

//
// Extract channel name.
//
static void decode_name (int i, char *name)
{
    memory_name_t *nm = i + (memory_name_t*) &radio_mem[OFFSET_NAMES];

    if (nm->valid && nm->used) {
        int n, c;
        for (n=0; n<6; n++) {
            c = nm->name[n];
            name[n] = (c < NCHARS) ? CHARSET[c] : ' ';

            // Replace spaces by underscore.
            if (name[n] == ' ')
                name[n] = '_';
        }
        // Strip trailing spaces.
        for (n=5; n>=0 && name[n]=='_'; n--)
            name[n] = 0;
        name[6] = 0;
    }
}

//
// Encode a character from ASCII to internal index.
// Replace underscores by spaces.
// Make all letters uppercase.
//
static int encode_char (int c)
{
    int i;

    // Replace underscore by space.
    if (c == '_')
        c = ' ';
    if (c >= 'a' && c <= 'z')
        c += 'A' - 'a';
    for (i=0; i<NCHARS; i++)
        if (c == CHARSET[i])
            return i;
    return OPENBOX;
}

//
// Set a name for the channel.
//
static void encode_name (int i, char *name)
{
    memory_name_t *nm = i + (memory_name_t*) &radio_mem[OFFSET_NAMES];
    int n;

    if (name && *name && *name != '-') {
        // Setup channel name.
        nm->valid = 1;
        nm->used = 1;
        for (n=0; n<6 && name[n]; n++) {
            nm->name[n] = encode_char (name[n]);
        }
        for (; n<6; n++)
            nm->name[n] = SPACE;
    } else {
        // Clear name.
        nm->valid = 0;
        nm->used = 0;
        for (n=0; n<6; n++)
            nm->name[n] = 0xff;
    }
}

//
// Get all parameters for a given channel.
// Seek selects the type of channel:
//  OFFSET_VFO      - VFO channel, 0..4
//  OFFSET_HOME     - home channel, 0..4
//  OFFSET_CHANNELS - memory channel, 0..999
//  OFFSET_PMS      - programmable memory scan, i=0..99
//
static void decode_channel (int i, int seek, char *name,
    int *rx_hz, int *tx_hz, int *rx_ctcs, int *tx_ctcs,
    int *rx_dcs, int *tx_dcs, int *power, int *wide,
    int *scan, int *isam, int *step, int *banks)
{
    memory_channel_t *ch = i + (memory_channel_t*) &radio_mem[seek];
    int scan_data = radio_mem[OFFSET_SCAN + i/4];

    *rx_hz = *tx_hz = *rx_ctcs = *tx_ctcs = *rx_dcs = *tx_dcs = 0;
    *power = *wide = *scan = *isam = *step = 0;
    if (name)
        *name = 0;
    if (banks)
        *banks = 0;
    if (! ch->used && (seek == OFFSET_CHANNELS || seek == OFFSET_PMS))
        return;

    // Extract channel name.
    if (name && seek == OFFSET_CHANNELS)
        decode_name (i, name);

    // Decode channel frequencies.
    *rx_hz = freq_to_hz (ch->rxfreq);

    *tx_hz = *rx_hz;
    switch (ch->duplex) {
    case D_NEG_OFFSET:
        *tx_hz -= ch->offset * 50000;
        break;
    case D_POS_OFFSET:
        *tx_hz += ch->offset * 50000;
        break;
    case D_CROSS_BAND:
        *tx_hz = freq_to_hz (ch->txfreq);
        break;
    }

    // Decode squelch modes.
    switch (ch->tmode) {
    case T_TONE:
        *tx_ctcs = CTCSS_TONES[ch->tone];
        break;
    case T_TSQL:
        *tx_ctcs = CTCSS_TONES[ch->tone];
        *rx_ctcs = CTCSS_TONES[ch->tone];
        break;
    case T_TSQL_REV:
        *tx_ctcs = CTCSS_TONES[ch->tone];
        *rx_ctcs = - CTCSS_TONES[ch->tone];
        break;
    case T_DTCS:
        *tx_dcs = DCS_CODES[ch->dtcs];
        *rx_dcs = DCS_CODES[ch->dtcs];
        break;
    case T_D:
        *tx_dcs = DCS_CODES[ch->dtcs];
        break;
    case T_T_DCS:
        *tx_ctcs = CTCSS_TONES[ch->tone];
        *rx_dcs = DCS_CODES[ch->dtcs];
        break;
    case T_D_TSQL:
        *tx_dcs = DCS_CODES[ch->dtcs];
        *rx_ctcs = CTCSS_TONES[ch->tone];
        break;
    }

    // Other parameters.
    *power = ch->power;
    *wide = ! ch->isnarrow;
    *scan = (scan_data << ((i & 3) * 2) >> 6) & 3;
    *isam = ch->isam;
    *step = ch->step;

    if (seek == OFFSET_CHANNELS)
        *banks = decode_banks (i);
}

//
// Set the parameters for a given memory channel.
//
static void setup_channel (int i, char *name, double rx_mhz, double tx_mhz,
    int tmode, int tone, int dtcs, int power, int wide, int scan, int isam, int banks)
{
    memory_channel_t *ch = i + (memory_channel_t*) &radio_mem[OFFSET_CHANNELS];

    hz_to_freq ((int) (rx_mhz * 1000000.0), ch->rxfreq);

    double offset_mhz = tx_mhz - rx_mhz;
    ch->offset = 0;
    ch->txfreq[0] = ch->txfreq[1] = ch->txfreq[2] = 0;
    if (offset_mhz == 0) {
        ch->duplex = D_SIMPLEX;
    } else if (offset_mhz > 0 && offset_mhz < 256 * 0.05) {
        ch->duplex = D_POS_OFFSET;
        ch->offset = (int) (offset_mhz / 0.05 + 0.5);
    } else if (offset_mhz < 0 && offset_mhz > -256 * 0.05) {
        ch->duplex = D_NEG_OFFSET;
        ch->offset = (int) (-offset_mhz / 0.05 + 0.5);
    } else {
        ch->duplex = D_CROSS_BAND;
        hz_to_freq ((int) (tx_mhz * 1000000.0), ch->txfreq);
    }
    ch->used = (rx_mhz > 0);
    ch->tmode = tmode;
    ch->tone = tone;
    ch->dtcs = dtcs;
    ch->power = power;
    ch->isnarrow = ! wide;
    ch->isam = isam;
    ch->step = (rx_mhz >= 400) ? STEP_12_5 : STEP_5;
    ch->_u1 = 0;
    ch->_u2 = (rx_mhz >= 400);
    ch->_u3 = 0;
    ch->_u4[0] = 15;
    ch->_u4[1] = 0;
    ch->_u5[0] = ch->_u5[1] = ch->_u5[2] = 0;

    // Scan mode.
    unsigned char *scan_data = &radio_mem[OFFSET_SCAN + i/4];
    int scan_shift = (i & 3) * 2;
    *scan_data &= ~(3 << scan_shift);
    *scan_data |= scan << scan_shift;

    setup_banks (i, banks);
    encode_name (i, name);
}

//
// Set the parameters for a given home channel.
// Band selects the channel: 144, 250, 350, 430 or 850.
//
static void setup_home (int band, double rx_mhz, double tx_mhz,
    int tmode, int tone, int dtcs, int power, int wide, int isam)
{
    memory_channel_t *ch = (memory_channel_t*) &radio_mem[OFFSET_HOME];

    switch (band) {
    default:  break;
    case 250: ch += 1; break;
    case 350: ch += 2; break;
    case 430: ch += 3; break;
    case 850: ch += 4; break;
    }
    hz_to_freq ((int) (rx_mhz * 1000000.0), ch->rxfreq);

    double offset_mhz = tx_mhz - rx_mhz;
    ch->offset = 0;
    ch->txfreq[0] = ch->txfreq[1] = ch->txfreq[2] = 0;
    if (offset_mhz == 0) {
        ch->duplex = D_SIMPLEX;
    } else if (offset_mhz > 0 && offset_mhz < 256 * 0.05) {
        ch->duplex = D_POS_OFFSET;
        ch->offset = (int) (offset_mhz / 0.05 + 0.5);
    } else if (offset_mhz < 0 && offset_mhz > -256 * 0.05) {
        ch->duplex = D_NEG_OFFSET;
        ch->offset = (int) (-offset_mhz / 0.05 + 0.5);
    } else {
        ch->duplex = D_CROSS_BAND;
        hz_to_freq ((int) (tx_mhz * 1000000.0), ch->txfreq);
    }
    ch->used = (rx_mhz > 0);
    ch->tmode = tmode;
    ch->tone = tone;
    ch->dtcs = dtcs;
    ch->power = power;
    ch->isnarrow = ! wide;
    ch->isam = isam;
    ch->step = (rx_mhz >= 400) ? STEP_12_5 : STEP_5;
    ch->_u1 = 0;
    ch->_u2 = (rx_mhz >= 400);
    ch->_u3 = 0;
    ch->_u4[0] = 15;
    ch->_u4[1] = 0;
    ch->_u5[0] = ch->_u5[1] = ch->_u5[2] = 0;
}

//
// Set the parameters for a given PMS pair.
//
static void setup_pms (int i, double lower_mhz, double upper_mhz)
{
    memory_channel_t *ch = i*2 + (memory_channel_t*) &radio_mem[OFFSET_PMS];

    if (! lower_mhz) {
        ch[0].used = 0;
        ch[1].used = 0;
        return;
    }
    hz_to_freq ((int) (lower_mhz * 1000000.0), ch[0].rxfreq);
    ch[0].used = 1;
    hz_to_freq ((int) (upper_mhz * 1000000.0), ch[1].rxfreq);
    ch[1].used = 1;
}

//
// Print the transmit offset or frequency.
//
static void print_offset (FILE *out, int rx_hz, int tx_hz)
{
    int delta = tx_hz - rx_hz;

    if (delta == 0) {
        fprintf (out, "+0      ");
    } else if (delta > 0 && delta/50000 <= 255) {
        if (delta % 1000000 == 0)
            fprintf (out, "+%-7u", delta / 1000000);
        else
            fprintf (out, "+%-7.3f", delta / 1000000.0);
    } else if (delta < 0 && -delta/50000 <= 255) {
        delta = - delta;
        if (delta % 1000000 == 0)
            fprintf (out, "-%-7u", delta / 1000000);
        else
            fprintf (out, "-%-7.3f", delta / 1000000.0);
    } else {
        // Cross band mode.
        fprintf (out, " %-7.4f", tx_hz / 1000000.0);
    }
}

//
// Print the squelch value: CTCSS or DCS.
//
static void print_squelch (FILE *out, int ctcs, int dcs)
{
    if      (ctcs)    fprintf (out, "%5.1f", ctcs / 10.0);
    else if (dcs > 0) fprintf (out, "D%03d", dcs);
    else              fprintf (out, "   - ");
}

//
// Print the list of channel banks.
//
static char *format_banks (int mask)
{
    static char buf [16];
    char *p;
    int b;

    p = buf;
    for (b=0; b<10; b++) {
        if ((mask >> b) & 1)
            *p++ = "1234567890" [b];
    }
    if (p == buf)
        *p++ = '-';
    *p = 0;
    return buf;
}

//
// Print full information about the device configuration.
//
static void ft60r_print_config (FILE *out, int verbose)
{
    int i;

    //
    // Memory channels.
    //
    fprintf (out, "\n");
    if (verbose) {
        fprintf (out, "# Table of preprogrammed channels.\n");
        fprintf (out, "# 1) Channel number: 1-%d\n", NCHAN);
        fprintf (out, "# 2) Name: up to 6 characters, no spaces\n");
        fprintf (out, "# 3) Receive frequency in MHz\n");
        fprintf (out, "# 4) Transmit frequency or +/- offset in MHz\n");
        fprintf (out, "# 5) Squelch tone for receive, or '-' to disable\n");
        fprintf (out, "# 6) Squelch tone for transmit, or '-' to disable\n");
        fprintf (out, "# 7) Transmit power: High, Mid, Low\n");
        fprintf (out, "# 8) Modulation: Wide, Narrow, AM\n");
        fprintf (out, "# 9) Scan mode: +, -, Only\n");
        fprintf (out, "# 10) List of banks 0..9, or '-' to disable\n");
        fprintf (out, "#\n");
    }
    fprintf (out, "Channel Name    Receive  Transmit R-Squel T-Squel Power Modulation Scan Banks\n");
    for (i=0; i<NCHAN; i++) {
        int rx_hz, tx_hz, rx_ctcs, tx_ctcs, rx_dcs, tx_dcs;
        int power, wide, scan, isam, step, banks;
        char name[17];

        decode_channel (i, OFFSET_CHANNELS, name, &rx_hz, &tx_hz, &rx_ctcs, &tx_ctcs,
            &rx_dcs, &tx_dcs, &power, &wide, &scan, &isam, &step, &banks);
        if (rx_hz == 0) {
            // Channel is disabled
            continue;
        }

        fprintf (out, "%5d   %-7s %8.4f ", i+1, name[0] ? name : "-", rx_hz / 1000000.0);
        print_offset (out, rx_hz, tx_hz);
        fprintf (out, " ");
        print_squelch (out, rx_ctcs, rx_dcs);
        fprintf (out, "   ");
        print_squelch (out, tx_ctcs, tx_dcs);

        fprintf (out, "   %-4s  %-10s %-4s %s\n", POWER_NAME[power],
            isam ? "AM" : wide ? "Wide" : "Narrow", SCAN_NAME[scan],
            format_banks (banks));
    }
    if (verbose)
        print_squelch_tones (out, 1);

    //
    // Home channels.
    //
    fprintf (out, "\n");
    if (verbose) {
        fprintf (out, "# Table of home frequencies.\n");
        fprintf (out, "# 1) Band: 144, 250, 350, 430 or, 850\n");
        fprintf (out, "# 2) Receive frequency in MHz\n");
        fprintf (out, "# 3) Transmit frequency or +/- offset in MHz\n");
        fprintf (out, "# 4) Squelch tone for receive, or '-' to disable\n");
        fprintf (out, "# 5) Squelch tone for transmit, or '-' to disable\n");
        fprintf (out, "# 6) Transmit power: High, Mid, Low\n");
        fprintf (out, "# 7) Modulation: Wide, Narrow, AM\n");
        fprintf (out, "#\n");
    }
    fprintf (out, "Home    Receive  Transmit R-Squel T-Squel Power Modulation\n");
    for (i=0; i<5; i++) {
        int rx_hz, tx_hz, rx_ctcs, tx_ctcs, rx_dcs, tx_dcs;
        int power, wide, scan, isam, step;

        decode_channel (i, OFFSET_HOME, 0, &rx_hz, &tx_hz, &rx_ctcs, &tx_ctcs,
            &rx_dcs, &tx_dcs, &power, &wide, &scan, &isam, &step, 0);

        fprintf (out, "%5s   %8.4f ", BAND_NAME[i], rx_hz / 1000000.0);
        print_offset (out, rx_hz, tx_hz);
        fprintf (out, " ");
        print_squelch (out, rx_ctcs, rx_dcs);
        fprintf (out, "   ");
        print_squelch (out, tx_ctcs, tx_dcs);

        fprintf (out, "   %-4s  %s\n", POWER_NAME[power],
            isam ? "AM" : wide ? "Wide" : "Narrow");
    }

    //
    // Programmable memory scan.
    //
    fprintf (out, "\n");
    if (verbose) {
        fprintf (out, "# Programmable memory scan: list of sub-band limits.\n");
        fprintf (out, "# 1) PMS pair number: 1-50\n");
        fprintf (out, "# 2) Lower frequency in MHz\n");
        fprintf (out, "# 3) Upper frequency in MHz\n");
        fprintf (out, "#\n");
    }
    fprintf (out, "PMS     Lower    Upper\n");
    for (i=0; i<NPMS; i++) {
        int lower_hz, upper_hz, tx_hz, rx_ctcs, tx_ctcs, rx_dcs, tx_dcs;
        int power, wide, scan, isam, step;

        decode_channel (i*2, OFFSET_PMS, 0, &lower_hz, &tx_hz, &rx_ctcs, &tx_ctcs,
            &rx_dcs, &tx_dcs, &power, &wide, &scan, &isam, &step, 0);
        decode_channel (i*2+1, OFFSET_PMS, 0, &upper_hz, &tx_hz, &rx_ctcs, &tx_ctcs,
            &rx_dcs, &tx_dcs, &power, &wide, &scan, &isam, &step, 0);
        if (lower_hz == 0 && upper_hz == 0)
            continue;

        fprintf (out, "%5d   ", i+1);
        if (lower_hz == 0)
            fprintf (out, "-       ");
        else
            fprintf (out, "%8.4f", lower_hz / 1000000.0);
        if (upper_hz == 0)
            fprintf (out, " -\n");
        else
            fprintf (out, " %8.4f\n", upper_hz / 1000000.0);
    }

    //
    // VFO channels.
    // Not much sense to store this to the configuration file.
    //
#if 0
    fprintf (out, "\n");
    fprintf (out, "VFO     Receive  Transmit R-Squel T-Squel Step  Power Modulation\n");
    for (i=0; i<5; i++) {
        int rx_hz, tx_hz, rx_ctcs, tx_ctcs, rx_dcs, tx_dcs;
        int power, wide, scan, isam, step;
        static const char *STEP_NAME[] = { "5.0", "10.0", "12.5", "15.0",
                                           "20.0", "25.0", "50.0", "100.0" };

        decode_channel (i, OFFSET_VFO, 0, &rx_hz, &tx_hz, &rx_ctcs, &tx_ctcs,
            &rx_dcs, &tx_dcs, &power, &wide, &scan, &isam, &step, 0);

        fprintf (out, "%5s   %8.4f ", BAND_NAME[i], rx_hz / 1000000.0);
        print_offset (out, rx_hz, tx_hz);
        fprintf (out, " ");
        print_squelch (out, rx_ctcs, rx_dcs);
        fprintf (out, "   ");
        print_squelch (out, tx_ctcs, tx_dcs);

        fprintf (out, "   %-5s %-4s  %s\n",
            STEP_NAME[step], POWER_NAME[power],
            isam ? "AM" : wide ? "Wide" : "Narrow");
    }
#endif
}

//
// Read memory image from the binary file.
//
static void ft60r_read_image (FILE *img, unsigned char *ident)
{
    if (fread (&radio_mem[0], 1, MEMSZ, img) != MEMSZ) {
        fprintf (stderr, "Error reading image data.\n");
        exit (-1);
    }
    memcpy (ident, radio_mem, 8);
}

//
// Save memory image to the binary file.
//
static void ft60r_save_image (FILE *img)
{
    memcpy (radio_mem, radio_ident, 8);
    fwrite (&radio_mem[0], 1, MEMSZ, img);
}

//
// Parse the scalar parameter.
//
static void ft60r_parse_parameter (char *param, char *value)
{
    if (strcasecmp ("Radio", param) == 0) {
        if (strcasecmp ("Yaesu FT-60R", value) != 0) {
            fprintf (stderr, "Bad value for %s: %s\n", param, value);
            exit(-1);
        }
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
    if (mhz >= 108 && mhz <= 520)
        return 1;
    if (mhz >= 700 && mhz <= 999)
        return 1;
    return 0;
}

#if 0
//
// Return the default step for a given frequency.
//
static int default_step (double mhz)
{
    static const struct {
        double min, max;
        int step;
    } tab [] = {
        { 108.000, 137.000, STEP_25   },
        { 137.000, 144.000, STEP_12_5 },
        { 144.000, 148.000, STEP_5    },
        { 148.000, 156.000, STEP_12_5 },
        { 156.000, 157.450, STEP_25   },
        { 157.450, 160.600, STEP_12_5 },
        { 160.600, 160.975, STEP_25   },
        { 160.975, 161.500, STEP_12_5 },
        { 161.500, 162.900, STEP_25   },
        { 162.900, 174.000, STEP_12_5 },
        { 174.000, 222.000, STEP_50   },
        { 222.000, 225.000, STEP_5    },
        { 225.000, 300.000, STEP_12_5 },
        { 300.000, 336.000, STEP_100  },
        { 336.000, 420.000, STEP_12_5 },
        { 420.000, 450.000, STEP_25   },
        { 450.000, 470.000, STEP_12_5 },
        { 470.000, 520.000, STEP_50   },
        { 700.000, 800.000, STEP_50   },
        { 800.000, 999.990, STEP_12_5 },
        { -1 },
    };
    int i;

    for (i=0; tab[i].min>0; i++) {
        if (mhz >= tab[i].min && mhz < tab[i].max)
            return tab[i].step;
    }
    return STEP_5;
}
#endif

//
// Parse the 'banks' parameter value.
//
static int encode_banks (char *str)
{
    int mask;

    if (*str == '-')
        return 0;

    for (mask=0; *str; str++) {
        if (*str < '0' || *str > '9') {
            fprintf (stderr, "Bad banks mask = '%s'\n", str);
            exit(-1);
        }
        mask |= 1 << (*str - '0');
    }
    return mask;
}

//
// Parse one line of memory channel table.
// Start_flag is 1 for the first table row.
// Return 0 on failure.
//
static int parse_channel (int first_row, char *line)
{
    char num_str[256], name_str[256], rxfreq_str[256], offset_str[256];
    char rq_str[256], tq_str[256], power_str[256], wide_str[256];
    char scan_str[256], banks_str[256];
    int num, tmode, tone, dtcs, power, wide, scan, isam, banks;
    double rx_mhz, tx_mhz;

    if (sscanf (line, "%s %s %s %s %s %s %s %s %s %s",
        num_str, name_str, rxfreq_str, offset_str, rq_str, tq_str, power_str,
        wide_str, scan_str, banks_str) != 10)
        return 0;

    num = atoi (num_str);
    if (num < 1 || num > NCHAN) {
        fprintf (stderr, "Bad channel number.\n");
        return 0;
    }

    if (sscanf (rxfreq_str, "%lf", &rx_mhz) != 1 ||
        ! is_valid_frequency (rx_mhz)) {
        fprintf (stderr, "Bad receive frequency.\n");
        return 0;
    }
    if (sscanf (offset_str, "%lf", &tx_mhz) != 1) {
badtx:  fprintf (stderr, "Bad transmit frequency.\n");
        return 0;
    }
    if (offset_str[0] == '-' || offset_str[0] == '+')
        tx_mhz += rx_mhz;
    if (! is_valid_frequency (tx_mhz))
        goto badtx;

    tmode = encode_squelch (rq_str, tq_str, &tone, &dtcs);

    if (strcasecmp ("High", power_str) == 0) {
        power = 0;
    } else if (strcasecmp ("Mid", power_str) == 0) {
        power = 1;
    } else if (strcasecmp ("Low", power_str) == 0) {
        power = 2;
    } else {
        fprintf (stderr, "Bad power level.\n");
        return 0;
    }

    if (strcasecmp ("Wide", wide_str) == 0) {
        wide = 1;
        isam = 0;
    } else if (strcasecmp ("Narrow", wide_str) == 0) {
        wide = 0;
        isam = 0;
    } else if (strcasecmp ("AM", wide_str) == 0) {
        wide = 1;
        isam = 1;
    } else {
        fprintf (stderr, "Bad modulation width.\n");
        return 0;
    }

    if (*scan_str == '+') {
        scan = 0;
    } else if (*scan_str == '-') {
        scan = 1;
    } else if (strcasecmp ("Only", scan_str) == 0) {
        scan = 2;
    } else {
        fprintf (stderr, "Bad scan flag.\n");
        return 0;
    }

    banks = encode_banks (banks_str);

    if (first_row) {
        // On first entry, erase the channel table.
        int i;
        for (i=0; i<NCHAN; i++) {
            setup_channel (i, 0, 0, 0, 0, TONE_DEFAULT, 0, 0, 1, 0, 0, 0);
        }
    }

    setup_channel (num-1, name_str, rx_mhz, tx_mhz,
        tmode, tone, dtcs, power, wide, scan, isam, banks);
    return 1;
}

//
// Parse one line of home channel table.
// Return 0 on failure.
//
static int parse_home (int first_row, char *line)
{
    char band_str[256], rxfreq_str[256], offset_str[256];
    char rq_str[256], tq_str[256], power_str[256], wide_str[256];
    int band, tmode, tone, dtcs, power, wide, isam;
    double rx_mhz, tx_mhz;

    if (sscanf (line, "%s %s %s %s %s %s %s",
        band_str, rxfreq_str, offset_str, rq_str, tq_str,
        power_str, wide_str) != 7)
        return 0;

    band = atoi (band_str);
    if (band != 144 && band != 250 && band != 350 &&
        band != 430 && band != 850) {
        fprintf (stderr, "Incorrect band.\n");
        return 0;
    }

    if (sscanf (rxfreq_str, "%lf", &rx_mhz) != 1 ||
        ! is_valid_frequency (rx_mhz)) {
        fprintf (stderr, "Bad receive frequency.\n");
        return 0;
    }
    if (sscanf (offset_str, "%lf", &tx_mhz) != 1) {
badtx:  fprintf (stderr, "Bad transmit frequency.\n");
        return 0;
    }
    if (offset_str[0] == '-' || offset_str[0] == '+')
        tx_mhz += rx_mhz;
    if (! is_valid_frequency (tx_mhz))
        goto badtx;

    tmode = encode_squelch (rq_str, tq_str, &tone, &dtcs);

    if (strcasecmp ("High", power_str) == 0) {
        power = 0;
    } else if (strcasecmp ("Mid", power_str) == 0) {
        power = 1;
    } else if (strcasecmp ("Low", power_str) == 0) {
        power = 2;
    } else {
        fprintf (stderr, "Bad power level.\n");
        return 0;
    }

    if (strcasecmp ("Wide", wide_str) == 0) {
        wide = 1;
        isam = 0;
    } else if (strcasecmp ("Narrow", wide_str) == 0) {
        wide = 0;
        isam = 0;
    } else if (strcasecmp ("AM", wide_str) == 0) {
        wide = 1;
        isam = 1;
    } else {
        fprintf (stderr, "Bad modulation width.\n");
        return 0;
    }

    setup_home (band, rx_mhz, tx_mhz, tmode, tone, dtcs, power, wide, isam);
    return 1;
}

//
// Parse one line of PMS table.
// Return 0 on failure.
//
static int parse_pms (int first_row, char *line)
{
    char num_str[256], lower_str[256], upper_str[256];
    int num;
    double lower_mhz, upper_mhz;

    if (sscanf (line, "%s %s %s", num_str, lower_str, upper_str) != 3)
        return 0;

    num = atoi (num_str);
    if (num < 1 || num > NPMS) {
        fprintf (stderr, "Bad PMS number.\n");
        return 0;
    }
    if (sscanf (lower_str, "%lf", &lower_mhz) != 1 ||
        ! is_valid_frequency (lower_mhz)) {
        fprintf (stderr, "Bad lower frequency.\n");
        return 0;
    }
    if (sscanf (upper_str, "%lf", &upper_mhz) != 1 ||
        ! is_valid_frequency (upper_mhz)) {
        fprintf (stderr, "Bad upper frequency.\n");
        return 0;
    }

    if (first_row) {
        // On first entry, erase the PMS table.
        int i;
        for (i=0; i<NPMS; i++) {
            setup_pms (i, 0, 0);
        }
    }
    setup_pms (num-1, lower_mhz, upper_mhz);
    return 1;
}

//
// Parse table header.
// Return table id, or 0 in case of error.
//
static int ft60r_parse_header (char *line)
{
    if (strncasecmp (line, "Channel", 7) == 0)
        return 'C';
    if (strncasecmp (line, "Home", 4) == 0)
        return 'H';
    if (strncasecmp (line, "PMS", 3) == 0)
        return 'P';
    return 0;
}

//
// Parse one line of table data.
// Return 0 on failure.
//
static int ft60r_parse_row (int table_id, int first_row, char *line)
{
    switch (table_id) {
    case 'C': return parse_channel (first_row, line);
    case 'H': return parse_home (first_row, line);
    case 'P': return parse_pms (first_row, line);
    }
    return 0;
}

//
// Yaesu FT-60R
//
radio_device_t radio_ft60r = {
    "Yaesu FT-60R",
    ft60r_download,
    ft60r_upload,
    ft60r_read_image,
    ft60r_save_image,
    ft60r_print_version,
    ft60r_print_config,
    ft60r_parse_parameter,
    ft60r_parse_header,
    ft60r_parse_row,
};
