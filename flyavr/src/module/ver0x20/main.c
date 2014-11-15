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
unsigned long i2c_counter;
unsigned long run_counter;


// TWI Slave defines

#define TWI_SLA 0x10  // Slave Address

void InitPins()
{
  direct(PinData,O); //as output
  direct(PinState,O); //as output
  direct(PinBlink,O); //as output
  direct(PinBuzzer,O); //as output
  
  on(PinBlink);
  off(PinData);
  off(PinState);
  off(PinBuzzer);
}


#if 0
u16 blink_state=0;
void blinking(void)
{
	blink_state++;
	if((blink_state >> 7)%2) {
		off(PinState);
	    //off(PinBuzzer);
	} else {
		on(PinState);
		 //on(PinBuzzer);
	}


#if 0
	if((blink_state/5000)%2)
		off(PinState);
	else
		on(PinState);
#endif
}
#endif

int main(void)
{
	unsigned char cmd;
	u08 buf[4];
	u16 crc16_xmodem, crc16_ccitt;
	uint32_t reboots;
	run_counter = 0;
	i2c_counter = 0;



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
			case CMD_DATA_ON:
				on(PinData);
				break;
			case CMD_DATA_OFF:
				off(PinData);
				break;

			case CMD_GET_COUNTER:
				//flushTwiTxBuffer();
				usiTwiTransmitDWord( run_counter );
				break;

			case CMD_GET_CRC1:
				buf[0] = usiTwiReceiveByte( );
				buf[1] = usiTwiReceiveByte( );
				buf[2] = usiTwiReceiveByte( );
				buf[3] = usiTwiReceiveByte( );

				flushTwiTxBuffer();

				crc16_xmodem = _crc_xmodem_update(0, buf[0]);
				crc16_xmodem = _crc_xmodem_update(crc16_xmodem, buf[1]);
				crc16_ccitt  = _crc_ccitt_update(crc16_xmodem, buf[2]);
				crc16_ccitt  = _crc_ccitt_update(crc16_xmodem, buf[3]);
				usiTwiTransmitByte( (crc16_xmodem >> 0) & 0xff );
				usiTwiTransmitByte( (crc16_ccitt >> 0) & 0xff );
				usiTwiTransmitByte( (crc16_xmodem >> 8) & 0xff );
				usiTwiTransmitByte( (crc16_ccitt >> 8) & 0xff );
				break;

			case CMD_GET_REBOOTS:
				//flushTwiTxBuffer();
				reboots = GetReboots();
				usiTwiTransmitDWord( reboots );
				reboots = GetWdtReboots();
				usiTwiTransmitDWord( reboots );
				break;

			case CMD_GET_EEPROM:
				//flushTwiTxBuffer();
				buf[0] = usiTwiReceiveByte( ); // OFFSET
				usiTwiTransmitByte( read_eeprom_buf(buf[0]) );
				break;

			case CMD_SET_EEPROM:
				//flushTwiTxBuffer();
				buf[0] = usiTwiReceiveByte( ); // OFFSET
				buf[1] = usiTwiReceiveByte( ); // VALUE
				write_eeprom_buf(buf[0], buf[1]);
				//usiTwiTransmitByte( read_eeprom_buf(buf[0]) );
				break;

			}
			i2c_counter=0;
	    }
		i2c_counter++;
		run_counter++;
#if 0
		if ( i2c_counter > (u32)((u32)12*(u32)1024*(u32)1024) ) {
			//on(PinBuzzer);
			delay();
			off(PinBuzzer);
			i2c_counter = 0;
		}
#endif

#if 0
		if ( run_counter > (u32)((UINT16_MAX*60)) ) {
			on(PinBuzzer);
			delay();
			Reboot();
		}
#endif

    // Do something else while waiting for the TWI transceiver to complete.
    // Put own code here.

  }

  return 0;
}
