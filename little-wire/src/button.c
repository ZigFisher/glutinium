/*
	Updated for the new firmware: July 2012
	by ihsan Kehribar <ihsan@kehribar.me>
	
	Created: December 2011
	by Omer Kilic <omerkilic@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include "littleWire.h"
#include "littleWire_util.h"

#define BUTTON		PIN3	// Pin button is connected to pin3 (active low)
#define DEBOUNCE	50		// Debounce delay, in miliseconds

unsigned char version;

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
	
	pinMode(lw, BUTTON, INPUT);
	internalPullup(lw, BUTTON, ENABLE);	

	for(;;){
		if ( digitalRead(lw, BUTTON) == LOW ){
			delay(DEBOUNCE);
			if( digitalRead(lw, BUTTON) == LOW ){
				printf("> Button pressed.\n");
			}
		}		
		if(lwStatus < 0){
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error!\n");
			return 0;
		}
	}
}
