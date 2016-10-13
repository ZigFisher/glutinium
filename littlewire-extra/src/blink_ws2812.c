/*
	ws2812 RGB-Leds blink example - cycles a single led through an RGB sequence
	
	T. Böscke May 26th, 2012
	Adapted from blink example.
*/

#include <stdio.h>
#include <stdlib.h>
#include "littleWire.h"
#include "littleWire_util.h"

#define LED		PIN1		// LED is connected to the pin1
#define DELAY	500 		// Delay, in miliseconds

unsigned char version;

int main(void)
{
	littleWire *lw = NULL;

	lw = littleWire_connect();

	if(lw == NULL){
		printf("> Little Wire could not be found!\n");
		exit(EXIT_FAILURE);
	}
	
	version = readFirmwareVersion(lw);
	printf("> Little Wire firmware version: %d.%d\n",((version & 0xF0)>>4),(version&0x0F));	
	if(version<0x12)
	{
		printf("> This example requires the new 1.2 version firmware. Please update soon.\n");
		return 0;
	}	
	
	pinMode(lw, LED, OUTPUT);

	for(;;){
		printf("Red\n");
		ws2812_write(lw, LED, 255,0,0);
		delay(DELAY);
		
		printf("Green\n");
		ws2812_write(lw, LED, 0,255,0);
		delay(DELAY);
		
		printf("Blue\n");
		ws2812_write(lw, LED, 0,0,255);
		delay(DELAY);
		
		printf("Off..\n");
		ws2812_write(lw, LED, 0,0,0);
		delay(DELAY);
	
		if(lwStatus<0)
		{
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error!\n");
			return 0;
		}
	}
	
}