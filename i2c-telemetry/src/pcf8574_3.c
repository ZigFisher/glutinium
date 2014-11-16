/*
    PCF8574_addr_byteout.c
    ----------------------
    Send address and byte to switch lines on 8 line port of i2c bus PCF8574
    -----------------------------------------------------------------------

      1) send the i2c address
      2) send byte to set the 8 port lines on or off

  usage :-  type with spaces but without the < >
<PCF8574_addr_byteout.c>  <address as decimal> <byte out value as decimal 0-255>

*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>

  int i2c;
  int buf[1];
  int address;

int main(int argc, char** argv)
{
 if (argc != 3)   /* error if we are not getting just 2 inputs after the program name */
 {
  printf("Error. usage: %s i2c_chip_address byte_to_send_out\n", argv[0]);           
 }

/* address is the first number after the program name */
	address = atoi(argv[1]);

/* the byte to send out to the PC8574 is the second number */

/* place it into the first element of the buf array */
	buf[0] = atoi(argv[2]);

  i2c = open("/dev/i2c/0",O_RDWR); /* open the device on i2c-bus number 0*/

  ioctl(i2c,I2C_SLAVE, address); /* set the i2c chip address */

  write(i2c,buf,1); /* send the byte */

  i2c = close(i2c);

}

