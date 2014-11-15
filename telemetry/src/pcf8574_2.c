/* 
PCF8574_addr_read_slug.c
-------------------
read the data on the 8 line port of an i2c bus PCF8574
------------------------------------------------------

1) set the i2c address of the PCF8574
2) send data 255 to turn off all 8 lines - make them inputs
3) read the data on the 8 port lines as a byte
usage :- type (without the < >)
<PCF8574_address_read_4> <space> <i2c_bus_address_of_PCF8574_as_decimal>

Home Page - http://www.sunspot.co.uk/Projects/SWEEX/slug/i2c/slug_i2c.html

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#define I2C_SLAVE0x0703/* Change slave address*/

int i2c;
char filename[20];
unsigned long address;
int rc;
unsigned char data[1];
int byte; /* the 8 bit byte that represents the data present on the 8 wire port */

int main(int argc, char ** argv) 
{
if (argc != 2) { /* error if we are not getting just 1 input after the program name */
fprintf(stderr, "usage: %s <address> <databyte>\n",argv[0]);
exit(1);
}

/* address is the first number after the program name */
address = atoi(argv[1]);

i2c = open("/dev/i2c-0",O_RDWR); /* open the device dev/i2c-0 */
rc=ioctl(i2c,I2C_SLAVE,address); /* set the i2c chip address */

data[0] = 255; 
write(i2c,data,1); /* we send 255 to make all lines go off, ready for input data */

read(i2c,data,1); /* now data(0) contains the byte of data on the 8 port lines*/

byte = data[0];

printf("BYTE = %d \n", byte); 
/* BYTE value is 0-256)*/
i2c = close(i2c);
return(0);
}
