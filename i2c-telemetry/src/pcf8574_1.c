/*
PCF8574_addr_byteout_slug.c
----------------------
Send address and byte to switch lines on 8 line port of i2c bus PCF8574
-----------------------------------------------------------------------

1) send the i2c address
2) send byte to set the 8 port lines on or off

usage :- type with spaces but without the < >
<PCF8574_addr_byteout_slug> <decimal address> <decimal byte out 0-255>

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
int main(int argc, char *argv[]) 
{
if (argc != 3) { /* error if we are not getting just 2 inputs after the program name */
fprintf(stderr, "usage: %s <address> <databyte>\n",argv[0]);
exit(1);
}

/* address is the first number after the program name */
address = atoi(argv[1]);
/* the byte to send out to the PC8574 is the second number
place it into the first element of the buf array */
data[0] = atoi(argv[2]);

i2c = open("/dev/i2c-0",O_RDWR); /* open the device dev/i2c-0 */

rc=ioctl(i2c,I2C_SLAVE,address); /* set the i2c chip address */

write(i2c,data,1) ; /* send the byte */

i2c = close(i2c);
return(0);
}

