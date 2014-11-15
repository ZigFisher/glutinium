#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/crc16.h>
#include "timer0.h"
#include "usiTwiSlave.h"
#include "utils.h"
#include "common.h"


volatile unsigned int timer_val = 0;
volatile unsigned int state = 0;
volatile unsigned long counter = 0;


// TWI Slave defines

#define TWI_SLA 0x10  // Slave Address

void InitPins()
{
  direct(FLY_RESET, O); //as output
  direct(FLY_PIN1,  O); //as output
  direct(FLY_PIN2,  O); //as output
  direct(FLY_PIN3,  O); //as output
  
  off(FLY_RESET);
  off(FLY_PIN1);
  off(FLY_PIN2);
  off(FLY_PIN3);
}


int main(void)
{
	unsigned char cmd;
	u08 buf[4];
	u16 crc16_xmodem, crc16_ccitt;


	UpdateEEPROM();
	InitPins();
	//Timer0_Init();
	//Timer0_RegisterCallbackFunction(blinking);
 
	// LED feedback port - connect port B to the STK500 LEDS
	// DDRB  = 0xFF; // Set to ouput
	// PORTB = 0x55; // Startup pattern

	// DDRA = 0xFF;  // L2 Used to time interrupts.

	// Own TWI slave address
	usiTwiSlaveInit( TWI_SLA );
	//wdt_enable(WDTO_1S);


	sei( );

  	// This loop runs forever.  If the TWI Transceiver is busy the execution
  	// will just continue doing other operations.

  	for( ; ; ) {

	    if( usiTwiDataInReceiveBuffer( ) )
	    {
	        cmd = usiTwiReceiveByte( );
			switch(cmd) {
			case CMD_GET_VERSION:
		        usiTwiTransmitByte( FLYAVR_VERSION );
				break;
			case CMD_GET_HW:
				usiTwiTransmitByte( FLYAVR_HW );
				break;
			case CMD_PIN1_ON:
				on(FLY_PIN1);
				break;
			case CMD_PIN1_OFF:
				off(FLY_PIN1);
				break;
			case CMD_PIN2_ON:
				on(FLY_PIN2);
				break;
			case CMD_PIN2_OFF:
				off(FLY_PIN2);
				break;
			case CMD_PIN3_ON:
				on(FLY_PIN3);
				break;
			case CMD_PIN3_OFF:
				off(FLY_PIN3);
				break;
			case CMD_GET_COUNTER:
				//flushTwiBuffers();
				usiTwiTransmitByte( counter & 0xff );
				usiTwiTransmitByte( (counter >> 8) &0xff );
				usiTwiTransmitByte( (counter >> 16) &0xff );
				usiTwiTransmitByte( (counter >> 24) &0xff );
				break;
			case CMD_GET_CRC1:
				buf[0] = usiTwiReceiveByte( );
				buf[1] = usiTwiReceiveByte( );
				buf[2] = usiTwiReceiveByte( );
				buf[3] = usiTwiReceiveByte( );

				flushTwiTxBuffer();

				crc16_xmodem = _crc_xmodem_update(0,             (  buf[0] ^ 0x55 ) ); // xor, 0x55 = 1010101b
				crc16_xmodem = _crc_xmodem_update(crc16_xmodem,  ( ~buf[1]        ) ); // not
				crc16_ccitt  = _crc_ccitt_update(crc16_xmodem,   ( ~buf[2]        ) ); // not
				crc16_ccitt  = _crc_ccitt_update(crc16_xmodem,   (  buf[3] ^ 0x66 ) ); // xor, 0x66 = 1100110b
				usiTwiTransmitByte( (crc16_xmodem >> 0) & 0xff );
				usiTwiTransmitByte( (crc16_ccitt >> 0) & 0xff );
				usiTwiTransmitByte( (crc16_xmodem >> 8) & 0xff );
				usiTwiTransmitByte( (crc16_ccitt >> 8) & 0xff );
				break;

			default:
	        	usiTwiTransmitByte( cmd*2 );
			}
			counter=0;
	    }
		counter++;
		if ( counter > (u32)((u32)20*(u32)1024*(u32)1024) ) { // 20 = ~5min, 24 = ~6min
			on(FLY_RESET);
			cli();
			for (counter = 0; counter < 100000; counter++) {
				asm ("nop");
				asm ("nop");
				asm ("nop");
			}
			sei();
			off(FLY_RESET);
			counter = 0;
		}

    // Do something else while waiting for the TWI transceiver to complete.
    // Put own code here.

  }

  return 0;
}
