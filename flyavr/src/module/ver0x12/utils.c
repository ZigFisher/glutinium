#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "common.h"
#include "eeprom.h"


/******************************************************************************/
void inc_reboot(void)
/******************************************************************************/
{
        uint16_t reboots = eeprom_read_word(&reboot_counter_lo_ee);
        reboots++;
        eeprom_write_word(&reboot_counter_lo_ee, reboots);
}

void UpdateEEPROM()
{
	inc_reboot();
};

uint16_t GetReboots()
{
	return eeprom_read_word(&reboot_counter_lo_ee);
}
