#include <avr/eeprom.h>


uint16_t reboot_counter_lo_ee EEMEM = 0; // Reboot counter lo
uint16_t reboot_counter_hi_ee EEMEM = 0; // Reboot counter hi
uint16_t serial_no_ee EEMEM = 0;  
