#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <termios.h>
#include <strings.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>


#define byte        unsigned char
#ifndef __uint32_t_defined
typedef unsigned int        uint32_t;
# define __uint32_t_defined
#endif


#define BSZ     255
#define UInt16      uint16_t

#define TIME_OUT    50 * 1000   // Mercury inter-command delay (mks)

#define OPT_DEBUG   "--debug"
#define OPT_HUMAN   "--human"

#define BAUDRATE    B9600       // 9600 baud
#define _POSIX_SOURCE   1       // POSIX compliant source
#define CH_TIME_OUT 2       // Channel timeout (sec)

int debugPrint = 0;

UInt16 ModRTU_CRC(byte* buf, int len)
{
  UInt16 crc = 0xFFFF;

  for (int pos = 0; pos < len; pos++) {
    crc ^= (UInt16)buf[pos];          // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}

// **** Enums
typedef enum
{
    OUT = 0,
    IN = 1
} Direction;

typedef struct
{
    byte    address[4];
    byte    command;
    UInt16  CRC;
} GetSerialCmd; // 0x2F - Серийный номер

typedef struct
{
    byte    address[4];
    byte    command;
    byte    p1[2];
    byte    p2[2];
    byte    p3[2];
    UInt16  CRC;
} Resultu;

typedef struct
{
    byte    address[4];
    byte    command;
    byte    p1[4];
    byte    p2[4];
    byte    p3[4];
    byte    p4[4];
    UInt16  CRC;
} ResultU;

typedef struct
{
    byte    address[4];
    byte    command;
    byte    f[2];
    byte    xz[8];
    UInt16  CRC;
} Resultf;

typedef enum            // Output formatting
{
    OF_HUMAN = 0,       // human readable
    OF_CSV = 1,     // comma-separated values
    OF_JSON = 2     // json
} OutputFormat;

// -- Print out data buffer in hex
void printPackage(byte *data, int size, int isin)
{
    if (debugPrint)
    {
        printf("%s bytes: %d\n\r\t", (isin) ? "Received" : "Sent", size);
        for (int i=0; i<size; i++)
            printf("%02X ", (byte)data[i]);
        printf("\n\r");
    }
}

void exitFailure(const char* msg)
{
    printf("{\n\t\"Error\": \"%s\",\n\t\"Serial\": -1, \n\t\"Voltage\": -1, \n\t\"Current\": -1, \n\t\"Power\": -1, \
\n\t\"Frequency\": -1, \n\t\"Energy_1\": -1, \n\t\"Energy_2\": -1, \n\t\"Energy_3\": -1, \n\t\"Energy_4\": -1\n}\n", msg);
    exit(1);
}

// Decode float from 3 bytes
float B3F(byte b[3], float factor)
{
    int val = ((b[0] & 0x3F) << 16) | (b[2] << 8) | b[1];
    return val/factor;
}

int nb_read_impl(int fd, byte* buf, int sz)
{
    fd_set set;
    struct timeval timeout;

    // Initialise the input set
    FD_ZERO(&set);
    FD_SET(fd, &set);

    // Set timeout
    timeout.tv_sec = CH_TIME_OUT;
    timeout.tv_usec = 0;

    int r = select(fd + 1, &set, NULL, NULL, &timeout);
    if (r < 0)
        exitFailure("Select failed.");
    if (r == 0)
        return 0;

    return read(fd, buf, BSZ);
}

int main(int argc, const char** args) {
    char dev[BSZ];

    int format = OF_JSON;
    byte initbytes[11];
    byte ubytes[14];
    byte freqbytes[17];
    byte Ubytes[23];
    struct termios oldtio, newtio;
    for (int i=3; i<argc; i++)
    {
        if (!strcmp(OPT_DEBUG, args[i]))
            debugPrint = 1;
        else if (!strcmp(OPT_HUMAN, args[i]))
            format = OF_HUMAN;
    }
    // get RS485 address (1st required param)
    if (argc < 2)
    {
        exitFailure("No RS485 port specified");
    }
    if (argc < 3)
    {
        exitFailure("No device ADDRESS specified");
    }



    strncpy(dev, args[1], BSZ);
    int address = atoi(args[2]);
    if (address == 0) {
        exitFailure("Device ADDRESS incorrect");
    }

    byte B_address[4];

    B_address[0] = (address >> 24) & 0xFF;
    B_address[1] = (address >> 16) & 0xFF;
    B_address[2] = (address >> 8) & 0xFF;
    B_address[3] = address & 0xFF;

    for (int i=3; i<argc; i++)
    {
        if (!strcmp(OPT_DEBUG, args[i]))
            debugPrint = 1;
        else if (!strcmp(OPT_HUMAN, args[i]))
            format = OF_HUMAN;
    }

    if (debugPrint == 1){
        printf("ADDRESS device :%d\n\r", address);
    }

    int fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0)
        exitFailure("Problem on serial port");

    fcntl(fd, F_SETFL, 0);

    tcgetattr(fd, &oldtio); /* save current port settings */

    bzero(&newtio, sizeof(newtio));

    cfsetispeed(&newtio, BAUDRATE);
    cfsetospeed(&newtio, BAUDRATE);

    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
//  newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
//  newtio.c_cflag = BAUDRATE | CS8 | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    cfmakeraw(&newtio);
    tcsetattr(fd, TCSANOW, &newtio);

    GetSerialCmd serialCmd = {
     .address = { B_address[0], B_address[1], B_address[2], B_address[3] },
     .command = 0x2F
    };
    serialCmd.CRC = ModRTU_CRC((byte*)&serialCmd, sizeof(serialCmd) - sizeof(UInt16));

    ssize_t res = write(fd, (byte*)&serialCmd, sizeof(serialCmd));
    if (res >= 0) {
        printPackage((byte*)&serialCmd, sizeof(serialCmd), OUT);
    } else {
        exitFailure("Failed to write to port.");
    }
    ssize_t initsize = nb_read_impl(fd, &initbytes, 11);
    if (initsize > 0) {
        printPackage((byte*)&initbytes, initsize, IN);
    } else {
        exitFailure("Failed read from port");
    }

// ==========================================================
    usleep(TIME_OUT);

    GetSerialCmd uCmd = {
     .address = { B_address[0], B_address[1], B_address[2], B_address[3] },
     .command = 0x63
    };
    uCmd.CRC = ModRTU_CRC((byte*)&uCmd, sizeof(uCmd) - sizeof(UInt16));
    res = write(fd, (byte*)&uCmd, sizeof(uCmd));
    if (res >= 0) {
        printPackage((byte*)&uCmd, sizeof(uCmd), OUT);
    } else {
        exitFailure("Failed to write to port");
    }
    ssize_t usize = nb_read_impl(fd, &ubytes, 14);
    if (usize > 0) {
        printPackage((byte*)&ubytes, usize, IN);
    } else {
        exitFailure("Failed read from port.");
    }

// ==========================================================
    usleep(TIME_OUT);

    GetSerialCmd fCmd = {
     .address = { B_address[0], B_address[1], B_address[2], B_address[3] },
     .command = 0x81
    };
    fCmd.CRC = ModRTU_CRC((byte*)&fCmd, sizeof(fCmd) - sizeof(UInt16));
    res = write(fd, (byte*)&fCmd, sizeof(fCmd));
    if (res >= 0) {
        printPackage((byte*)&fCmd, sizeof(fCmd), OUT);
    } else {
        exitFailure("Failed to write to port.");
    }
    ssize_t fsize = nb_read_impl(fd, &freqbytes, 17);
    if (fsize > 0) {
        printPackage((byte*)&freqbytes, fsize, IN);
    } else {
        exitFailure("Failed read from port.");
    }

// ==========================================================
    usleep(TIME_OUT);

    GetSerialCmd UCmd = {
     .address = { B_address[0], B_address[1], B_address[2], B_address[3] },
     .command = 0x27
    };
    UCmd.CRC = ModRTU_CRC((byte*)&UCmd, sizeof(UCmd) - sizeof(UInt16));
    res = write(fd, (byte*)&UCmd, sizeof(UCmd));
    if (res >= 0) {
        printPackage((byte*)&UCmd, sizeof(UCmd), OUT);
    } else {
        exitFailure("Failed to write to port.");
    }
    ssize_t Usize = nb_read_impl(fd, &Ubytes, 23);
    if (fsize > 0) {
        printPackage((byte*)&Ubytes, Usize, IN);
        if (debugPrint) {
            printf("\n=======================================================\n\n\r");
        }
    } else {
        exitFailure("Failed read from port.");
    }

// ==========================================================
    // Result
    Resultu* result = (Resultu*)ubytes;
    ResultU* resultU = (ResultU*)Ubytes;
    int u;
    int i;
    int p;
    int freq;
    int energy_1;
    int energy_2;
    int energy_3;
    int energy_4;

    char u_buffer[16];
    sprintf (u_buffer, "%02X%02X ", (byte)result->p1[0], (byte)result->p1[1]);
    sscanf(u_buffer, "%d", &u);

    char i_buffer[16];
    sprintf (i_buffer, "%02X%02X ", (byte)result->p2[0], (byte)result->p2[1]);
    sscanf(i_buffer, "%d", &i);

    char p_buffer[16];
    sprintf (p_buffer, "%02X%02X%02X ", (byte)result->p3[0], (byte)result->p3[1], (byte)result->p3[2]);
    sscanf(p_buffer, "%d", &p);

    Resultf* resultf = (Resultf*)freqbytes;
    char fr_buffer[16];
    sprintf (fr_buffer, "%02X%02X ", (byte)resultf->f[0], (byte)resultf->f[1]);
    sscanf(fr_buffer, "%d", &freq);

    char U_buffer[32];
    sprintf (U_buffer, "%02X%02X%02X%02X ", (byte)resultU->p1[0], (byte)resultU->p1[1],
        (byte)resultU->p1[2], (byte)resultU->p1[3]);
    sscanf(U_buffer, "%d", &energy_1);
    sprintf (U_buffer, "%02X%02X%02X%02X ", (byte)resultU->p2[0], (byte)resultU->p2[1],
        (byte)resultU->p2[2], (byte)resultU->p2[3]);
    sscanf(U_buffer, "%d", &energy_2);
    sprintf (U_buffer, "%02X%02X%02X%02X ", (byte)resultU->p3[0], (byte)resultU->p3[1],
        (byte)resultU->p3[2], (byte)resultU->p3[3]);
    sscanf(U_buffer, "%d", &energy_3);
    sprintf (U_buffer, "%02X%02X%02X%02X ", (byte)resultU->p4[0], (byte)resultU->p4[1],
        (byte)resultU->p4[2], (byte)resultU->p4[3]);
    sscanf(U_buffer, "%d", &energy_4);

    if (format == OF_HUMAN) {
        printf("Адрес устройства : %d\n", address);
        printf("Напряжение, V    : %.2f\n", u/10.0);
        printf("Ток, A           : %.2f\n", i/100.0);
        printf("Мощность, Wt     : %d\n", p);
        printf("Частота, Hz      : %.2f\n", freq/100.0);
        printf("Показания, кВт*ч : %.2f, %.2f, %.2f, %.2f\n",
            energy_1/100.0, energy_2/100.0, energy_3/100.0, energy_4/100.0);
    } else {
//         printf("{\"Serial\": %d, \"Voltage\": %f, \"Current\": %f, \"Power\": %d, \
// \"Frequency\": %f, \"Energy\": [%d, %d, %d, %d]}", address, u/10.0, i/100.0, p,
//             freq/100.0, energy_1, energy_2, energy_3, energy_4);
        printf("{\n\t\"Serial\": %d, \n\t\"Voltage\": %.2f, \n\t\"Current\": %.2f, \n\t\"Power\": %d, \
\n\t\"Frequency\": %.2f, \n\t\"Energy_1\": %.2f, \n\t\"Energy_2\": %.2f, \n\t\"Energy_3\": %.2f, \n\t\"Energy_4\": %.2f\n}\n", address, u/10.0, i/100.0, p,
            freq/100.0, energy_1/100.0, energy_2/100.0, energy_3/100.0, energy_4/100.0);
    }

    close(fd);
    tcsetattr(fd, TCSANOW, &oldtio);
    return 0;
}