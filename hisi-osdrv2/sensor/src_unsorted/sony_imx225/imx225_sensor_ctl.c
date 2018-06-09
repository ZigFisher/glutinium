/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : sony225_sensor_ctl.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2014/11/07
  Description   : Sony IMX225 sensor driver
  History       :
  1.Date        : 2014/11/07
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


void sensor_init()
{
 
	sensor_write_register(0x200, 0x01); //Standby
	usleep(200000);
		
    
	// chip_id = 0x2
	sensor_write_register(0x207, 0x10);
	sensor_write_register(0x209, 0x01);
	sensor_write_register(0x20f, 0x00);
	sensor_write_register(0x212, 0x2c);
	sensor_write_register(0x213, 0x01);
	sensor_write_register(0x216, 0x09);
	sensor_write_register(0x218, 0xee);
	sensor_write_register(0x219, 0x02);
	sensor_write_register(0x21b, 0xc8);
	sensor_write_register(0x21c, 0x19);
	sensor_write_register(0x21d, 0xc2);
    sensor_write_register(0x246, 0x03);
    sensor_write_register(0x247, 0x06);
    sensor_write_register(0x248, 0xc2);
	sensor_write_register(0x25c, 0x20);
	sensor_write_register(0x25D, 0x00);
	sensor_write_register(0x25E, 0x20);
	sensor_write_register(0x25F, 0x00);
	sensor_write_register(0x270, 0x02);
	sensor_write_register(0x271, 0x01);
	sensor_write_register(0x29E, 0x22);
	sensor_write_register(0x2A5, 0xFB);
	sensor_write_register(0x2A6, 0x02);
	sensor_write_register(0x2B3, 0xFF);
	sensor_write_register(0x2B4, 0x01);
	sensor_write_register(0x2B5, 0x42);
	sensor_write_register(0x2B8, 0x10);
	sensor_write_register(0x2C2, 0x01);
    
	// chip_id = 0x3
	sensor_write_register(0x30F, 0x0F);
	sensor_write_register(0x310, 0x0E);
	sensor_write_register(0x311, 0xE7);
	sensor_write_register(0x312, 0x9C);
	sensor_write_register(0x313, 0x83);
	sensor_write_register(0x314, 0x10);
	sensor_write_register(0x315, 0x42);
	sensor_write_register(0x328, 0x1E);
	sensor_write_register(0x3ED, 0x38);
	
	// chip_id = 0x4
	sensor_write_register(0x40C, 0xCF);
	sensor_write_register(0x44C, 0x40);
	sensor_write_register(0x44D, 0x03);
	sensor_write_register(0x461, 0xE0);
	sensor_write_register(0x462, 0x02);
	sensor_write_register(0x46E, 0x2F);
	sensor_write_register(0x46F, 0x30);
	sensor_write_register(0x470, 0x03);
	sensor_write_register(0x498, 0x00);
	sensor_write_register(0x49A, 0x12);
	sensor_write_register(0x49B, 0xF1);
	sensor_write_register(0x49C, 0x0C);

	usleep(200000);
	sensor_write_register(0x200, 0x00); //release standy
	usleep(200000);
	sensor_write_register(0x202, 0x00); //Master mose start
	usleep(200000);
	sensor_write_register(0x249, 0x80); //XVS & XHS output
	usleep(200000);

	printf("-------Sony IMX225 Sensor Initial OK!-------\n");
    
	return ; 

}



