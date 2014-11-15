/*
    PCF8591_addr_line.c
    -------------------
    read the voltage on one A/D line (0-3) and set the output voltage on the D/A pin
    --------------------------------------------------------------------------------

      1) send the i2c address as decimal
      2) send A/D line number (program adds 64 to enable D/A in control register)
      3) send byte for D/A out
      4) read the chosen A/D - print it as a raw decimal 0 to 255
      5) save the chosen port value to /var/www/ramdisk/AtoD.dat
  usage :-  type with spaces but without the < >
<PCF8591_address_readvolts>  <address as decimal> <A/D line 0-3> <D/A value as decimal 0-255>
      (even if the D/A pin is unused type in any value 0-255)
*/

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include "smbus.h"


int main(int argc, char** argv)
{
  int i2c; 
  int AD_line;      /* line number 0-3 */
  int V_out;        /* value for the D/A out line */
  int buf[1];       /* use to feed the i2c driver */
  int address;      /* i2c bus address */
  int byte_in_0;    /* the 8 bit byte that represents the voltage on the AD_line */

 if (argc != 4)   /* report error if we are not getting just 3 inputs after the program name */
 {
  printf("Error. usage: %s i2c_chip_address A/D_line_(0-3) D/A_Vout(0-255)\n", argv[0]);
 }

  address = atoi(argv[1]); /* address is the first number after the program name */
  AD_line = atoi(argv[2]); /* A/D input line 0-3 */
  AD_line = AD_line + 64;  /* enable the D/A output pin */
  V_out = atoi(argv[3]); /* A/D input line 0-3 */

  i2c = open("/dev/i2c/0",O_RDWR); /* open the i2c-bus number 0 */

  ioctl(i2c,I2C_SLAVE, address); /* set the i2c-bus address of the chip we will talk to */

  buf[0] = AD_line;       /* A/D input line number is the first data byte sent */
  buf[1] = V_out;         /* D/A out value is the second data byte sent */

  write(i2c,buf,2); /* we send 2 bytes <control register> <D/A value> */

  read(i2c,buf,1);  /* buf(0) contains the byte of data in the chip cache */
  /* do it again */
  read(i2c,buf,1);  /* buf(0) now contains the byte of data from the most recent analogue input*/

  printf("BYTE = %d \n", buf[0]);
  printf("%d\n", buf[0]);
  /* buf[0] is an integer. It goes 0 to 255 as voltage input goes from 0 to supply voltage*/

  //save AtoD byte to ramdisk
  FILE *f;
  f = fopen("/tmp/AtoD.dat","w");
  fprintf(f, "%d", buf[0]);
  fclose(f);

  i2c = close(i2c);
}


