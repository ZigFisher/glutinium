/*---------------------------- pelco.h---------------------------------------
*	Serial control port for 1650A.
*	01/18/97	started	Joe Briggs
*-------------------------------------------------------------------------*/
#ifndef PELCO_H
	#define PELCO_H
#include <sys/signal.h>
#include <sys/types.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

typedef int error_code;
typedef unsigned int u_int32;

typedef enum {p_sync,p_addr,p_cmd1,p_cmd2,p_data1,p_data2,p_cksum}pelco_t;
/*
*	pelco D protocol is a 7-byte packet with (starting with byte-0):
*	sync,address,cmd1,cmd2,data1,data2, and checksum.
*	sync is always 0xff, and checksum is modulo 8 sum of bytes 0-5.
*/
#define PELCO_PACKET	7 // seven bytes - go figure?
#define C1_SENSE 	(1<<7)
#define C1_SCAN		(1<<4)
#define C1_ONOFF	(1<<3)
#define C1_IRIS_CLOSE	(1<<2)
#define C1_IRIS_OPEN	(1<<1)
void spc_delete();
/*
*	for baud_rate, use 1200, 2400, 19200, 115200 
*/
/*-----------------------------------------------------------------*/
#endif /* pelco_h */
