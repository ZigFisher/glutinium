/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : sony036_sensor_ctl.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/03/23
  Description   : Sony IMX036 sensor driver
  History       :
  1.Date        : 2011/03/23
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
	//printf("write data = %#x\n", value);
    return sony_sensor_write_packet(value);
}

int sensor_read_register(int addr)
{
	unsigned int data = (unsigned int)(((addr&0xffff)<<8));
	//printf("read data = %#x\n", data);
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
	// sequence according to "Flow  Power-on to Operation Start(Sensor Master Mode)

	// chip_id = 0x2
	sensor_write_register(0x200, 0x55);
	sensor_write_register(0x201, 0x70);
	sensor_write_register(0x203, 0x08);
	sensor_write_register(0x204, 0x0D);
	sensor_write_register(0x207, 0x98);
	sensor_write_register(0x209, 0x65);
	sensor_write_register(0x20A, 0x04);
	sensor_write_register(0x21F, 0xF0);
	sensor_write_register(0x225, 0x47);
	sensor_write_register(0x25E, 0xFF);
	sensor_write_register(0x25F, 0x02);
	sensor_write_register(0x273, 0xB1);
	sensor_write_register(0x274, 0x01);
	sensor_write_register(0x276, 0x60);
	sensor_write_register(0x29D, 0x00);
	sensor_write_register(0x2D4, 0x00);
	sensor_write_register(0x2D6, 0x40);
	sensor_write_register(0x2D8, 0x66);
	sensor_write_register(0x2D9, 0x0E);
	sensor_write_register(0x2DB, 0x06);
	sensor_write_register(0x2DC, 0x06);
	sensor_write_register(0x2DD, 0x0C);
	sensor_write_register(0x2E0, 0x7C);

	// chip_id = 0x3
	sensor_write_register(0x302, 0x3F);
	sensor_write_register(0x304, 0xF0);
	sensor_write_register(0x305, 0x00);
	sensor_write_register(0x306, 0xF0);
	sensor_write_register(0x307, 0xA0);
	sensor_write_register(0x308, 0x10);
	sensor_write_register(0x309, 0x10);
	sensor_write_register(0x30A, 0x24);
	sensor_write_register(0x30B, 0x42);
	sensor_write_register(0x30C, 0xFA);
	sensor_write_register(0x30D, 0x43);
	sensor_write_register(0x30E, 0x43);
	sensor_write_register(0x30F, 0x46);
	sensor_write_register(0x310, 0x04);
	sensor_write_register(0x311, 0x41);
	sensor_write_register(0x312, 0x35);
	sensor_write_register(0x313, 0x41);
	sensor_write_register(0x314, 0x35);
	sensor_write_register(0x315, 0x51);
	sensor_write_register(0x316, 0x49);
	sensor_write_register(0x317, 0x41);
	sensor_write_register(0x318, 0x35);
	sensor_write_register(0x319, 0x00);
	sensor_write_register(0x31B, 0x00);
	sensor_write_register(0x31F, 0x09);
	sensor_write_register(0x320, 0xC1);
	sensor_write_register(0x321, 0x10);
	sensor_write_register(0x323, 0x60);
	sensor_write_register(0x324, 0x44);
	sensor_write_register(0x325, 0xFA);
	sensor_write_register(0x326, 0x43);
	sensor_write_register(0x327, 0x43);
	sensor_write_register(0x329, 0x40);
	sensor_write_register(0x32A, 0x43);
	sensor_write_register(0x32B, 0xFA);
	sensor_write_register(0x32C, 0x43);
	sensor_write_register(0x32D, 0x43);
	sensor_write_register(0x32E, 0x4A);
	sensor_write_register(0x32F, 0xB4);
	sensor_write_register(0x330, 0x3F);
	sensor_write_register(0x332, 0xD0);
	sensor_write_register(0x335, 0x10);
	sensor_write_register(0x337, 0xFA);
	sensor_write_register(0x338, 0x13);
	sensor_write_register(0x33A, 0xFA);
	sensor_write_register(0x33B, 0x13);
	sensor_write_register(0x33E, 0xC0);
	sensor_write_register(0x33F, 0x02);
	sensor_write_register(0x343, 0xEE);
	sensor_write_register(0x344, 0xF0);
	sensor_write_register(0x345, 0x0E);
	sensor_write_register(0x346, 0x21);
	sensor_write_register(0x34B, 0x28);
	sensor_write_register(0x34D, 0x31);
	sensor_write_register(0x34F, 0x31);
	sensor_write_register(0x351, 0x16);
	sensor_write_register(0x352, 0x01);
	sensor_write_register(0x353, 0x16);
	sensor_write_register(0x354, 0x01);
	sensor_write_register(0x357, 0x36);
	sensor_write_register(0x359, 0x0F);
	sensor_write_register(0x35B, 0xFA);
	sensor_write_register(0x35C, 0x03);
	sensor_write_register(0x35D, 0x0E);
	sensor_write_register(0x35F, 0xF1);
	sensor_write_register(0x363, 0xF9);
	sensor_write_register(0x364, 0x03);
	sensor_write_register(0x369, 0x34);
	sensor_write_register(0x36B, 0x2D);
	sensor_write_register(0x36D, 0xFB);
	sensor_write_register(0x36F, 0xF6);
	sensor_write_register(0x373, 0x2D);
	sensor_write_register(0x379, 0x2F);
	sensor_write_register(0x37B, 0x2D);
	sensor_write_register(0x37D, 0xF8);
	sensor_write_register(0x37F, 0xF6);
	sensor_write_register(0x383, 0xF6);
	sensor_write_register(0x398, 0xF7);
	sensor_write_register(0x39A, 0x0B);
	sensor_write_register(0x39B, 0x01);
	sensor_write_register(0x39C, 0x06);
	sensor_write_register(0x39E, 0x0E);
	sensor_write_register(0x3AC, 0x16);
	sensor_write_register(0x3AE, 0x07);
	sensor_write_register(0x3B0, 0x16);
	sensor_write_register(0x3B2, 0x07);
	sensor_write_register(0x3B4, 0x0E);
	sensor_write_register(0x3B6, 0x0B);
	sensor_write_register(0x3B7, 0x01);
	sensor_write_register(0x3B8, 0x0B);
	sensor_write_register(0x3B9, 0x01);
	sensor_write_register(0x3BA, 0x0E);
	sensor_write_register(0x3BC, 0x0E);
	sensor_write_register(0x3BE, 0x0B);
	sensor_write_register(0x3BF, 0x01);
	sensor_write_register(0x3C0, 0x0F);
	sensor_write_register(0x3C1, 0x01);
	sensor_write_register(0x3C2, 0x13);
	sensor_write_register(0x3C3, 0x01);
	sensor_write_register(0x3C4, 0x12);
	sensor_write_register(0x3C6, 0x16);
	sensor_write_register(0x3C8, 0xF7);
	sensor_write_register(0x3C9, 0x30);
	sensor_write_register(0x3CA, 0x11);
	sensor_write_register(0x3CB, 0x06);
	sensor_write_register(0x3CC, 0x60);
	sensor_write_register(0x3CE, 0x36);
	sensor_write_register(0x3D0, 0xEF);
	sensor_write_register(0x3D1, 0x60);
	sensor_write_register(0x3D2, 0x13);
	sensor_write_register(0x3D3, 0xFB);
	sensor_write_register(0x3D4, 0x03);
	sensor_write_register(0x3D5, 0x24);
	sensor_write_register(0x3D6, 0x04);
	sensor_write_register(0x3D7, 0x28);
	sensor_write_register(0x3D8, 0x04);
	sensor_write_register(0x3DD, 0x3F);
	sensor_write_register(0x3DF, 0xEE);
	sensor_write_register(0x3E1, 0x3F);
	sensor_write_register(0x3E2, 0x01);
	sensor_write_register(0x3E3, 0xFA);
	sensor_write_register(0x3E4, 0x03);
	
	// standby cancel
	sensor_write_register(0x200, 0x54);

	// waiting for internal regular stabilization
	usleep(200000);
	
	// master mode start
	sensor_write_register(0x22D, 0x08);

	// XVS,XHS output start
	sensor_write_register(0x229, 0xC0);

	// waiting for image stabilization
	usleep(200000);

	printf("-------Sony IMX036 Sensor Initial OK!-------\n");
}