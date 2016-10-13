/*
	Updated for the new firmware: July 2012
	by ihsan Kehribar <ihsan@kehribar.me>

	Created: December 2011
	by ihsan Kehribar <ihsan@kehribar.me>
	
	I2C bus address search and BlinkM color controlling.
*/

#include <stdio.h>
#include <stdlib.h>
#include "littleWire.h"
#include "littleWire_util.h"

unsigned char i=0;
unsigned char rc;
unsigned char version;
unsigned char myBuffer[8];

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
	if(version==0x10)
	{
		printf("> This example requires the new 1.1 version firmware. Please update soon.\n");
		return 0;
	}
	
	i2c_init(lw);
	i2c_updateDelay(lw,10);

	printf("> Address search ... \n");
	
	for(i=0;i<128;i++)
	{
		rc = i2c_start(lw,i,WRITE);	
		if(rc==1)
			printf("> Found device at %3d\n",i);
	}
	
	delay(2000);
	
	for(;;)
	{ 			
		/* Fade to a HSV color */
		i2c_start(lw,0x09,WRITE);						
			myBuffer[0] = 'h';
			myBuffer[1] = i++;
			myBuffer[2] = 255;
			myBuffer[3] = 255;		
		i2c_write(lw,myBuffer,4,END_WITH_STOP);
		
		delay(10);
		
		/* Get the current RGB color values */				
		i2c_start(lw,0x09,WRITE);		
			myBuffer[0] = 'g'; 
		i2c_write(lw,myBuffer,1,END_WITH_STOP);
		
		delay(10);
		
		/* Actually read the three bytes */
		i2c_start(lw,0x09,READ);
		i2c_read(lw,myBuffer,3,END_WITH_STOP);
		
		delay(10);		
		
		printf("> Read:\t#R: %2X\t#G: %2X\t#B: %2X\t\n",myBuffer[0],myBuffer[1],myBuffer[2]);				
		
		if(lwStatus<0)
		{
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error!\n");
			return 0;
		}
	}
	
	return 0;
}
