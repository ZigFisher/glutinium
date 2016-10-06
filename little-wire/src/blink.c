/*
	Updated for the multiple device support: June 2013
	by ihsan Kehribar <ihsan@kehribar.me>

	Updated for the new firmware: July 2012
	by ihsan Kehribar <ihsan@kehribar.me>

	Created: December 2011
	by Omer Kilic <omerkilic@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include "littleWire.h"
#include "littleWire_util.h"

#define LED		PIN2		// LED is connected to the pin2
#define DELAY	500 		// Delay, in miliseconds

unsigned char version;
int total_lwCount;
int i;

int main(void)
{
	littleWire *lw = NULL;
	
	total_lwCount = littlewire_search();

	if(total_lwCount == 1)
	{
		printf("----------------------------------------------------------\n");
		printf("> 1 Little Wire device is found with serialNumber: %d\n",lwResults[0].serialNumber);
		printf("----------------------------------------------------------\n");
	}
	else if(total_lwCount > 1)
	{
		printf("----------------------------------------------------------\n");
		printf("> %d Little Wire devices are found\n",total_lwCount);
		printf("----------------------------------------------------------\n");
		printf("> #id - serialNumber\n");
		for(i=0;i<total_lwCount;i++)
		{
			printf(">  %2d - %3d\n",i,lwResults[i].serialNumber);
		}
		printf("----------------------------------------------------------\n");
	}
	else if(total_lwCount == 0)
	{
		printf("----------------------------------------------------------\n");
		printf("> Little Wire could not be found!\n");
		printf("----------------------------------------------------------\n");
		exit(EXIT_FAILURE);
	}	

	/* Connects to the first littleWire device the computer can find. */
	// lw = littleWire_connect();

	/* Connects to the spesific littleWire device by array id. */
	lw = littlewire_connect_byID(0);

	/* Connects to the spesific littleWire with a given serial number. */
	/* If multiple devices have the same serial number, it connects to the last one it finds */
	// lw = littlewire_connect_bySerialNum(126);

	if(lw == NULL){
		printf("> Little Wire connection problem!\n");
		exit(EXIT_FAILURE);
	}
	
	version = readFirmwareVersion(lw);
	printf("> Little Wire firmware version: %d.%d\n",((version & 0xF0)>>4),(version&0x0F));	
	if(version < 0x12)
	{
		printf("> This example requires the new 1.2 version firmware. Please update soon.\n");
		return 0;
	}	

	/* In order to change the serial number of the current connected device, use the following function. */
	/* You need to unplug-plug to see the change. */
	// changeSerialNumber(lw,752);

	/*------------------------------------------------------------------------------------------------------*/

	pinMode(lw, LED, OUTPUT);

	/* Main loop! */
	for(;;)
	{
		printf("Blink!\n");
		digitalWrite(lw, LED, HIGH);
		delay(DELAY);
		printf("...\n");
		digitalWrite(lw, LED, LOW);
		delay(DELAY);
	
		if(lwStatus<0)
		{
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error: ");
			printf("%s\n",littleWire_errorName());
			return 0;
		}
	}
	
	return 0;
}
