/* 
PCF8591-all-lines_addr_V-slug.c
---------------------
read the voltage on all A/D lines (0-3) and set the output voltage on the D/A pin
---------------------------------------------------------------------------------

1) send the i2c address
3) send byte for D/A out
3) read all 4 A/D lines
usage :- type with spaces but without the < >
<PCF8591_address_readvolts> <address as decimal> <D/A value as decimal 0-255>
(even if the D/A pin is unused type in any value 0-255)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#define I2C_SLAVE0x0703/* Change slave address*/
int i2c; 
int AD_line; /* line number 0-3 */
int V_out; /* value for the D/A out line */
unsigned char buf[1]; /* use to feed the i2c driver */
unsigned long address; /* i2c bus address */
int byte_in_0; /* the 8 bit byte that represents the voltage on the AD_line */
int AD_line_0; /* volts byte on line number 0 */
int AD_line_1; /* volts byte on line number 1 */
int AD_line_2; /* volts byte on line number 2 */
int AD_line_3; /* volts byte on line number 3 */
int rc;

int main(int argc, char** argv)
{
if (argc != 3) /* report error if we are not getting just 2 inputs after the program name */

{

printf("Error. usage: %s i2c_chip_address D/A_Vout(0-255)\n", argv[0]);

}

address = atoi(argv[1]); /* address is the first number after the program name */

V_out = atoi(argv[2]); /* A/D input line 0-3 */

i2c = open("/dev/i2c-0",O_RDWR); /* open the device dev/i2c-0 */
rc=ioctl(i2c,I2C_SLAVE,address); /* set the i2c chip address */

AD_line = 0 + 64; /* enable the D/A output pin 0*/
buf[0] = AD_line; /* A/D input line number is the first data byte sent */
buf[1] = V_out; /* D/A out value is the second data byte sent */
write(i2c,buf,2); /* we send 2 bytes <control register> <D/A value> */
read(i2c,buf,1); /* buf(0) contains the byte of data in the chip cache */
sleep (1);	/* do it again */
read(i2c,buf,1); /* buf(0) now contains the byte of data from the most recent analogue input*/
AD_line_0 = buf[0];

AD_line = 1 + 64; /* enable the D/A output pin 0*/
buf[0] = AD_line; /* A/D input line number is the first data byte sent */
buf[1] = V_out; /* D/A out value is the second data byte sent */
write(i2c,buf,2); /* we send 2 bytes <control register> <D/A value> */
read(i2c,buf,1); /* buf(0) contains the byte of data in the chip cache */
//sleep (1);	/* do it again */
// read(i2c,buf,1); /* buf(0) now contains the byte of data from the most recent analogue input*/
AD_line_1 = buf[0];

AD_line = 2 + 64; /* enable the D/A output pin 0*/
buf[0] = AD_line; /* A/D input line number is the first data byte sent */
buf[1] = V_out; /* D/A out value is the second data byte sent */
write(i2c,buf,2); /* we send 2 bytes <control register> <D/A value> */
read(i2c,buf,1); /* buf(0) contains the byte of data in the chip cache */
//sleep (1);	/* do it again */
// read(i2c,buf,1); /* buf(0) now contains the byte of data from the most recent analogue input*/
AD_line_2 = buf[0];

AD_line = 3 + 64; /* enable the D/A output pin 0*/
buf[0] = AD_line; /* A/D input line number is the first data byte sent */
buf[1] = V_out; /* D/A out value is the second data byte sent */
write(i2c,buf,2); /* we send 2 bytes <control register> <D/A value> */
read(i2c,buf,1); /* buf(0) contains the byte of data in the chip cache */
//sleep (1);	/* do it again */
// read(i2c,buf,1); /* buf(0) now contains the byte of data from the most recent analogue input*/
AD_line_3 = buf[0];

printf("A/D line 0 = %d \n", AD_line_0, " "); 
printf("A/D line 1 = %d \n", AD_line_1, " ");
printf("A/D line 2 = %d \n", AD_line_2, " ");
printf("A/D line 3 = %d \n", AD_line_3, " ");
/*AD_line_x goes 0 to 255 as voltage input goes from 0 to supply voltage*/

i2c = close(i2c);
return(0);
}
