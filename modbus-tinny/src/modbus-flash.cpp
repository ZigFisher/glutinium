/*********************************************************************
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2018
 *
 * Balint Cristian <cristian dot balint at gmail dot com>
 *
 * TinnyModbus
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *   * Neither the name of the copyright holders nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************/

/*

  modbus-flash.c (Burn intel-hex file data over modbus)

*/

#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <vector>



/*
 * Compute CRC for message.
 */
u_int16_t msg_crc( unsigned char *buf, int len )
{
  u_int16_t crc = 0xffff;
  for ( int pos = 0; pos < len; pos++ )
  {
    // XOR byte into least
    // significant byte of crc
    crc ^= (u_int16_t) buf[pos];

    // loop over each bit
    for ( int i = 8; i != 0; i-- )
    {
      // if the LSB is set
      if ( (crc & 0x0001) != 0 )
      {
        crc >>= 1;  // shift right and XOR 0xa001
        crc  ^= 0xA001;
      }
      else          // else LSB is not set
        crc >>= 1;  // just shift right
    }
  }
  return crc;
}

/*
 * Set UART (rs485) line attributes.
 */
int set_interface_attribs( int fd, int speed )
{
    struct termios tty;

    if ( tcgetattr( fd, &tty ) < 0 )
    {
      printf( "Error from tcgetattr: %s\n", strerror( errno ) );
      return -1;
    }

    // set speed
    cfsetospeed( &tty, (speed_t)speed );
    cfsetispeed( &tty, (speed_t)speed );

    // ignore modem controls
    tty.c_cflag |= ( CLOCAL | CREAD );

    // flush UART buffers
    tcsetattr(fd, TCSANOW, &tty);
    tcsetattr(fd, TCSAFLUSH, &tty);

    // flush wait
    usleep(10000);
    tcflush(fd, TCIOFLUSH);

    // 8N1 flow control
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;      // 8-bit characters
    tty.c_cflag &= ~PARENB;  // no parity bit
    tty.c_cflag &= ~CSTOPB;  // only need 1 stop bit
    tty.c_cflag &= ~CRTSCTS; // no hardware flowcontrol

    // setup for non-canonical mode
    tty.c_iflag &= ~( IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON );
    tty.c_lflag &= ~( ECHO | ECHONL | ICANON | ISIG | IEXTEN );
    tty.c_oflag &= ~OPOST;

    // fetch bytes
    // as they become
    // available
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 1;

    // line error
    if ( tcsetattr( fd, TCSANOW, &tty ) != 0 )
    {
      printf( "Error from tcsetattr: %s\n", strerror( errno ) );
      return -1;
    }
    return 0;
}

/*
 * Set UART (rs485) buffer behaviour.
 */
void set_mincount( int fd, int mcount )
{
    struct termios tty;

    if ( tcgetattr( fd, &tty ) < 0 )
    {
      printf( "Error tcgetattr: %s\n", strerror( errno ) );
      return;
    }

    tty.c_cc[VMIN]  = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5; // half second timer

    if ( tcsetattr( fd, TCSANOW, &tty ) < 0 )
      printf( "Error tcsetattr: %s\n", strerror( errno ) );
}

/*
 *  MAIN
 */
int main( int argc, char**argv )
{

    if (argc != 3)
    {
      printf( "Usage: modbus-flash <slave-addr> <intel-hexfile.hex>\n" );
      exit( 1 );
    }

    if ( (  (int)strtol(argv[1], NULL, 16) < 1 )
        || ((int)strtol(argv[1], NULL, 16) > 255)
       )
    {
      printf("Slave address is is invalid.\n");
      exit(0);
    }


    int chr;
    FILE* fhex;
    // data base address
    int baseaddress = -1;
    // data storage container
    std::vector<uint8_t> dataflash;


    // open intel hex file
    char *hexfile = argv[2];
    fhex = fopen( hexfile, "r" );

    if ( fhex == 0 )
    {
      printf("Canot open input file [%s]\n", hexfile);
      exit(-1);
    }

    int addr = 0;
    // parse hexfile
    while ( ( chr = fgetc(fhex)) != EOF )
    {
      if ( chr == ':' )
      {

        char buf[256];

        buf[0] = fgetc(fhex);
        buf[1] = fgetc(fhex);
        buf[2] = '\0';
        // fetch dataline length
        int num = (int) strtol(buf, NULL, 16);

        buf[0] = fgetc(fhex);
        buf[1] = fgetc(fhex);
        buf[2] = fgetc(fhex);
        buf[3] = fgetc(fhex);
        buf[4] = '\0';

        if ( (int) strtol(buf, NULL, 16) < addr )
          break;
        // fetch dataline start address
        addr = (int) strtol(buf, NULL, 16);

        if ( baseaddress == -1 )
          baseaddress = addr;

        buf[0] = fgetc(fhex);
        buf[1] = fgetc(fhex);
        buf[2] = '\0';

        // fetch dataline content type
        int typhex = (int) strtol(buf, NULL, 16);

        if ( typhex > 1 )
        {
           printf("Only IHEX8 supported.\n");
           exit(-1);
        }

        // store data
        for (int i = 0; i < num; i++)
        {
          buf[0] = fgetc(fhex);
          buf[1] = fgetc(fhex);
          buf[2] = '\0';

          // store octet
          uint8_t val = (uint8_t) strtol(buf, NULL, 16);
          dataflash.push_back(val);

        }
      }
    }

    fclose(fhex);


    printf("Loaded %lu octets from [%s] @0x%04x base.\n", dataflash.size(), hexfile, baseaddress);


    int fd;
    int wlen = 0;

    const char *io = "/dev/ttyUSB0";

    fd = open( io, O_RDWR | O_NOCTTY | O_SYNC );

    if ( fd < 0 )
    {
        printf( "Error opening %s: %s\n", io, strerror(errno) );
        return -1;
    }

    // 38400 8N1
    set_interface_attribs( fd, B9600 );
    // set to timed read
    set_mincount( fd, 1 );

    // blank modbus frame
    unsigned char msg[40] =
    {
      0x01, // slave address
      0x10, // fcode
      0x00, 0x00, // register start
      0x00, 0x10, // number of registers
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff  // crc
    };

    // slv address
    msg[0] = (int)strtol(argv[1], NULL, 16);


    int offset = 0;
    // burn data in 32 octet batches
    for ( long unsigned int i = 0; i < dataflash.size(); i++ )
    {

      // place octet on wire message
      msg[offset + 6] = dataflash[i];

      // time to send message on wire
      if ( ( ( i != 0) && ( (i+1) % 32 == 0 ) ) ||
           ( i+1 == dataflash.size() ) )
      {
        // base address
        msg[2] = (baseaddress & 0x0000ff00) >> 8;
        msg[3] = (baseaddress & 0x000000ff);

        // add crc to message
        u_int16_t crc = msg_crc( msg, 38 );
        msg[38] = (crc & 0x00ff);
        msg[39] = (crc & 0xff00) >> 8;

        wlen = write( fd, msg, 40 );
        if ( wlen != 40 )
        {
           printf( "Error from write: %d, %d\n", wlen, errno );
           exit(-1);
        }

        printf("TX:");
        for ( int i = 0; i < 38; i++ )
        {
          printf( " %02x", msg[i] );
        }

        printf(" CRC [0x");
        for ( int i = 38; i < 40; i++ )
        {
          printf( "%02x", msg[i] );
        }
        printf("]");

        // small buffer
        unsigned char buf[40];

        // read header
        printf("\nRX:");
        for ( int i = 0; i < 6; i++ )
        {
          read( fd, &buf[i], 0x1 );
          printf( " %02x", buf[i] );
        }
        // read crc
        for ( int i = 0; i < 2; i++ )
          read( fd, &buf[i], 0x1 );

        unsigned char locrc = buf[0];
        unsigned char hicrc = buf[1];
        printf( " CRC [0x%02x%02x]\n\n", locrc, hicrc );
        fflush( stdout );

        // reset message buffer
        for ( int k = 0; k < 32; k++ )
          msg[k + 6] = 0xff;

        // reset offset
        offset = 0;
        // increment base
        baseaddress += 32;

        continue;

      }
      offset++;
    }

    return 0;
}
