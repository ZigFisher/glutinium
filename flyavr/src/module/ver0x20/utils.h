#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED


void UpdateEEPROM();
uint32_t GetReboots();
uint32_t GetWdtReboots();
uint8_t read_eeprom_buf(uint8_t offset);
void write_eeprom_buf(uint8_t offset, uint8_t value);
void Reboot(void);
void delay(void);

#endif
