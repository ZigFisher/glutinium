/*
	Updated for the new firmware: July 2012
	by ihsan Kehribar <ihsan@kehribar.me>
	
	-- This is experimental! --
	Created: December 2011
	by Omer Kilic <omerkilic@gmail.com>
*/

#ifndef LINUX
  #warning "This probably won't work outside Linux."
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "littleWire.h"
#include "littleWire_util.h"

#define BUTTON		PIN1		// Pin button is connected to pin1 (active low)
#define DEBOUNCE	100			// Debounce time, in miliseconds

littleWire *lw = NULL;

unsigned char version;

void *buttonHandler(void *arg)
{
	int buttonPin = (int)arg;

	for(;;){
		if ( digitalRead(lw, buttonPin) == LOW ){
			delay(DEBOUNCE);
			if( digitalRead(lw, buttonPin) == LOW ){
				printf("\n> Button pressed.\n");
			}
		}
		delay(1000);
	}
}

int main()
{
	pthread_t buttonThread;

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

	pthread_create(&buttonThread, NULL, buttonHandler, (void*)BUTTON);
	pthread_tryjoin_np(buttonThread, BUTTON);

	for(;;){
		// this is where main() does stuff
	}

}
