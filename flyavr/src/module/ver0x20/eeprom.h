#include <avr/eeprom.h>


uint32_t reboot_counter EEMEM = 0; // Reboot counter lo
uint32_t wdt_reboot_counter = 0;
uint16_t flags EEMEM = 0;
//uint16_t reboot_counter_hi_ee EEMEM = 0; // Reboot counter hi
uint16_t serial_no_ee EEMEM = 0;  
uint8_t buf_ee[64] EEMEM = 	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
														
