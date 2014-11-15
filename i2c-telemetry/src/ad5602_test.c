/* 
    AD5602_test1.c
    ---------------------
    set the output voltage from the D/A
    -----------------------------------
 
      1) send the i2c address
      2) send HI_byte
      3) send LO_byte
      
	usage :-  type with spaces but without the < >
	<AD5602_test1>  <address as decimal> <HI_byte> <LO_byte>
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>

int main(int argc, char** argv)
{
  int i2c; 
  int HI_byte;   /* last 4 bits are output byte HI bits */
  int LO_byte;   /* first 4 bits are output byte LO bits */
  int buf[1];    /* use to feed the i2c driver */
  int address;   /* i2c bus address */

 if (argc != 4)   /* report error if we are not getting just 3 inputs after the program name */
 {
  printf("Error. usage (decimal): %s AD5602_test1 address HI_byte LO_byte\n", argv[0]);           
 }

  address = atoi(argv[1]); /* address is the first number after the program name */
  // address bits are  0 0 0 1   1 1 0 R/W as per manual
  // the 1 0  before R/W is for the address pin (pin 1) open-circuit
  // the R/W is 0 for write. Linux see the chip address as 0000 1110 (0E) - shifts a bit to the left - odd!
  HI_byte = atoi(argv[2]); /* HI_byte is 0 0 0 0 D7 D6 D5 D4   */
  LO_byte = atoi(argv[3]); /* LO_byte is D3 D2 D1 D0 X X X X   (X = anything) */

  i2c = open("/dev/i2c/0",O_RDWR); /* open the i2c-bus number 0 */

  ioctl(i2c,I2C_SLAVE,address); /* set the i2c-bus address of the chip we will talk to */

  buf[0] = HI_byte;         /* HI_byte */
  buf[1] = LO_byte;         /* LO_byte */


  write(i2c,buf,2); /* we send 2 bytes <HI_byte> <LO_byte> */

  printf("%d\n", buf[0]); /* just to prove it ran!*/
  printf("%d\n", buf[1]);

  i2c = close(i2c);
}

//  AD5602_test1 14 0 0  gives 0 volts
//  AD5602_test1 14 8 0  gives 2.5 volts     8 = 08 00001000    0 = 00 00000000
//  AD5602_test1 14 15 240  gives 5 volts   15 = 0F 00001111  240 = F0 11110000

