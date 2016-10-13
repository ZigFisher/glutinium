/*
	Updated for the new firmware: July 2012
	by ihsan Kehribar <ihsan@kehribar.me>

	Created: December 2011
	by ihsan Kehribar <ihsan@kehribar.me>
	
	Tested with LTC1448:
		Dual 12-Bit Rail-to-Rail Micropower DAC
	
	Generates ramp signal on both output channels.

*/

#include <stdio.h>
#include <stdlib.h>
#include "littleWire.h"
#include "littleWire_util.h"

unsigned int chA=0;
unsigned int chB=0;
unsigned char version;
unsigned char sendBuffer[4];
unsigned char receiveBuffer[4];	

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
	
	pinMode(lw,PIN3,OUTPUT); // pin3 will be used as our chip select pin
	
	spi_updateDelay(lw,0); // Change this according to your device. If your device doesn't respond, try to increase the delay
	
	for(;;)
	{
		if(chA<4096) chA+=1;
		else chA=0;
		if(chB<4096) chB+=1;
		else chB=0;
		
		sendBuffer[0]=(chA>>4);
		sendBuffer[1]=((chA&0x0F)<<4)+((chB&0xF00)>>8);
		sendBuffer[2]=(chB&0xFF);	
	
		spi_sendMessage(lw,sendBuffer,receiveBuffer,3,AUTO_CS); // Send 3 consequent messages with automatic chip select mode		
		
		if(lwStatus<0)
		{
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error!\n");
			return 0;
		}
	
	}
	
}