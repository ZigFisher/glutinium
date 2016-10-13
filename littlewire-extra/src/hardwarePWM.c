/*
	Created: July 2012
	by ihsan Kehribar <ihsan@kehribar.me>	
*/

#include <stdio.h>
#include <stdlib.h>
#include "littleWire.h"
#include "littleWire_util.h"

unsigned char version;
unsigned char val1=0;
unsigned char val2=0;

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
	
	pwm_init(lw);
	
	pwm_updatePrescaler(lw,1);	/* Fastest PWM frequency */

	for(;;)
	{ 
		val1++;
		val2--;
		
		pwm_updateCompare(lw,val1,val2);
		
		delay(50);
		
		if(lwStatus<0)
		{
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error!\n");
			return 0;
		}		
		
	}
	
	return 0;
}
