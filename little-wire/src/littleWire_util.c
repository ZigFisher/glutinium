
#include "littleWire_util.h"

/* Delay in miliseconds */
void delay(unsigned int duration)
{
	#ifdef LINUX
		// use standard unix api with microseconds
		usleep(duration*1000);
	#else
		// Sleep(duration);
		Sleep(duration * 2);
	#endif
}
