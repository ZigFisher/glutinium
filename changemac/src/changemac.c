/*
* atheros eeprom editor by (md5sum :)
* c5c111cd582e10cbcdacc0a32e5886c5 and b17264918531cf1f512f7098d474eca5
* some values and HW identify code got from Atheros ath9k Linux driver
* Copyright (c) 2008-2010 Atheros Communications Inc.
* GPLv2 License applied.
*/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    // основные переменные
    unsigned short eeprom_start = 0x1200;		// начало eeprom (ubnt M серия)
    unsigned short crc_offs	= eeprom_start + 0x02;	// контрольная сумма
    unsigned short antenna_offs = eeprom_start + 0x08;	// типы антенн
    unsigned short wmac_offs	= eeprom_start + 0x0c;	// мак адрес wifi
    unsigned short country_offs = eeprom_start + 0x12;	// код страны
    // конфигурация LAN
    unsigned short lan1_mac = 0x00;			// мак1
    unsigned short lan2_mac = 0x06;			// мак2

    // максимальный размер исходного файла (mtd5)
    unsigned int full_size, max_size = 131072;

    unsigned short eeprom_size, crc_read, crc_calc = 0, data = 0;

    FILE* source_file = fopen(argv[1],"r");
    if (source_file == 0) {
	perror("Cannot open file for read");
	return 0;
    }

    // заменить на fstat, нехорошо будет, если входной файл - flash
    fseek(source_file, 0, SEEK_END);
    full_size = ftell(source_file);
    
    if (full_size > max_size) {
	printf("invalid file size %d, should be less than %d", full_size, max_size);
	return 0;
    }

    void *full_eeprom = malloc(full_size);
    memset(full_eeprom, 0, full_size);

    fseek(source_file, 0, SEEK_SET);
    fread(full_eeprom, full_size, 1, source_file);
    fclose(source_file);

    // автодетект размера eeprom
    memcpy(&eeprom_size, full_eeprom + eeprom_start, sizeof(eeprom_size));
    // чтение исходного crc
    memcpy(&crc_read, full_eeprom + crc_offs, sizeof(crc_read));

    printf("old crc = %d, %#x\n", crc_read, crc_read);

    unsigned short mac_test = 0x070d;

    if (!strncmp(argv[3], "crc", 4)) {
	printf ("Nothing to change, just to recalculate CRC.\n");
    }
    if (!strncmp(argv[3], "mac", 4)) {
     if (strlen(argv[4]) == 17) {
        // меняем мак wifi
	void *new_wlan_mac = malloc(6);
        memset(new_wlan_mac, 0, 6);

        printf("using mac: %s\n",argv[4]);
        sscanf(argv[4], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", new_wlan_mac, new_wlan_mac + 1,
              new_wlan_mac + 2, new_wlan_mac + 3, new_wlan_mac + 4, new_wlan_mac + 5);

        memcpy(full_eeprom + wmac_offs, new_wlan_mac, 6);
        free(new_wlan_mac);
     } else {
	printf ("Invalid MAC, must be XX:XX:XX:XX:XX:XX, exiting!\n");
	return 0;
     }
    }
    if (!strncmp(argv[3], "country", 8)) {
        // меняем страну
	void *new_wlan_country = malloc(2);
        memset(new_wlan_country, 0, 2);
        printf("using country: %s\n",argv[4]);
        sscanf(argv[4], "%hhx%hhx", new_wlan_country, new_wlan_country + 1);
        memcpy(full_eeprom + country_offs, new_wlan_country, 2);
        free(new_wlan_country);
    }

    if (!strncmp(argv[3], "ant", 4)) {
        // меняем страну
	void *new_wlan_ant = malloc(2);
        memset(new_wlan_ant, 0, 2);
        printf("using antennas: %s\n",argv[4]);
        sscanf(argv[4], "%hhx%hhx", new_wlan_ant, new_wlan_ant + 1);
        memcpy(full_eeprom + antenna_offs, new_wlan_ant, 2);
        free(new_wlan_ant);
    }

    printf("piston");
    int i;
    for (i = 0; i < eeprom_size; i += 2) {
            if (2 == i) continue;
            memcpy(&data, full_eeprom + eeprom_start + i, 2);
            crc_calc ^= data;
            if (!(i & 0xFFC0)) printf(".");
    }

    crc_calc ^= 0xFFFF;
    printf("\n");

    // пишем новую контрольную сумму
    memcpy(full_eeprom + crc_offs, &crc_calc, sizeof(crc_calc));

    printf("new crc = %d, %#x\n", crc_calc, crc_calc);

    FILE* result_file = fopen(argv[2],"w");
    if (result_file == 0) {
	perror("Cannot open file for write");
	return 0;
    }

    fseek(result_file, 0, SEEK_SET);
    fwrite(full_eeprom, full_size, 1, result_file);
    fclose(result_file);

    free(full_eeprom);

    return 0;
}
