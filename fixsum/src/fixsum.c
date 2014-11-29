/*
 * (C) Copyright 2010
 * evaxige <evxige at gmail dot com>
 * Version:	1.0
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>

static uint16_t swab16(uint16_t x)
{
	return ((uint16_t)(
	(((uint16_t)x & (uint16_t)0x00ffU) << 8) | 
	(((uint16_t)x & (uint16_t)0xff00U) >> 8))); 
}

static void parse_mac(char* str, uint8_t* mac)
{
    char* tp = NULL;
    char* buf = str;
    int i = 0;

    while( NULL != (tp = strtok(buf, ":-")) ) {
        if(i < 6 && tp) {
            mac[i] = (uint8_t)(strtol(tp, NULL, 16));
/*            printf("0x%02x\n", mac[i]);*/
            i++;
        }
        buf = NULL;
    }
}

static void printmac(char* str, uint8_t* mac)
{
    int i = 0;
	printf("%s",str);
    while( i<6 ) {
       printf("%02x", mac[i]);
	   if (i<5) printf(":");
       i++;
      }
	printf("\n");
}

static int checkmac(uint8_t* mac)
{
	int i = 0;
	i=0;
	if ((mac[0]==00) && (mac[1]==0x15) && (mac[2]==0x6d)) { i=1; }
	if ((mac[0]==00) && (mac[1]==0x27) && (mac[2]==0x22)) { i=1; }
	if ((mac[0]==0xDC) && (mac[1]==0x9F) && (mac[2]==0xDB)) { i=1; }
	if ((mac[0]==0x24) && (mac[1]==0xA4) && (mac[2]==0x3C)) { i=1; }
	return i;
}


static void modify_mac(uint8_t *data, uint8_t *mac)
{
	uint8_t *eep = data;
	uint16_t eep_16_len = 0;
	uint16_t *eep_16 = NULL;
	int eep_len = 0;
	int i = 0;
	uint16_t k = 0, sum = 0;

	/* get eeprom len */
	eep_len = (eep[0] << 8) + eep[1];
	eep_16_len = eep_len / 2;
	printf("eep_len(0x%04x)\n", eep_len);

	eep_16 = (uint16_t*) eep;
	printf("old checksum(0x%04x)\n", swab16(eep_16[1]));

	/* modify mac */
	for(i = 0; i < 6; i++) {
		eep[0xc + i] = mac[i];
	}

	/* fix checksum */
	for(k = 0; i < 0xffff; k++ ) {
		eep_16[1] = k;

		for(i = 0; i < eep_16_len; i++) {
			sum ^= eep_16[i];
		}
		if(sum == 0xffff) {
			printf("new checksum(0x%04x)\n", swab16(k));
			break;
		}
		sum = 0;
	}
}

static void modify_mac_lan(uint8_t *data, uint8_t *mac)
{
	int i = 0;
	printf("modify mac lan ");
	/* modify mac */
	for(i = 0; i < 6; i++) {
		data[i] = mac[i];
		printf("%02x", mac[i]);
		if (i<5) { printf(":"); }
	}
	printf("\n");

}


static int save_data(uint8_t *data, int len)
{
	FILE* pf = NULL;

	if( NULL == (pf = fopen("EEPROM_NEW.bin", "wb")) ) {
		perror("fopen");
		return -1;
	}

	if(len != fwrite(data, 1, len, pf)) {
		perror("fwrite");
		return -1;
	}

	fclose(pf);

	return 0;
}

void usage()
{
	printf("Fix atheros ath9k chipset EEPROM checksum\n");
	printf("Binary file is wholeflash data or \"EEPROM\" partition\n");
	printf("Usage:\n");
	printf("fixsum filename org_mac new_mac\n");
	printf("Example:\nfixsum /tmp/EEPROM_ORIG.bin 01:02:03:04:05:06 11:22:33:44:55:66\n");
}

int main(int argc, char* argv[])
{
	int j = 0;
	int i = 0;
	int file_len = 0;
	uint8_t *data = NULL;
	uint8_t old_mac[6] = {0}, new_mac[6] = {0};
	uint8_t new_mac2[6] = {0}, new_mac3[6] = {0};
	FILE* pf = NULL;
	struct stat st;

	if(argc != 4) {
		usage();
		exit(-1);
	}

	/* parse arguments */
	parse_mac(argv[2], old_mac);
	printmac("Old ",old_mac);
	parse_mac(argv[3], new_mac);
	printmac("New ",new_mac);
    if (checkmac(new_mac)==0) {
		perror("mac ungenuine");
		exit(-1);
	}
	
	i=0;
    while( i<6 ) {
	   new_mac2[i]=new_mac[i];
	   new_mac3[i]=new_mac[i];
       i++;
      }
	 new_mac2[3]=new_mac[3]+1; 
	 new_mac3[3]=new_mac[3]+1; 
	 new_mac3[0]=new_mac[0]+2; 

	/* read file */
	if(-1 == stat(argv[1], &st)) {
		perror("error");
		exit(-1);
	}
	file_len = st.st_size;

	if( (pf = fopen(argv[1], "rb")) == NULL) {
		perror("fopen");
		exit(-1);
	}

	data = (uint8_t *)malloc(file_len);
	if(data == NULL) {
		perror("out of memory");
		exit(-1);
	}

	if(file_len != fread(data, 1, file_len, pf)) {
		perror("fread");
		exit(-1);
	}
	fclose(pf);

	printf("HWID: %02x%02x\n", data[0x1016],data[0x1017]);

	/* find offset */
	for(i = 0; i < file_len; i++) {
		if( old_mac[0] == data[i + 0] &&
		    old_mac[1] == data[i + 1] &&
			old_mac[2] == data[i + 2] &&
			old_mac[3] == data[i + 3] &&
			old_mac[4] == data[i + 4] &&
			old_mac[5] == data[i + 5] ) {
			printf("find old_mac_wifi offset(0x%x)\n", i);
			/* update new mac */
			if (i>0x8000) {
			  modify_mac_lan(data + i, new_mac);
			}
			else {
			  modify_mac(data + (i - 0xc), new_mac);
			}
			j++;
		}
		if( old_mac[0] == data[i + 0] &&
		    old_mac[1] == data[i + 1] &&
			old_mac[2] == data[i + 2] &&
			(old_mac[3]+0x1) == data[i + 3] &&
			old_mac[4] == data[i + 4] &&
			old_mac[5] == data[i + 5] ) {
			printf("find old_mac_lan offset(0x%x)\n", i);
			/* update new mac lan 2*/
			modify_mac_lan(data + i, new_mac2);
			j++;
		}

		if( (old_mac[0]+2) == data[i + 0] &&
		    old_mac[1] == data[i + 1] &&
			old_mac[2] == data[i + 2] &&
			(old_mac[3]+0x1) == data[i + 3] &&
			old_mac[4] == data[i + 4] &&
			old_mac[5] == data[i + 5] ) {
			printf("find old_mac_lan2 offset(0x%x)\n", i);
			/* update new mac lan 2*/
			modify_mac_lan(data + i, new_mac3);
			j++;
		}
	}
    
	if (j>0) 
	if (0 == save_data(data, file_len)) {
				printf("change %04x mac-s ok!\n",j);
			}

	
	
	free(data);
	
	return 0;
}
