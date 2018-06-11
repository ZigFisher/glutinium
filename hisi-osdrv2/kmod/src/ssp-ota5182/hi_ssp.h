/*
 * extdrv/include/hi_ssp.h for Linux .
 *
 * History:
 *      2006-4-11 create this file
 */

#ifndef __HI_SSP_H__
#define __HI_SSP_H__

#define SSP_LCD_READ_ALT	0x1
#define SSP_LCD_WRITE_CMD	0X3
#define SSP_LCD_WRITE_DAT	0X5
#define SSP_LCD_WRITE_CMD16 0X7

int hi_ssp_set_frameform(unsigned char framemode, unsigned char spo, unsigned char sph, unsigned char datawidth);
int hi_ssp_readdata(void);
void hi_ssp_writedata(unsigned short data);

void hi_ssp_enable(void);
void hi_ssp_disable(void);

int hi_ssp_set_serialclock(unsigned char, unsigned char);

void spi_write_a9byte(unsigned char cmd_dat, unsigned char dat);

#endif

