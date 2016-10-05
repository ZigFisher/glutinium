/*
	Updated for the new firmware: July 2012
	by ihsan Kehribar <ihsan@kehribar.me>
	
	Created: December 2011
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
		
	// -- Select ADC reference voltage
	// Those definetly needs calibration for precision measurement. 
	analog_init(lw,VREF_VCC);
	// analog_init(lw,VREF_1100mV);
	// analog_init(lw,VREF_2560mV;	

	while(1)
	{
		adcValue=analogRead(lw, ADC_PIN2);
	
		printf("> Value: %X\tVoltage: %f milivolts\n", adcValue,(float)((adcValue*(5000))/1024.0));
		// printf("> Value: %X\tVoltage: %f milivolts\n", adcValue,(float)((adcValue*(1100))/1024.0));		
		// printf("> Value: %X\tVoltage: %f milivolts\n", adcValue,(float)((adcValue*(2560))/1024.0));		
		
		if(lwStatus<0)
		{
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error!\n");
			return 0;
		}
		
		delay(100);
	}
}
