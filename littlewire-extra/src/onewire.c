/*
	Created: July 2012
	by ihsan Kehribar <ihsan@kehribar.me>
	
	Example program to search onewire bus and read temperature from
	DS1820 type digital sensors. Temperature reading in deg. Centigrate - native to the sensor.
	
	How to connect to LittleWire: 
		- Connect GND pin on sensor (left pin) to GND on Little-Wire
		- Connect DQ pin on sensor to PIN2 on Little-Wire
		- Connect Vdd pin on sensor (right) to +5V on Little-Wire
		- Connect R4k7 (4.7k = 4700 Ohm resistor) between DQ pin and Vdd pin

	Data pin: PIN2
*/

#include <stdio.h>
#include <stdlib.h>
#include "littleWire.h"
#include "littleWire_util.h"

littleWire *lw = NULL;

unsigned char version;
unsigned char rxx[8];
int rc=0;
int i=0;
int t=0;

int main(void)
{
	unsigned char temphigh;
	unsigned char templow;
	unsigned int tempdecimal;
	unsigned char scratch;

	lw = littleWire_connect();

	if(lw == NULL){
		printf("Little Wire could not be found!\n");
		exit(EXIT_FAILURE);
	}
	
	version = readFirmwareVersion(lw);
	printf("> Little Wire found with firmware version: %d.%d\n",((version & 0xF0)>>4),(version&0x0F));
	if(version==0x10)
	{
		printf("This example requires the new 1.1 version of Little-Wire firmware. Please update soon.\n");
		return 0;
	}
	
	printf("> Address search ... \n");
	
	if(onewire_firstAddress(lw))
	{
		printf("> Addr#%d:  %X\t",i,ROM_NO[0]);
		for(t=0;t<7;t++)
			printf("%X\t",ROM_NO[t+1]);			
		printf("\n");
		i=1;
	}

	while(onewire_nextAddress(lw))
	{
		printf("> Addr#%d:  %X\t",i,ROM_NO[0]);
		for(t=0;t<7;t++)
			printf("%X\t",ROM_NO[t+1]);			
		printf("\n");
		i++;
	}

	if(i>0)
		printf("> End of search with %d onewire device(s) found\r\n",i);	
	else
	{
		printf("> No onewire device has been found!\n");
		return 0;
	}
	
	printf("> Start the talking with the last onewire device found\n");
	
	delay(2000);
	
	for(;;)
	{			
		// send reset signal and read the presence value
		if(!onewire_resetPulse(lw)) // if this returns zero, sensor is unplugged		
		{
			printf("> Sensor is unplugged!\n");
			return 0;
		}
		
		// onewire_writeByte(0xCC); /* SKIP ROM command */
		onewire_writeByte(lw,0x55); /* MATCH ROM command */		
		
		onewire_writeByte(lw,ROM_NO[0]); /* Address begins */
		onewire_writeByte(lw,ROM_NO[1]);
		onewire_writeByte(lw,ROM_NO[2]);
		onewire_writeByte(lw,ROM_NO[3]);
		onewire_writeByte(lw,ROM_NO[4]);
		onewire_writeByte(lw,ROM_NO[5]);
		onewire_writeByte(lw,ROM_NO[6]);
		onewire_writeByte(lw,ROM_NO[7]); /* Address ends */	
		
		onewire_writeByte(lw,0x44); /* CONVERT T command */

		// wait for conversion - this is important, if you allways get 85 deg. C, increase tis value 
		delay(650);
		
		// send reset signal and read the presence value
		if(!onewire_resetPulse(lw)) // if this returns zero, sensor is unplugged		
		{
			printf("> Sensor is unplugged!\n");
			return 0;
		}
			
		// onewire_writeByte(0xCC); /* SKIP ROM command */
		onewire_writeByte(lw,0x55); /* MATCH ROM command */		
		
		onewire_writeByte(lw,ROM_NO[0]); /* Address begins */
		onewire_writeByte(lw,ROM_NO[1]);
		onewire_writeByte(lw,ROM_NO[2]);
		onewire_writeByte(lw,ROM_NO[3]);
		onewire_writeByte(lw,ROM_NO[4]);
		onewire_writeByte(lw,ROM_NO[5]);
		onewire_writeByte(lw,ROM_NO[6]);
		onewire_writeByte(lw,ROM_NO[7]); /* Address ends */	
		
		onewire_writeByte(lw,0xBE); /* Read register (scratchpad) command */
		
		printf("> SCRATCH:");
		
		for(i=0;i<9;i++) //read 9 bytes from SCRATCHPAD
		{	
                        scratch = onewire_readByte(lw);
                        printf(":%2X",scratch);
                        if (i == 0) // LSB temperature register byte (0)
                                templow = scratch;
                        if (i == 1) // MSB temperature register byte (1)
                                temphigh = scratch;
		}

		char ch; 
		ch = templow;
		printf("\nLSB %c%c%c%c%c%c%c%c",
			(ch&0x80)?'1':'0',
			(ch&0x40)?'1':'0',
			(ch&0x20)?'1':'0',
			(ch&0x10)?'1':'0',
			(ch&0x08)?'1':'0',
			(ch&0x04)?'1':'0',
			(ch&0x02)?'1':'0',
			(ch&0x01)?'1':'0'
		);	

		 ch = temphigh;
			printf("\nMSB %c%c%c%c%c%c%c%c\n",
			(ch&0x80)?'1':'0',
			(ch&0x40)?'1':'0',
			(ch&0x20)?'1':'0',
			(ch&0x10)?'1':'0',
			(ch&0x08)?'1':'0',
			(ch&0x04)?'1':'0',
			(ch&0x02)?'1':'0',
			(ch&0x01)?'1':'0'
		);

		tempdecimal = 0;

		switch (templow & 0x01) {
			case 0x01:
				tempdecimal = 625;
				break;
		}

	       switch (templow & 0x02) {
			case 0x02:
                        	tempdecimal += 1250;
                        	break;
		}
        	switch (templow & 0x04) {
                	case 0x04:
                        	tempdecimal += 2500;
                        break;
                }
        	switch (templow & 0x08) {
				case 0x08:
                        	tempdecimal += 5000;
                        break;
                }

		tempdecimal /= 10;

		//decide whether this is positive, or negative temperature value and either print minus sign, or not
                printf("> TEMP %c%d.%d deg. C\n", ((temphigh & 0x80) == 0x80) ? '-' : ' ', ((templow & 0xf0) >> 4) | ((temphigh & 0x07) << 4), tempdecimal);
		
		if(lwStatus<0)
		{
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error!\n");
			return 0;
		}
		
	}
	
}
