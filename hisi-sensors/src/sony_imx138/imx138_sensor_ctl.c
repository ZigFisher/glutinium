/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : sony138_sensor_ctl.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/09/09
  Description   : Sony IMX138 sensor driver
  History       :
  1.Date        : 2011/09/09
    Author      : MPP
    Modification: Created file

******************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "hi_ssp.h"

int sony_sensor_write_packet(unsigned int data)
{
	int fd = -1;
	int ret;
	unsigned int value;

	fd = open("/dev/ssp", 0);
	if(fd < 0)
	{
		printf("Open /dev/ssp error!\n");
		return -1;
	}

	value = data;

	ret = ioctl(fd, SSP_WRITE_ALT, &value);

	close(fd);
	return 0;
}

int sony_sensor_read_packet(unsigned int data)
{
	unsigned int value;
	int fd = -1;
	int ret;

	fd = open("/dev/ssp", 0);
	if(fd < 0)
	{
		printf("Open /dev/ssp error!\n");
		return -1;
	}

	value = data;

	ret = ioctl(fd, SSP_READ_ALT, &value);

	close(fd);
	return (value&0xff);
}

int sensor_write_register(int addr, int data)
{
	unsigned int value = (unsigned int)(((addr&0xffff)<<8) | (data & 0xff));
    return sony_sensor_write_packet(value);
}

int sensor_read_register(int addr)
{
	unsigned int data = (unsigned int)(((addr&0xffff)<<8));
    return sony_sensor_read_packet(data);
}

void sensor_prog(int* rom) 
{
}

void setup_sensor(int isp_mode)
{
}

void sensor_init()
{
	sensor_write_register(0x200, 0x01); //Standby
	usleep(200000);

    // chip_id = 0x2
	sensor_write_register(0x205, 0x01); //12bit
	sensor_write_register(0x206, 0x00); //Drive mode:All-pix scan mode(720p mode)
	sensor_write_register(0x207, 0x10); //Window mode:720p mode
	sensor_write_register(0x209, 0x02); //30fps mode
	sensor_write_register(0x20A, 0xF0); //black level

    /*linear & WDR mode is different*/
    sensor_write_register(0x20C, 0x00);
    sensor_write_register(0x20F, 0x01);
    sensor_write_register(0x210, 0x39);
    sensor_write_register(0x212, 0x50);

	sensor_write_register(0x217, 0x01);
	sensor_write_register(0x218, 0xF0);
	sensor_write_register(0x219, 0x02);
	sensor_write_register(0x21A, 0x00);
	
	sensor_write_register(0x21B, 0xC0);//different from imx104
	sensor_write_register(0x21C, 0x19);
	
	sensor_write_register(0x21D, 0xFF);
	sensor_write_register(0x21E, 0x01);
	sensor_write_register(0x236, 0x14); //VB size
	
	sensor_write_register(0x238, 0x00); //cropping postion(Vertical position)
	sensor_write_register(0x239, 0x00);
	
	sensor_write_register(0x23A, 0x19); //cropping size(Vertical direction)
	sensor_write_register(0x23B, 0x04);
	
	sensor_write_register(0x23C, 0x00); //cropping postion(horizontal position)
	sensor_write_register(0x23D, 0x00);
	sensor_write_register(0x23E, 0x1C); //cropping size(horizontal direction)
	sensor_write_register(0x23F, 0x05);
	sensor_write_register(0x244, 0x01); //Parallel CMOS SDR output
	sensor_write_register(0x254, 0x63);
	sensor_write_register(0x25B, 0x00); //CLK 37.125MHz
	sensor_write_register(0x25D, 0x00); //CLK 37.125MHz
	sensor_write_register(0x25F, 0x10); //invalid
	sensor_write_register(0x2BF, 0x1F);

    /*linear & WDR mode is different*/
    sensor_write_register(0x265, 0x20);
    sensor_write_register(0x286, 0x01);
    sensor_write_register(0x2CF, 0xD1);
    sensor_write_register(0x2D0, 0x1B);
    sensor_write_register(0x2D2, 0x5F);
    sensor_write_register(0x2D3, 0x00);

	// chip_id = 0x3
	sensor_write_register(0x312, 0x00);
	sensor_write_register(0x31D, 0x07);
	sensor_write_register(0x323, 0x07);
	sensor_write_register(0x326, 0xDF);
	sensor_write_register(0x347, 0x87);

	// chip_id = 0x4
	sensor_write_register(0x403, 0xCD);
	sensor_write_register(0x407, 0x4B);
	sensor_write_register(0x409, 0xE9);
	sensor_write_register(0x413, 0x1B);
	sensor_write_register(0x415, 0xED);
	sensor_write_register(0x416, 0x01);
	sensor_write_register(0x418, 0x09);
	sensor_write_register(0x41A, 0x19);
	sensor_write_register(0x41B, 0xA1);
	sensor_write_register(0x41C, 0x11);
	sensor_write_register(0x427, 0x00);
	sensor_write_register(0x428, 0x05);
	sensor_write_register(0x429, 0xEC);
	sensor_write_register(0x42A, 0x40);
	sensor_write_register(0x42B, 0x11);
	sensor_write_register(0x42D, 0x22);
	sensor_write_register(0x42E, 0x00);
	sensor_write_register(0x42F, 0x05);
	sensor_write_register(0x431, 0xEC);
	sensor_write_register(0x432, 0x40);
	sensor_write_register(0x433, 0x11);
	sensor_write_register(0x435, 0x23);
	sensor_write_register(0x436, 0xB0);
	sensor_write_register(0x437, 0x04);
	sensor_write_register(0x439, 0x24);
	sensor_write_register(0x43A, 0x30);
	sensor_write_register(0x43B, 0x04);
	sensor_write_register(0x43C, 0xED);
	sensor_write_register(0x43D, 0xC0);
	sensor_write_register(0x43E, 0x10);
	sensor_write_register(0x440, 0x44);
	sensor_write_register(0x441, 0xA0);
	sensor_write_register(0x442, 0x04);
	sensor_write_register(0x443, 0x0D);
	sensor_write_register(0x444, 0x31);
	sensor_write_register(0x445, 0x11);
	sensor_write_register(0x447, 0xEC);
	sensor_write_register(0x448, 0xD0);
	sensor_write_register(0x449, 0x1D);
	sensor_write_register(0x455, 0x03);
	sensor_write_register(0x456, 0x54);
	sensor_write_register(0x457, 0x60);
	sensor_write_register(0x458, 0x1F);
	sensor_write_register(0x45A, 0xA9);
	sensor_write_register(0x45B, 0x50);
	sensor_write_register(0x45C, 0x0A);
	sensor_write_register(0x45D, 0x25);
	sensor_write_register(0x45E, 0x11);
	sensor_write_register(0x45F, 0x12);
	sensor_write_register(0x461, 0x9B);
	sensor_write_register(0x466, 0xD0);
	sensor_write_register(0x467, 0x08);
	sensor_write_register(0x46A, 0x20);
	sensor_write_register(0x46B, 0x0A);
	sensor_write_register(0x46E, 0x20);
	sensor_write_register(0x46F, 0x0A);
	sensor_write_register(0x472, 0x20);
	sensor_write_register(0x473, 0x0A);
	sensor_write_register(0x475, 0xEC);
	sensor_write_register(0x47D, 0xA5);
	sensor_write_register(0x47E, 0x20);
	sensor_write_register(0x47F, 0x0A);
	sensor_write_register(0x481, 0xEF);
	sensor_write_register(0x482, 0xC0);
	sensor_write_register(0x483, 0x0E);
	sensor_write_register(0x485, 0xF6);
	sensor_write_register(0x48A, 0x60);
	sensor_write_register(0x48B, 0x1F);
	sensor_write_register(0x48D, 0xBB);
	sensor_write_register(0x48E, 0x90);
	sensor_write_register(0x48F, 0x0D);
	sensor_write_register(0x490, 0x39);
	sensor_write_register(0x491, 0xC1);
	sensor_write_register(0x492, 0x1D);
	sensor_write_register(0x494, 0xC9);
	sensor_write_register(0x495, 0x70);
	sensor_write_register(0x496, 0x0E);
	sensor_write_register(0x497, 0x47);
	sensor_write_register(0x498, 0xA1);
	sensor_write_register(0x499, 0x1E);
	sensor_write_register(0x49B, 0xC5);
	sensor_write_register(0x49C, 0xB0);
	sensor_write_register(0x49D, 0x0E);
	sensor_write_register(0x49E, 0x43);
	sensor_write_register(0x49F, 0xE1);
	sensor_write_register(0x4A0, 0x1E);
	sensor_write_register(0x4A2, 0xBB);
	sensor_write_register(0x4A3, 0x10);
	sensor_write_register(0x4A4, 0x0C);
	sensor_write_register(0x4A6, 0xB3);
	sensor_write_register(0x4A7, 0x30);
	sensor_write_register(0x4A8, 0x0A);
	sensor_write_register(0x4A9, 0x29);
	sensor_write_register(0x4AA, 0x91);
	sensor_write_register(0x4AB, 0x11);
	sensor_write_register(0x4AD, 0xB4);
	sensor_write_register(0x4AE, 0x40);
	sensor_write_register(0x4AF, 0x0A);
	sensor_write_register(0x4B0, 0x2A);
	sensor_write_register(0x4B1, 0xA1);
	sensor_write_register(0x4B2, 0x11);
	sensor_write_register(0x4B4, 0xAB);
	sensor_write_register(0x4B5, 0xB0);
	sensor_write_register(0x4B6, 0x0B);
	sensor_write_register(0x4B7, 0x21);
	sensor_write_register(0x4B8, 0x11);
	sensor_write_register(0x4B9, 0x13);
	sensor_write_register(0x4BB, 0xAC);
	sensor_write_register(0x4BC, 0xC0);
	sensor_write_register(0x4BD, 0x0B);
	sensor_write_register(0x4BE, 0x22);
	sensor_write_register(0x4BF, 0x21);
	sensor_write_register(0x4C0, 0x13);
	sensor_write_register(0x4C2, 0xAD);
	sensor_write_register(0x4C3, 0x10);
	sensor_write_register(0x4C4, 0x0B);
	sensor_write_register(0x4C5, 0x23);
	sensor_write_register(0x4C6, 0x71);
	sensor_write_register(0x4C7, 0x12);
	sensor_write_register(0x4C9, 0xB5);
	sensor_write_register(0x4CA, 0x90);
	sensor_write_register(0x4CB, 0x0B);
	sensor_write_register(0x4CC, 0x2B);
	sensor_write_register(0x4CD, 0xF1);
	sensor_write_register(0x4CE, 0x12);
	sensor_write_register(0x4D0, 0xBB);
	sensor_write_register(0x4D1, 0x10);
	sensor_write_register(0x4D2, 0x0C);
	sensor_write_register(0x4D4, 0xE7);
	sensor_write_register(0x4D5, 0x90);
	sensor_write_register(0x4D6, 0x0E);
	sensor_write_register(0x4D8, 0x45);
	sensor_write_register(0x4D9, 0x11);
	sensor_write_register(0x4DA, 0x1F);
	sensor_write_register(0x4EB, 0xA4);
	sensor_write_register(0x4EC, 0x60);
	sensor_write_register(0x4ED, 0x1F);

    /*linear & WDR mode is different*/
    sensor_write_register(0x461, 0x9B);
    sensor_write_register(0x466, 0xD0);
    sensor_write_register(0x467, 0x08);

	usleep(200000);
	sensor_write_register(0x200, 0x00); //release standy
	usleep(200000);
	sensor_write_register(0x202, 0x00); //Master mose start
	usleep(200000);
	sensor_write_register(0x249, 0x0A); //XVS & XHS output
	usleep(200000);

	printf("-------Sony IMX138 Sensor Initial OK!-------\n");
}

