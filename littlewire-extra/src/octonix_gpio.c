/*
	Experimental
*/

#include <stdio.h>
#include <stdlib.h>
#include "littleWire.h"
#include "littleWire_util.h"

#define LED	PIN1		// LED is connected to the pinX
#define DELAY	5000		// Delay, in miliseconds

unsigned char version;
int total_lwCount;
int i;

int main(void)
{
	littleWire *lw = NULL;

	lw = littlewire_connect_byID(0);

	if(lw == NULL){
		printf("> Little Wire connection problem!\n");
		exit(EXIT_FAILURE);
	}

	printf("v.09\n");
	pinMode(lw, LED, OUTPUT);
	digitalWrite(lw, LED, HIGH);
	// digitalWrite(lw, LED, LOW);
	delay(DELAY);
        return 0;
}
