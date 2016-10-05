/*
	Created: September 2013
	by ihsan Kehribar <ihsan@kehribar.me>
	
	I2C bus address search and nunchuck example
*/

#include <stdio.h>
#include <stdlib.h>
#include "littleWire.h"
#include "littleWire_util.h"

unsigned char i=0;
unsigned char rc;
unsigned char version;
unsigned char myBuffer[8];
unsigned char response[6];

int analogX;
int analogY;
int accelX;
int accelY;
int accelZ;
int zButton;
int cButton;

int main()
{
	littleWire *lw = NULL;

	lw = littleWire_connect();

	if(lw == NULL){
		printf("> Little Wire could not be found!\n");
		exit(EXIT_FAILURE);
	}

	version = readFirmwareVersion(lw);
	printf("> Little Wire firmware version: %d.%d\n",((version & 0xF0)>>4),(version&0x0F));	
	if(version<=0x12)
	{
		printf("> This example requires the new 1.3 version firmware. Please update soon.\n");
		return 0;
	}
	
	i2c_init(lw);
	i2c_updateDelay(lw,5);

	printf("> Address search ... \n");
	
	for(i=0;i<128;i++)
	{
		rc = i2c_start(lw,i,WRITE);	
		if(rc==1)
			printf("> Found device at %3d\n",i);
	}
	
	delay(2000);

	/* init the nunchuck */
	i2c_start(lw,0x52,WRITE);
		myBuffer[0] = 0x40;
		myBuffer[1] = 0x00;
	i2c_write(lw,myBuffer,2,END_WITH_STOP);

	for(;;)
	{ 			

		/* Read message! */		
		i2c_start(lw,0x52,WRITE);
			myBuffer[0] = 0x00;			
		i2c_write(lw,myBuffer,1,END_WITH_STOP);		
		delay(100);
		
		/* Get the standard response */
		i2c_start(lw,0x52,READ);
		i2c_read(lw,myBuffer,6,END_WITH_STOP);
		
		for(i=0;i<6;i++)
		{
			/* XOR with 0x17 and add 0x17 */
			response[i] = (myBuffer[i] ^ 0x17 ) + 0x17;
		}
		
		/* Decode the message */
		analogX = response[0];
		analogY = response[1];
		accelX = (response[2] << 2) | ((response[5] >> 2) & 3);
		accelY = (response[3] << 2) | ((response[5] >> 4) & 3);
		accelZ = (response[4] << 2) | ((response[5] >> 6) & 3);
		zButton = !((response[5] >> 0) & 1);
		cButton = !((response[5] >> 1) & 1);
		
		printf("> ---------------------------------------\n");
		printf("> RAW:");
		for(i=0;i<5;i++)
		{
			printf("%d,",response[i]);
		}
		printf("%d\r\n",response[5]);

		printf("> joy_x: %d\n",analogX);
		printf("> joy_y: %d\n",analogY);
		printf("> accel_x: %d\n",accelX);
		printf("> accel_y: %d\n",accelY);
		printf("> accel_z: %d\n",accelZ);
		printf("> zButton %d\n",zButton);
		printf("> cButton %d\n",cButton);
			
		if(lwStatus<0)
		{
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error!\n");
			return 0;
		}
	}
	
	return 0;
}
