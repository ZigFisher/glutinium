#ifndef __WIRING_H__
#define __WIRING_H__


#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


/* Wiring functions for bootstraping SPI */
void wiring_init();

/* Full-duplex read and write function */
uint8_t wiring_write_then_read(uint8_t* out, 
	                           uint16_t out_len, 
	                   		   uint8_t* in, 
	                   		   uint16_t in_len);

/* Function for setting gpio values */
void wiring_set_gpio_value( uint8_t state);

#endif
