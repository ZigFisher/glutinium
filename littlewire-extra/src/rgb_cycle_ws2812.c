/*
	ws2812 rgb-string - cycles five leds through a colour sequence using the internal buffer
	
	T. Böscke May 26th, 2012
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
	int step=0;
	
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
		int i=0;
		step++;
		
		for (i=0; i<5; i++)
		{
			switch((i+step)%5) {
				case 0:		ws2812_preload(lw, 255,255,255); break;
				case 1:		ws2812_preload(lw, 255,  0,  0); break;
				case 2:		ws2812_preload(lw, 255,255,  0); break;
				case 3:		ws2812_preload(lw,   0,255,255); break;
				case 4:		ws2812_preload(lw,   0,255,  0); break;				
			}		
		}		
		ws2812_flush(lw, LED);	
		delay(DELAY);
	
		if(lwStatus<0)
		{
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error!\n");
			return 0;
		}
	}
	
}