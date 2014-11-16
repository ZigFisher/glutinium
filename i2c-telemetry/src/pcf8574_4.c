/*
    PCF8574_addr_read.c
    -------------------
read the data on the 8 line port of an i2c bus PCF8574
------------------------------------------------------

1) set the i2c address of the PCF8574
2) send data 255 to turn off all 8 lines - make them inputs
3) read the data on the 8 port lines as a byte
  usage :- type (without the < >)
  <PCF8574_address_read_4> <space> <i2c_bus_address_of_PCF8574_as_decimal>
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>

  int i2c;
  int buf[1];
  int address; /* i2c bus address of the PCF8574 */
  int byte;    /* the 8 bit byte that represents the data present on the 8 wire port */

int main(int argc, char** argv)
{
 if (argc != 2)   /* report error if we are not getting just 1 input after the program name */
 {
  printf("Error. usage: %s i2c_chip_address\n", argv[0]);
 }

  address = atoi(argv[1]); /* address is the first number after the program name */

  i2c = open("/dev/i2c/0",O_RDWR); /* open the i2c-bus number 0 */

  ioctl(i2c,I2C_SLAVE, address); /* set the address of the chip we will talk to */

  buf[0] = 255; 
  write(i2c,buf,1); /* we send 255 to make all lines go off, ready for input data */

  read(i2c,buf,1); /* now buf(0) contains the byte of data on the 8 port lines*/

  byte = buf[0];

  printf("BYTE = %d \n", byte); 
/* BYTE value is 0-256)*/

  i2c = close(i2c);
}

