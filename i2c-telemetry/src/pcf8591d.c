//***************************************************
// pcf8591d.c
// 
// Example to read A/D values from a 
// 4 channel / 8 bit AD converter PCF8591
// through I2C using the I2C driver improved by
// Geert Vancompernolle
// http://www.acmesystems.it/?id=10
//***************************************************

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "time.h"
#include "string.h"

#include "i2c_errno.h"
#include "etraxi2c.h"


int main( int argc, char **argv ) {
  int rtc;
  int fd_i2c;
  I2C_DATA i2c_d;
  int ch;
  
  printf("Reading from a PCF8591 (4 chanel A/D at 8 bits with I2C bus)\n");

  fd_i2c = open( "/dev/i2c", O_RDWR );
  if (fd_i2c<=0)     {
    printf( "Open error on /dev/i2c\n" );
    exit( 1 );
  }

  // PCF8591 address scheme
  // |  1 |  0 |  0 |  1 | A2 | A1 | A0 | R/W |
  i2c_d.slave =(0x09<<4)|(0x01<<1);

  for (ch=0;ch<=3;ch++) {  
    // Select the A/D channel
    i2c_d.wbuf[0] = ch;
    i2c_d.wlen  = 1;
    if ((rtc=ioctl(fd_i2c,_IO( ETRAXI2C_IOCTYPE, I2C_WRITE), &i2c_d))!=EI2CNOERRORS)  {
      close(fd_i2c);
      printf( "Error %d on line %d\n",rtc,__LINE__);
      return ( -1 );   
    }

    i2c_d.rlen  = 3;
    if ((rtc=ioctl(fd_i2c,_IO( ETRAXI2C_IOCTYPE, I2C_READ), &i2c_d))!=EI2CNOERRORS)  {
      close(fd_i2c);
      printf( "Error %d on line %d\n",rtc,__LINE__);
      return ( -1 );   
    }

    // Show the voltage level
    printf("Chanel %d = %.2fv (%02X hex)\n",ch,i2c_d.rbuf[2]*0.012941,i2c_d.rbuf[2]);
  }

  close(fd_i2c);
  return(0);
}

