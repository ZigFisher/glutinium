/*
	Created: July 2012
	by ihsan Kehribar <ihsan@kehribar.me>
	
	May 2013: Adapted to ws2812 LEDS - T. Böscke
	
	Connect a ws2812 RGB led to PIN1
*/

#include <stdio.h>
#include <stdlib.h>
#include "littleWire.h"
#include "littleWire_util.h"

unsigned char i=0;
int r,g,b;
int h,s,v;
unsigned char version;

// Taken from: http://www.nunosantos.net/archives/114
void HSVtoRGB( int  *r, int *g,int *b, int h, int s, int v );

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
	if(version<0x12)
	{
		printf("> This example requires the new 1.2 version firmware. Please update soon.\n");
		return 0;
	}
	
//	softPWM_state(lw,ENABLE);
	
	h=0;
	s=255;
	v=255;
	
	for(;;)
	{ 						
		if(h==359)
			h=0;
		else
			h+=1;
		
		HSVtoRGB(&r,&g,&b,h,s,v);		
		
		printf("> Current:\t#r: %3d\t#g: %3d\t#b: %3d\t\n",r,g,b);	
	
		ws2812_write(lw,1,r,g,b);

		delay(40);
		
		if(lwStatus<0)
		{
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error!\n");
			return 0;
		}
	}
	
	return 0;
}

void HSVtoRGB( int  *r, int *g,int *b, int h, int s, int v )
{
        int f;
        long p, q, t;
 
        if( s == 0 )
        {
                *r = *g = *b = v;
                return;
        }
 
        f = ((h%60)*255)/60;
        h /= 60;
 
        p = (v * (256 - s))/256;
        q = (v * ( 256 - (s * f)/256 ))/256;
        t = (v * ( 256 - (s * ( 256 - f ))/256))/256;
 
        switch( h ) {
                case 0:
                        *r = v;
                        *g = t;
                        *b = p;
                        break;
                case 1:
                        *r = q;
                        *g = v;
                        *b = p;
                        break;
                case 2:
                        *r = p;
                        *g = v;
                        *b = t;
                        break;
                case 3:
                        *r = p;
                        *g = q;
                        *b = v;
                        break;
                case 4:
                        *r = t;
                        *g = p;
                        *b = v;
                        break;
                default:
                        *r = v;
                        *g = p;
                        *b = q;
                        break;
        }
}
 