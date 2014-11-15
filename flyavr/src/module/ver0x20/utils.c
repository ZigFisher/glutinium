#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "common.h"
#include "eeprom.h"


uint32_t GetReboots()
{
	return eeprom_read_dword(&reboot_counter);
}


/******************************************************************************/
void inc_reboot(void)
/******************************************************************************/
{
        uint32_t reboots = GetReboots();
        reboots++;
        eeprom_write_dword(&reboot_counter, reboots);
}

void UpdateEEPROM()
{
	inc_reboot();
};

uint32_t GetWdtReboots()
{
	return eeprom_read_dword(&wdt_reboot_counter);
}


uint8_t read_eeprom_buf(uint8_t offset)
{
	return eeprom_read_byte(&buf_ee[offset]);
}

void write_eeprom_buf(uint8_t offset, uint8_t value)
{
	eeprom_write_byte(&buf_ee[offset], value);
}

void Reboot(void)
{
	uint32_t reboots = GetWdtReboots();
	reboots++;
	eeprom_write_dword(&wdt_reboot_counter, reboots);

	cli();
	wdt_reset();
	wdt_enable(1);
	while (1);
}

extern unsigned long run_counter;
void delay()
{
	for (run_counter = 0; run_counter < 100000; run_counter++) {
		asm ("nop");
		asm ("nop");
		asm ("nop");
	}
}
