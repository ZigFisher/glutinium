/*
	Created July 2012
	by ihsan Kehribar <ihsan@kehribar.me>
*/

#include <stdio.h>
#include <stdlib.h>
#include "littleWire.h"
#include "littleWire_util.h"

unsigned char version;
unsigned int adcValue;

int main(int argc, char **argv)
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
		
	char input;

	while(1)
	{
		input=debugSpi(lw,0x00);
		if(input!=0)
			printf("%c",input);
			
		if(lwStatus<0)
		{
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error!\n");
			return 0;
		}
	}
}
