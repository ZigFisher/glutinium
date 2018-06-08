/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : sony122_sensor_ctl.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/09/09
  Description   : Sony IMX222 sensor driver
  History       :
  1.Date        : 2011/09/09
    Author      : MPP
    Modification: Created file

******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "hi_comm_video.h"

#include "hi_spi.h"
extern HI_U8 gu8SensorImageMode;
extern HI_BOOL bSensorInit;
static int g_fd = -1;

int sensor_spi_init(void)
{
    if(g_fd >= 0)
    {
        return 0;
    }    
	unsigned int value;
	int ret = 0;
	char file_name[] = "/dev/spidev0.0";

	g_fd = open(file_name, 0);
	if (g_fd < 0)
	{
		printf("Open %s error!\n",file_name);
		return -1;
	}

	value = SPI_MODE_3 | SPI_LSB_FIRST;// | SPI_LOOP;
	ret = ioctl(g_fd, SPI_IOC_WR_MODE, &value);
	if (ret < 0)
	{
		printf("ioctl SPI_IOC_WR_MODE err, value = %d ret = %d\n", value, ret);
		return ret;
	}

	value = 8;
	ret = ioctl(g_fd, SPI_IOC_WR_BITS_PER_WORD, &value);
	if (ret < 0)
	{
		printf("ioctl SPI_IOC_WR_BITS_PER_WORD err, value = %d ret = %d\n",value, ret);
		return ret;
	}

	value = 2000000;
	ret = ioctl(g_fd, SPI_IOC_WR_MAX_SPEED_HZ, &value);
	if (ret < 0)
	{
		printf("ioctl SPI_IOC_WR_MAX_SPEED_HZ err, value = %d ret = %d\n",value, ret);
		return ret;
	}

	return 0;
}

int sensor_spi_exit(void)
{
    if (g_fd >= 0)
    {
        close(g_fd);
        g_fd = -1;
        return 0;
    }
    return -1;
}

int sensor_write_register(unsigned int addr, unsigned char data)
{
	int ret;
	struct spi_ioc_transfer mesg[1];
	unsigned char  tx_buf[8] = {0};
	unsigned char  rx_buf[8] = {0};
	
	tx_buf[0] = (addr & 0xff00) >> 8;
	tx_buf[0] &= (~0x80);
	tx_buf[1] = addr & 0xff;
	tx_buf[2] = data;

	//printf("func:%s tx_buf = %#x, %#x, %#x\n", __func__, tx_buf[0], tx_buf[1], tx_buf[2]);

	memset(mesg, 0, sizeof(mesg));  
	mesg[0].tx_buf = (__u32)tx_buf;  
	mesg[0].len    = 3;  
	mesg[0].rx_buf = (__u32)rx_buf; 
	mesg[0].cs_change = 1;

	ret = ioctl(g_fd, SPI_IOC_MESSAGE(1), mesg);
	if (ret < 0) {  
		printf("SPI_IOC_MESSAGE error \n");  
		return -1;  
	}
	//printf("func:%s ret = %d, rx_buf = %#x, %#x, %#x\n", __func__, ret , rx_buf[0], rx_buf[1], rx_buf[2]);

	return 0;
}

int sensor_read_register(unsigned int addr)
{
	int ret = 0;
	struct spi_ioc_transfer mesg[1];
	unsigned char  tx_buf[8] = {0};
	unsigned char  rx_buf[8] = {0};
	
	tx_buf[0] = (addr & 0xff00) >> 8;
	tx_buf[0] |= 0x80;
	tx_buf[1] = addr & 0xff;
	tx_buf[2] = 0;

	memset(mesg, 0, sizeof(mesg));
	mesg[0].tx_buf = (__u32)tx_buf;
	mesg[0].len    = 3;
	mesg[0].rx_buf = (__u32)rx_buf;
	mesg[0].cs_change = 1;

	ret = ioctl(g_fd, SPI_IOC_MESSAGE(1), mesg);
	if (ret  < 0) {  
		printf("SPI_IOC_MESSAGE error \n");  
		return -1;  
	}
	//printf("func:%s ret = %d, rx_buf = %#x, %#x, %#x\n", __func__, ret , rx_buf[0], rx_buf[1], rx_buf[2]);
	
	return rx_buf[2];
}


void sensor_prog(int* rom) 
{
}

void setup_sensor(int isp_mode)
{
}

void sensor_init_1080p_30fps();
void sensor_init_720p_60fps();

void sensor_init()
{
	// sequence according to "Flow  Power-on to Operation Start(Sensor Master Mode)

	// chip_id = 0x2
	/* 1. sensor spi init */
	sensor_spi_init();

    if (1 == gu8SensorImageMode)    /* SENSOR_1080P_30FPS_MODE */
    {
        sensor_init_1080p_30fps();
        bSensorInit = HI_TRUE;
    }
    else if (2 == gu8SensorImageMode) /* SENSOR_720P_60FPS_MODE */
    {
        sensor_init_720p_60fps();
        bSensorInit = HI_TRUE;
    }
    else
    {
        printf("Not support this mode\n");
    }
	
}


void sensor_exit()
{
    sensor_spi_exit();

    return;
}


void sensor_init_720p_30fps()
{

        sensor_write_register(0x200, 0x31); //sensor_write_register(0x200, 0x01)
        sensor_write_register(0x211, 0x00);
        sensor_write_register(0x22D, 0x40);
        sensor_write_register(0x202, 0x01);
        sensor_write_register(0x216, 0xF0);
        sensor_write_register(0x217, 0x00);
        sensor_write_register(0x214, 0x40);
        sensor_write_register(0x215, 0x01);
        sensor_write_register(0x218, 0x40);
        sensor_write_register(0x219, 0x05);
        sensor_write_register(0x2CE, 0x00);
        sensor_write_register(0x2CF, 0x00);
        sensor_write_register(0x2D0, 0x00);
        sensor_write_register(0x29A, 0x4C);
        sensor_write_register(0x29B, 0x04);
        //sensor_write_register(0x212, 0x82);//  sensor_write_register(0x212, 0x80);
        sensor_write_register(0x212, 0x80);

        sensor_write_register(0x20F, 0x00);
        sensor_write_register(0x20D, 0x00);
        sensor_write_register(0x208, 0x00);
        sensor_write_register(0x209, 0x00);
        sensor_write_register(0x21E, 0x00);
        sensor_write_register(0x220, 0x3C);
        sensor_write_register(0x221, 0x00);
        //  sensor_write_register(0x222, 0x01); // sensor_write_register(0x222, 0xC0)

        sensor_write_register(0x222, 0xC0);
         
        sensor_write_register(0x205, 0xEE);
        sensor_write_register(0x206, 0x02);
        sensor_write_register(0x203, 0x72);
        sensor_write_register(0x204, 0x06);
        sensor_write_register(0x23B, 0xE0);

            // master mode start
        sensor_write_register(0x22C, 0x00);    //  sensor_write_register(0x22C, 0x01);

        sensor_write_register(0x201, 0x00);

        sensor_write_register(0x207, 0x00);

        sensor_write_register(0x20A, 0x00);
        sensor_write_register(0x20B, 0x00);
        sensor_write_register(0x20C, 0x00);

        sensor_write_register(0x20E, 0x00);
        sensor_write_register(0x210, 0x00);
        sensor_write_register(0x213, 0x40);
        sensor_write_register(0x21A, 0xE9);
        sensor_write_register(0x21B, 0x02);
        sensor_write_register(0x21C, 0x50);
        sensor_write_register(0x21D, 0x00);

        sensor_write_register(0x21F, 0x31);

        sensor_write_register(0x223, 0x08);
        sensor_write_register(0x224, 0x30);
        sensor_write_register(0x225, 0x00);
        sensor_write_register(0x226, 0x80);
        sensor_write_register(0x227, 0x20);
        sensor_write_register(0x228, 0x34);
        sensor_write_register(0x229, 0x63);
        sensor_write_register(0x22A, 0x00);
        sensor_write_register(0x22B, 0x00);

        sensor_write_register(0x22E, 0x00);
        sensor_write_register(0x22F, 0x02);

        sensor_write_register(0x230, 0x30);
        sensor_write_register(0x231, 0x20);
        sensor_write_register(0x232, 0x00);
        sensor_write_register(0x233, 0x14);
        sensor_write_register(0x234, 0x20);
        sensor_write_register(0x235, 0x60);
        sensor_write_register(0x236, 0x00);
        sensor_write_register(0x237, 0x23);
        sensor_write_register(0x238, 0x01);
        sensor_write_register(0x239, 0x00);
        sensor_write_register(0x23A, 0xA8);
        sensor_write_register(0x23B, 0xE0);
        sensor_write_register(0x23C, 0x06);
        sensor_write_register(0x23D, 0x00);
        sensor_write_register(0x23E, 0x10);
        sensor_write_register(0x23F, 0x00);
        sensor_write_register(0x240, 0x42);
        sensor_write_register(0x241, 0x23);
        sensor_write_register(0x242, 0x3C);
        sensor_write_register(0x243, 0x01);
        sensor_write_register(0x244, 0x00);
        sensor_write_register(0x245, 0x00);
        sensor_write_register(0x246, 0x00);
        sensor_write_register(0x247, 0x00);
        sensor_write_register(0x248, 0x00);
        sensor_write_register(0x249, 0x00);
        sensor_write_register(0x24A, 0x00);
        sensor_write_register(0x24B, 0x00);
        sensor_write_register(0x24C, 0x01);
        sensor_write_register(0x24D, 0x00);
        sensor_write_register(0x24E, 0x01);
        sensor_write_register(0x24F, 0x07);
        sensor_write_register(0x250, 0x10);
        sensor_write_register(0x251, 0x18);
        sensor_write_register(0x252, 0x12);
        sensor_write_register(0x253, 0x00);
        sensor_write_register(0x254, 0x00);
        sensor_write_register(0x255, 0x00);
        sensor_write_register(0x256, 0x00);
        sensor_write_register(0x257, 0x00);
        sensor_write_register(0x258, 0xE0);
        sensor_write_register(0x259, 0x01);
        sensor_write_register(0x25A, 0xE0);
        sensor_write_register(0x25B, 0x01);
        sensor_write_register(0x25C, 0x00);
        sensor_write_register(0x25D, 0x00);
        sensor_write_register(0x25E, 0x00);
        sensor_write_register(0x25F, 0x00);
        sensor_write_register(0x260, 0x00);
        sensor_write_register(0x261, 0x00);
        sensor_write_register(0x262, 0x76);
        sensor_write_register(0x263, 0x00);
        sensor_write_register(0x264, 0x01);
        sensor_write_register(0x265, 0x00);
        sensor_write_register(0x266, 0x00);
        sensor_write_register(0x267, 0x00);
        sensor_write_register(0x268, 0x00);
        sensor_write_register(0x269, 0x00);
        sensor_write_register(0x26A, 0x00);
        sensor_write_register(0x26B, 0x00);
        sensor_write_register(0x26C, 0x00);
        sensor_write_register(0x26D, 0x00);
        sensor_write_register(0x26E, 0x00);
        sensor_write_register(0x26F, 0x00);
        sensor_write_register(0x270, 0x00);
        sensor_write_register(0x271, 0x00);
        sensor_write_register(0x272, 0x00);
        sensor_write_register(0x273, 0x01);
        sensor_write_register(0x274, 0x06);
        sensor_write_register(0x275, 0x07);
        sensor_write_register(0x276, 0x80);
        sensor_write_register(0x277, 0x00);
        sensor_write_register(0x278, 0x40);
        sensor_write_register(0x279, 0x08);
        sensor_write_register(0x27A, 0x00);
        sensor_write_register(0x27B, 0x00);
        sensor_write_register(0x27C, 0x10);
        sensor_write_register(0x27D, 0x00);
        sensor_write_register(0x27E, 0x00);
        sensor_write_register(0x27F, 0x00);
        sensor_write_register(0x280, 0x06);
        sensor_write_register(0x281, 0x19);
        sensor_write_register(0x282, 0x00);
        sensor_write_register(0x283, 0x64);
        sensor_write_register(0x284, 0x00);
        sensor_write_register(0x285, 0x01);
        sensor_write_register(0x286, 0x00);
        sensor_write_register(0x287, 0x00);
        sensor_write_register(0x288, 0x00);
        sensor_write_register(0x289, 0x00);
        sensor_write_register(0x28A, 0x00);
        sensor_write_register(0x28B, 0x00);
        sensor_write_register(0x28C, 0x00);
        sensor_write_register(0x28D, 0x00);
        sensor_write_register(0x28E, 0x00);
        sensor_write_register(0x28F, 0x00);
        sensor_write_register(0x290, 0x00);
        sensor_write_register(0x291, 0x00);
        sensor_write_register(0x292, 0x01);
        sensor_write_register(0x293, 0x01);
        sensor_write_register(0x294, 0x00);
        sensor_write_register(0x295, 0xFF);
        sensor_write_register(0x296, 0x0F);
        sensor_write_register(0x297, 0x00);
        sensor_write_register(0x298, 0x26);
        sensor_write_register(0x299, 0x02);

        sensor_write_register(0x29A, 0x4C); //new
        sensor_write_register(0x29B, 0x04); //new

        sensor_write_register(0x29C, 0x9C);
        sensor_write_register(0x29D, 0x01);
        sensor_write_register(0x29E, 0x39);
        sensor_write_register(0x29F, 0x03);
        sensor_write_register(0x2A0, 0x01);
        sensor_write_register(0x2A1, 0x05);
        sensor_write_register(0x2A2, 0xD0);
        sensor_write_register(0x2A3, 0x07);
        sensor_write_register(0x2A4, 0x00);
        sensor_write_register(0x2A5, 0x02);
        sensor_write_register(0x2A6, 0x0B);
        sensor_write_register(0x2A7, 0x0F);
        sensor_write_register(0x2A8, 0x24);
        sensor_write_register(0x2A9, 0x00);
        sensor_write_register(0x2AA, 0x28);
        sensor_write_register(0x2AB, 0x00);
        sensor_write_register(0x2AC, 0xE8);
        sensor_write_register(0x2AD, 0x04);
        sensor_write_register(0x2AE, 0xEC);
        sensor_write_register(0x2AF, 0x04);
        sensor_write_register(0x2B0, 0x00);
        sensor_write_register(0x2B1, 0x00);
        sensor_write_register(0x2B2, 0x03);
        sensor_write_register(0x2B3, 0x05);
        sensor_write_register(0x2B4, 0x00);
        sensor_write_register(0x2B5, 0x0F);
        sensor_write_register(0x2B6, 0x10);
        sensor_write_register(0x2B7, 0x00);
        sensor_write_register(0x2B8, 0x28);
        sensor_write_register(0x2B9, 0x00);
        sensor_write_register(0x2BA, 0xBF);
        sensor_write_register(0x2BB, 0x07);
        sensor_write_register(0x2BC, 0xCF);
        sensor_write_register(0x2BD, 0x07);
        sensor_write_register(0x2BE, 0xCF);
        sensor_write_register(0x2BF, 0x07);
        sensor_write_register(0x2C0, 0xCF);
        sensor_write_register(0x2C1, 0x07);
        sensor_write_register(0x2C2, 0xD0);
        sensor_write_register(0x2C3, 0x07);
        sensor_write_register(0x2C4, 0x01);
        sensor_write_register(0x2C5, 0x02);
        sensor_write_register(0x2C6, 0x03);
        sensor_write_register(0x2C7, 0x04);
        sensor_write_register(0x2C8, 0x05);
        sensor_write_register(0x2C9, 0x06);
        sensor_write_register(0x2CA, 0x07);
        sensor_write_register(0x2CB, 0x08);
        sensor_write_register(0x2CC, 0x01);
        sensor_write_register(0x2CD, 0x03);

        sensor_write_register(0x2D1, 0x00);
        sensor_write_register(0x2D2, 0x00);
        sensor_write_register(0x2D3, 0x00);
        sensor_write_register(0x2D4, 0x00);
        sensor_write_register(0x2D5, 0x00);
        sensor_write_register(0x2D6, 0x00);
        sensor_write_register(0x2D7, 0x00);
        sensor_write_register(0x2D8, 0x00);
        sensor_write_register(0x2D9, 0x00);
        sensor_write_register(0x2DA, 0x00);
        sensor_write_register(0x2DB, 0x00);
        sensor_write_register(0x2DC, 0x00);
        sensor_write_register(0x2DD, 0x00);
        sensor_write_register(0x2DE, 0x00);
        sensor_write_register(0x2DF, 0x00);
        sensor_write_register(0x2E0, 0x00);
        sensor_write_register(0x2E1, 0x00);
        sensor_write_register(0x2E2, 0x00);
        sensor_write_register(0x2E3, 0x00);
        sensor_write_register(0x2E4, 0x00);
        sensor_write_register(0x2E5, 0x00);
        sensor_write_register(0x2E6, 0x00);
        sensor_write_register(0x2E7, 0x00);
        sensor_write_register(0x2E8, 0x00);
        sensor_write_register(0x2E9, 0x00);
        sensor_write_register(0x2EA, 0x00);
        sensor_write_register(0x2EB, 0x00);
        sensor_write_register(0x2EC, 0x00);
        sensor_write_register(0x2ED, 0x00);
        sensor_write_register(0x2EE, 0x00);
        sensor_write_register(0x2EF, 0x00);
        sensor_write_register(0x2F0, 0x00);
        sensor_write_register(0x2F1, 0x00);
        sensor_write_register(0x2F2, 0x00);
        sensor_write_register(0x2F3, 0x00);
        sensor_write_register(0x2F4, 0x00);
        sensor_write_register(0x2F5, 0x00);
        sensor_write_register(0x2F6, 0x00);
        sensor_write_register(0x2F7, 0x00);
        sensor_write_register(0x2F8, 0x00);
        sensor_write_register(0x2F9, 0x00);
        sensor_write_register(0x2FA, 0x00);
        sensor_write_register(0x2FB, 0x00);
        sensor_write_register(0x2FC, 0x00);
        sensor_write_register(0x2FD, 0x00);
        sensor_write_register(0x2FE, 0x00);
        sensor_write_register(0x2FF, 0x00);

        // chip_id = 0x3

        sensor_write_register(0x300, 0x00);
        sensor_write_register(0x301, 0x00);
        sensor_write_register(0x302, 0x00);
        sensor_write_register(0x303, 0x00);
        sensor_write_register(0x304, 0x00);
        sensor_write_register(0x305, 0x00);
        sensor_write_register(0x306, 0x00);
        sensor_write_register(0x307, 0xFA);
        sensor_write_register(0x308, 0xFA);
        sensor_write_register(0x309, 0x41);
        sensor_write_register(0x30A, 0x31);
        sensor_write_register(0x30B, 0x38);
        sensor_write_register(0x30C, 0x04);
        sensor_write_register(0x30D, 0x00);
        sensor_write_register(0x30E, 0x1A);
        sensor_write_register(0x30F, 0x10);
        sensor_write_register(0x310, 0x00);
        sensor_write_register(0x311, 0x00);
        sensor_write_register(0x312, 0x10);
        sensor_write_register(0x313, 0x00);
        sensor_write_register(0x314, 0x00);
        sensor_write_register(0x315, 0x06);
        sensor_write_register(0x316, 0x33);
        sensor_write_register(0x317, 0x0D);
        sensor_write_register(0x318, 0x00);
        sensor_write_register(0x319, 0x00);
        sensor_write_register(0x31A, 0x00);
        sensor_write_register(0x31B, 0x00);
        sensor_write_register(0x31C, 0x00);
        sensor_write_register(0x31D, 0x00);
        sensor_write_register(0x31E, 0x00);
        sensor_write_register(0x31F, 0x00);
        sensor_write_register(0x320, 0x00);
        sensor_write_register(0x321, 0x80);
        sensor_write_register(0x322, 0x0C);
        sensor_write_register(0x323, 0x00);
        sensor_write_register(0x324, 0x00);
        sensor_write_register(0x325, 0x00);
        sensor_write_register(0x326, 0x00);
        sensor_write_register(0x327, 0x00);
        sensor_write_register(0x328, 0x05);
        sensor_write_register(0x329, 0x80);
        sensor_write_register(0x32A, 0x00);
        sensor_write_register(0x32B, 0x00);
        sensor_write_register(0x32C, 0x04);
        sensor_write_register(0x32D, 0x04);
        sensor_write_register(0x32E, 0x00);
        sensor_write_register(0x32F, 0x00);
        sensor_write_register(0x330, 0x9B);
        sensor_write_register(0x331, 0x71);
        sensor_write_register(0x332, 0x33);
        sensor_write_register(0x333, 0x37);
        sensor_write_register(0x334, 0xB3);
        sensor_write_register(0x335, 0x19);
        sensor_write_register(0x336, 0x97);
        sensor_write_register(0x337, 0xB1);
        sensor_write_register(0x338, 0x19);
        sensor_write_register(0x339, 0x01);
        sensor_write_register(0x33A, 0x50);
        sensor_write_register(0x33B, 0x00);
        sensor_write_register(0x33C, 0x35);
        sensor_write_register(0x33D, 0xB0);
        sensor_write_register(0x33E, 0x03);
        sensor_write_register(0x33F, 0xD1);
        sensor_write_register(0x340, 0x71);
        sensor_write_register(0x341, 0x1D);
        sensor_write_register(0x342, 0x00);
        sensor_write_register(0x343, 0x00);
        sensor_write_register(0x344, 0x00);
        sensor_write_register(0x345, 0x00);
        sensor_write_register(0x346, 0x02);
        sensor_write_register(0x347, 0x30);
        sensor_write_register(0x348, 0x00);
        sensor_write_register(0x349, 0x00);
        sensor_write_register(0x34A, 0x00);
        sensor_write_register(0x34B, 0x03);
        sensor_write_register(0x34C, 0x00);
        sensor_write_register(0x34D, 0x02);
        sensor_write_register(0x34E, 0x10);
        sensor_write_register(0x34F, 0xA0);
        sensor_write_register(0x350, 0x00);
        sensor_write_register(0x351, 0x07);
        sensor_write_register(0x352, 0x40);
        sensor_write_register(0x353, 0x80);
        sensor_write_register(0x354, 0x00);
        sensor_write_register(0x355, 0x02);
        sensor_write_register(0x356, 0x50);
        sensor_write_register(0x357, 0x02);
        sensor_write_register(0x358, 0x23);
        sensor_write_register(0x359, 0xE4);
        sensor_write_register(0x35A, 0x45);
        sensor_write_register(0x35B, 0x33);
        sensor_write_register(0x35C, 0x79);
        sensor_write_register(0x35D, 0xD1);
        sensor_write_register(0x35E, 0xCC);
        sensor_write_register(0x35F, 0x2F);
        sensor_write_register(0x360, 0xB6);
        sensor_write_register(0x361, 0xA1);
        sensor_write_register(0x362, 0x17);
        sensor_write_register(0x363, 0xCB);
        sensor_write_register(0x364, 0xE8);
        sensor_write_register(0x365, 0xC5);
        sensor_write_register(0x366, 0x32);
        sensor_write_register(0x367, 0xC0);
        sensor_write_register(0x368, 0xA8);
        sensor_write_register(0x369, 0xC6);
        sensor_write_register(0x36A, 0x5E);
        sensor_write_register(0x36B, 0x20);
        sensor_write_register(0x36C, 0x63);
        sensor_write_register(0x36D, 0x0D);
        sensor_write_register(0x36E, 0x6D);
        sensor_write_register(0x36F, 0x44);
        sensor_write_register(0x370, 0xA6);
        sensor_write_register(0x371, 0x32);
        sensor_write_register(0x372, 0x24);
        sensor_write_register(0x373, 0x50);
        sensor_write_register(0x374, 0xC4);
        sensor_write_register(0x375, 0x2F);
        sensor_write_register(0x376, 0xF4);
        sensor_write_register(0x377, 0x42);
        sensor_write_register(0x378, 0x82);
        sensor_write_register(0x379, 0x13);
        sensor_write_register(0x37A, 0x90);
        sensor_write_register(0x37B, 0x00);
        sensor_write_register(0x37C, 0x10);
        sensor_write_register(0x37D, 0x8A);
        sensor_write_register(0x37E, 0x60);
        sensor_write_register(0x37F, 0xC4);
        sensor_write_register(0x380, 0x2F);
        sensor_write_register(0x381, 0x84);
        sensor_write_register(0x382, 0xF1);
        sensor_write_register(0x383, 0x0B);
        sensor_write_register(0x384, 0xCD);
        sensor_write_register(0x385, 0x70);
        sensor_write_register(0x386, 0x42);
        sensor_write_register(0x387, 0x16);
        sensor_write_register(0x388, 0x00);
        sensor_write_register(0x389, 0x61);
        sensor_write_register(0x38A, 0x0B);
        sensor_write_register(0x38B, 0x29);
        sensor_write_register(0x38C, 0x74);
        sensor_write_register(0x38D, 0x81);
        sensor_write_register(0x38E, 0x10);
        sensor_write_register(0x38F, 0xBA);
        sensor_write_register(0x390, 0x18);
        sensor_write_register(0x391, 0x22);
        sensor_write_register(0x392, 0x11);
        sensor_write_register(0x393, 0xE9);
        sensor_write_register(0x394, 0x60);
        sensor_write_register(0x395, 0x07);
        sensor_write_register(0x396, 0x09);
        sensor_write_register(0x397, 0xF6);
        sensor_write_register(0x398, 0x40);
        sensor_write_register(0x399, 0x02);
        sensor_write_register(0x39A, 0x3C);
        sensor_write_register(0x39B, 0x00);
        sensor_write_register(0x39C, 0x00);
        sensor_write_register(0x39D, 0x00);
        sensor_write_register(0x39E, 0x00);
        sensor_write_register(0x39F, 0x00);
        sensor_write_register(0x3A0, 0x80);
        sensor_write_register(0x3A1, 0x0B);
        sensor_write_register(0x3A2, 0x64);
        sensor_write_register(0x3A3, 0x90);
        sensor_write_register(0x3A4, 0x8D);
        sensor_write_register(0x3A5, 0x6E);
        sensor_write_register(0x3A6, 0x98);
        sensor_write_register(0x3A7, 0x40);
        sensor_write_register(0x3A8, 0x05);
        sensor_write_register(0x3A9, 0xD1);
        sensor_write_register(0x3AA, 0xA8);
        sensor_write_register(0x3AB, 0x86);
        sensor_write_register(0x3AC, 0x09);
        sensor_write_register(0x3AD, 0x54);
        sensor_write_register(0x3AE, 0x10);
        sensor_write_register(0x3AF, 0x8D);
        sensor_write_register(0x3B0, 0x6A);
        sensor_write_register(0x3B1, 0xE8);
        sensor_write_register(0x3B2, 0x82);
        sensor_write_register(0x3B3, 0x17);
        sensor_write_register(0x3B4, 0x1C);
        sensor_write_register(0x3B5, 0x60);
        sensor_write_register(0x3B6, 0xC1);
        sensor_write_register(0x3B7, 0x31);
        sensor_write_register(0x3B8, 0xAE);
        sensor_write_register(0x3B9, 0xD1);
        sensor_write_register(0x3BA, 0x81);
        sensor_write_register(0x3BB, 0x16);
        sensor_write_register(0x3BC, 0x20);
        sensor_write_register(0x3BD, 0x03);
        sensor_write_register(0x3BE, 0x1B);
        sensor_write_register(0x3BF, 0x24);
        sensor_write_register(0x3C0, 0xE0);
        sensor_write_register(0x3C1, 0xC1);
        sensor_write_register(0x3C2, 0x33);
        sensor_write_register(0x3C3, 0xBE);
        sensor_write_register(0x3C4, 0x51);
        sensor_write_register(0x3C5, 0x82);
        sensor_write_register(0x3C6, 0x1E);
        sensor_write_register(0x3C7, 0x40);
        sensor_write_register(0x3C8, 0x03);
        sensor_write_register(0x3C9, 0x1C);
        sensor_write_register(0x3CA, 0x34);
        sensor_write_register(0x3CB, 0xD0);
        sensor_write_register(0x3CC, 0x81);
        sensor_write_register(0x3CD, 0x02);
        sensor_write_register(0x3CE, 0x16);
        sensor_write_register(0x3CF, 0x00);
        sensor_write_register(0x3D0, 0x02);
        sensor_write_register(0x3D1, 0x04);
        sensor_write_register(0x3D2, 0x00);
        sensor_write_register(0x3D3, 0x00);
        sensor_write_register(0x3D4, 0x00);
        sensor_write_register(0x3D5, 0x80);
        sensor_write_register(0x3D6, 0x00);
        sensor_write_register(0x3D7, 0x00);
        sensor_write_register(0x3D8, 0x23);
        sensor_write_register(0x3D9, 0x01);
        sensor_write_register(0x3DA, 0x03);
        sensor_write_register(0x3DB, 0x02);
        sensor_write_register(0x3DC, 0x00);
        sensor_write_register(0x3DD, 0x00);
        sensor_write_register(0x3DE, 0x00);
        sensor_write_register(0x3DF, 0x00);
        sensor_write_register(0x3E0, 0x22);
        sensor_write_register(0x3E1, 0x00);
        sensor_write_register(0x3E2, 0x00);
        sensor_write_register(0x3E3, 0x00);
        sensor_write_register(0x3E4, 0x3F);
        sensor_write_register(0x3E5, 0x17);
        sensor_write_register(0x3E6, 0x15);
        sensor_write_register(0x3E7, 0x00);
        sensor_write_register(0x3E8, 0x00);
        sensor_write_register(0x3E9, 0x00);
        sensor_write_register(0x3EA, 0x00);
        sensor_write_register(0x3EB, 0x00);
        sensor_write_register(0x3EC, 0x00);
        sensor_write_register(0x3ED, 0x00);
        sensor_write_register(0x3EE, 0x00);
        sensor_write_register(0x3EF, 0x00);
        sensor_write_register(0x3F0, 0x00);
        sensor_write_register(0x3F1, 0x00);
        sensor_write_register(0x3F2, 0x00);
        sensor_write_register(0x3F3, 0x00);
        sensor_write_register(0x3F4, 0x00);
        sensor_write_register(0x3F5, 0x00);
        sensor_write_register(0x3F6, 0x00);
        sensor_write_register(0x3F7, 0x00);
        sensor_write_register(0x3F8, 0x00);
        sensor_write_register(0x3F9, 0x00);
        sensor_write_register(0x3FA, 0x00);
        sensor_write_register(0x3FB, 0x00);
        sensor_write_register(0x3FC, 0x00);
        sensor_write_register(0x3FD, 0x00);
        sensor_write_register(0x3FE, 0x00);
        sensor_write_register(0x3FF, 0x00);

           // standby cancel
           sensor_write_register(0x200, 0x30);
        // XVS,XHS output start
        //sensor_write_register(0x229, 0xC0);

        // waiting for image stabilization
        usleep(200000);

	printf("-------Sony IMX222 Sensor Initial OK!-------\n");

}



void sensor_init_1080p_30fps()
{
    sensor_write_register(0x200, 0x31);
    sensor_write_register(0x211, 0x00);
    sensor_write_register(0x22D, 0x40);
    sensor_write_register(0x202, 0x0F);
    sensor_write_register(0x216, 0x3C);
    sensor_write_register(0x217, 0x00);
    sensor_write_register(0x214, 0x00);
    sensor_write_register(0x215, 0x00);
    sensor_write_register(0x218, 0xC0);
    sensor_write_register(0x219, 0x07);
    sensor_write_register(0x2CE, 0x16);
    sensor_write_register(0x2CF, 0x82);
    sensor_write_register(0x2D0, 0x00);
    sensor_write_register(0x29A, 0x26);
    sensor_write_register(0x29B, 0x02);
    sensor_write_register(0x212, 0x82);
    sensor_write_register(0x20F, 0x00);
    sensor_write_register(0x20D, 0x00);
    sensor_write_register(0x208, 0x00);
    sensor_write_register(0x209, 0x00);
    sensor_write_register(0x21E, 0x00);
    sensor_write_register(0x220, 0xF0);
    sensor_write_register(0x221, 0x00);
    sensor_write_register(0x222, 0x40);
    sensor_write_register(0x205, 0x65);
    sensor_write_register(0x206, 0x04);
    sensor_write_register(0x203, 0x4C);
    sensor_write_register(0x204, 0x04);
    sensor_write_register(0x23B, 0xE0);

        // master mode start
    sensor_write_register(0x22C, 0x00);


    sensor_write_register(0x201, 0x00);



    sensor_write_register(0x207, 0x00);

    sensor_write_register(0x20A, 0x00);
    sensor_write_register(0x20B, 0x00);
    sensor_write_register(0x20C, 0x00);

    sensor_write_register(0x20E, 0x00);

    sensor_write_register(0x210, 0x00);
    

    sensor_write_register(0x213, 0x40);



    sensor_write_register(0x21A, 0xC9);
    sensor_write_register(0x21B, 0x04);
    sensor_write_register(0x21C, 0x50);
    sensor_write_register(0x21D, 0x00);

    sensor_write_register(0x21F, 0x31);


    sensor_write_register(0x223, 0x08);
    sensor_write_register(0x224, 0x30);
    sensor_write_register(0x225, 0x00);
    sensor_write_register(0x226, 0x80);
    sensor_write_register(0x227, 0x20);
    sensor_write_register(0x228, 0x34);
    sensor_write_register(0x229, 0x63);
    sensor_write_register(0x22A, 0x00);
    sensor_write_register(0x22B, 0x00);
    

    sensor_write_register(0x22E, 0x00);
    sensor_write_register(0x22F, 0x02);


    
    sensor_write_register(0x230, 0x30);
    sensor_write_register(0x231, 0x20);
    sensor_write_register(0x232, 0x00);
    sensor_write_register(0x233, 0x14);
    sensor_write_register(0x234, 0x20);
    sensor_write_register(0x235, 0x60);
    sensor_write_register(0x236, 0x00);
    sensor_write_register(0x237, 0x23);
    sensor_write_register(0x238, 0x01);
    sensor_write_register(0x239, 0x00);
    sensor_write_register(0x23A, 0xA8);
    sensor_write_register(0x23B, 0xE0);
    sensor_write_register(0x23C, 0x06);
    sensor_write_register(0x23D, 0x00);
    sensor_write_register(0x23E, 0x10);
    sensor_write_register(0x23F, 0x00);
    sensor_write_register(0x240, 0x42);
    sensor_write_register(0x241, 0x23);
    sensor_write_register(0x242, 0x3C);
    sensor_write_register(0x243, 0x01);
    sensor_write_register(0x244, 0x00);
    sensor_write_register(0x245, 0x00);
    sensor_write_register(0x246, 0x00);
    sensor_write_register(0x247, 0x00);
    sensor_write_register(0x248, 0x00);
    sensor_write_register(0x249, 0x00);
    sensor_write_register(0x24A, 0x00);
    sensor_write_register(0x24B, 0x00);
    sensor_write_register(0x24C, 0x01);
    sensor_write_register(0x24D, 0x00);
    sensor_write_register(0x24E, 0x01);
    sensor_write_register(0x24F, 0x47);
    sensor_write_register(0x250, 0x10);
    sensor_write_register(0x251, 0x18);
    sensor_write_register(0x252, 0x12);
    sensor_write_register(0x253, 0x00);
    sensor_write_register(0x254, 0x00);
    sensor_write_register(0x255, 0x00);
    sensor_write_register(0x256, 0x00);
    sensor_write_register(0x257, 0x00);
    sensor_write_register(0x258, 0xE0);
    sensor_write_register(0x259, 0x01);
    sensor_write_register(0x25A, 0xE0);
    sensor_write_register(0x25B, 0x01);
    sensor_write_register(0x25C, 0x00);
    sensor_write_register(0x25D, 0x00);
    sensor_write_register(0x25E, 0x00);
    sensor_write_register(0x25F, 0x00);
    sensor_write_register(0x260, 0x00);
    sensor_write_register(0x261, 0x00);
    sensor_write_register(0x262, 0x76);
    sensor_write_register(0x263, 0x00);
    sensor_write_register(0x264, 0x01);
    sensor_write_register(0x265, 0x00);
    sensor_write_register(0x266, 0x00);
    sensor_write_register(0x267, 0x00);
    sensor_write_register(0x268, 0x00);
    sensor_write_register(0x269, 0x00);
    sensor_write_register(0x26A, 0x00);
    sensor_write_register(0x26B, 0x00);
    sensor_write_register(0x26C, 0x00);
    sensor_write_register(0x26D, 0x00);
    sensor_write_register(0x26E, 0x00);
    sensor_write_register(0x26F, 0x00);
    sensor_write_register(0x270, 0x00);
    sensor_write_register(0x271, 0x00);
    sensor_write_register(0x272, 0x00);
    sensor_write_register(0x273, 0x01);
    sensor_write_register(0x274, 0x06);
    sensor_write_register(0x275, 0x07);
    sensor_write_register(0x276, 0x80);
    sensor_write_register(0x277, 0x00);
    sensor_write_register(0x278, 0x40);
    sensor_write_register(0x279, 0x08);
    sensor_write_register(0x27A, 0x00);
    sensor_write_register(0x27B, 0x00);
    sensor_write_register(0x27C, 0x10);
    sensor_write_register(0x27D, 0x00);
    sensor_write_register(0x27E, 0x00);
    sensor_write_register(0x27F, 0x00);
    sensor_write_register(0x280, 0x06);
    sensor_write_register(0x281, 0x19);
    sensor_write_register(0x282, 0x00);
    sensor_write_register(0x283, 0x64);
    sensor_write_register(0x284, 0x00);
    sensor_write_register(0x285, 0x01);
    sensor_write_register(0x286, 0x00);
    sensor_write_register(0x287, 0x00);
    sensor_write_register(0x288, 0x00);
    sensor_write_register(0x289, 0x00);
    sensor_write_register(0x28A, 0x00);
    sensor_write_register(0x28B, 0x00);
    sensor_write_register(0x28C, 0x00);
    sensor_write_register(0x28D, 0x00);
    sensor_write_register(0x28E, 0x00);
    sensor_write_register(0x28F, 0x00);
    sensor_write_register(0x290, 0x00);
    sensor_write_register(0x291, 0x00);
    sensor_write_register(0x292, 0x01);
    sensor_write_register(0x293, 0x01);
    sensor_write_register(0x294, 0x00);
    sensor_write_register(0x295, 0xFF);
    sensor_write_register(0x296, 0x0F);
    sensor_write_register(0x297, 0x00);
    sensor_write_register(0x298, 0x26);
    sensor_write_register(0x299, 0x02);

    sensor_write_register(0x29C, 0x9C);
    sensor_write_register(0x29D, 0x01);
    sensor_write_register(0x29E, 0x39);
    sensor_write_register(0x29F, 0x03);
    sensor_write_register(0x2A0, 0x01);
    sensor_write_register(0x2A1, 0x05);
    sensor_write_register(0x2A2, 0xD0);
    sensor_write_register(0x2A3, 0x07);
    sensor_write_register(0x2A4, 0x00);
    sensor_write_register(0x2A5, 0x02);
    sensor_write_register(0x2A6, 0x0B);
    sensor_write_register(0x2A7, 0x0F);
    sensor_write_register(0x2A8, 0x24);
    sensor_write_register(0x2A9, 0x00);
    sensor_write_register(0x2AA, 0x28);
    sensor_write_register(0x2AB, 0x00);
    sensor_write_register(0x2AC, 0xE8);
    sensor_write_register(0x2AD, 0x04);
    sensor_write_register(0x2AE, 0xEC);
    sensor_write_register(0x2AF, 0x04);
    sensor_write_register(0x2B0, 0x00);
    sensor_write_register(0x2B1, 0x00);
    sensor_write_register(0x2B2, 0x03);
    sensor_write_register(0x2B3, 0x05);
    sensor_write_register(0x2B4, 0x00);
    sensor_write_register(0x2B5, 0x0F);
    sensor_write_register(0x2B6, 0x10);
    sensor_write_register(0x2B7, 0x00);
    sensor_write_register(0x2B8, 0x28);
    sensor_write_register(0x2B9, 0x00);
    sensor_write_register(0x2BA, 0xBF);
    sensor_write_register(0x2BB, 0x07);
    sensor_write_register(0x2BC, 0xCF);
    sensor_write_register(0x2BD, 0x07);
    sensor_write_register(0x2BE, 0xCF);
    sensor_write_register(0x2BF, 0x07);
    sensor_write_register(0x2C0, 0xCF);
    sensor_write_register(0x2C1, 0x07);
    sensor_write_register(0x2C2, 0xD0);
    sensor_write_register(0x2C3, 0x07);
    sensor_write_register(0x2C4, 0x01);
    sensor_write_register(0x2C5, 0x02);
    sensor_write_register(0x2C6, 0x03);
    sensor_write_register(0x2C7, 0x04);
    sensor_write_register(0x2C8, 0x05);
    sensor_write_register(0x2C9, 0x06);
    sensor_write_register(0x2CA, 0x07);
    sensor_write_register(0x2CB, 0x08);
    sensor_write_register(0x2CC, 0x01);
    sensor_write_register(0x2CD, 0x03);

    sensor_write_register(0x2D1, 0x00);
    sensor_write_register(0x2D2, 0x00);
    sensor_write_register(0x2D3, 0x00);
    sensor_write_register(0x2D4, 0x00);
    sensor_write_register(0x2D5, 0x00);
    sensor_write_register(0x2D6, 0x00);
    sensor_write_register(0x2D7, 0x00);
    sensor_write_register(0x2D8, 0x00);
    sensor_write_register(0x2D9, 0x00);
    sensor_write_register(0x2DA, 0x00);
    sensor_write_register(0x2DB, 0x00);
    sensor_write_register(0x2DC, 0x00);
    sensor_write_register(0x2DD, 0x00);
    sensor_write_register(0x2DE, 0x00);
    sensor_write_register(0x2DF, 0x00);
    sensor_write_register(0x2E0, 0x00);
    sensor_write_register(0x2E1, 0x00);
    sensor_write_register(0x2E2, 0x00);
    sensor_write_register(0x2E3, 0x00);
    sensor_write_register(0x2E4, 0x00);
    sensor_write_register(0x2E5, 0x00);
    sensor_write_register(0x2E6, 0x00);
    sensor_write_register(0x2E7, 0x00);
    sensor_write_register(0x2E8, 0x00);
    sensor_write_register(0x2E9, 0x00);
    sensor_write_register(0x2EA, 0x00);
    sensor_write_register(0x2EB, 0x00);
    sensor_write_register(0x2EC, 0x00);
    sensor_write_register(0x2ED, 0x00);
    sensor_write_register(0x2EE, 0x00);
    sensor_write_register(0x2EF, 0x00);
    sensor_write_register(0x2F0, 0x00);
    sensor_write_register(0x2F1, 0x00);
    sensor_write_register(0x2F2, 0x00);
    sensor_write_register(0x2F3, 0x00);
    sensor_write_register(0x2F4, 0x00);
    sensor_write_register(0x2F5, 0x00);
    sensor_write_register(0x2F6, 0x00);
    sensor_write_register(0x2F7, 0x00);
    sensor_write_register(0x2F8, 0x00);
    sensor_write_register(0x2F9, 0x00);
    sensor_write_register(0x2FA, 0x00);
    sensor_write_register(0x2FB, 0x00);
    sensor_write_register(0x2FC, 0x00);
    sensor_write_register(0x2FD, 0x00);
    sensor_write_register(0x2FE, 0x00);
    sensor_write_register(0x2FF, 0x00);

    // chip_id = 0x3
    
    sensor_write_register(0x300, 0x00);
    sensor_write_register(0x301, 0x00);
    sensor_write_register(0x302, 0x00);
    sensor_write_register(0x303, 0x00);
    sensor_write_register(0x304, 0x00);
    sensor_write_register(0x305, 0x00);
    sensor_write_register(0x306, 0x00);
    sensor_write_register(0x307, 0xFA);
    sensor_write_register(0x308, 0xFA);
    sensor_write_register(0x309, 0x41);
    sensor_write_register(0x30A, 0x31);
    sensor_write_register(0x30B, 0x38);
    sensor_write_register(0x30C, 0x04);
    sensor_write_register(0x30D, 0x00);
    sensor_write_register(0x30E, 0x1A);
    sensor_write_register(0x30F, 0x10);
    sensor_write_register(0x310, 0x00);
    sensor_write_register(0x311, 0x00);
    sensor_write_register(0x312, 0x10);
    sensor_write_register(0x313, 0x00);
    sensor_write_register(0x314, 0x00);
    sensor_write_register(0x315, 0x06);
    sensor_write_register(0x316, 0x33);
    sensor_write_register(0x317, 0x0D);
    sensor_write_register(0x318, 0x00);
    sensor_write_register(0x319, 0x00);
    sensor_write_register(0x31A, 0x00);
    sensor_write_register(0x31B, 0x00);
    sensor_write_register(0x31C, 0x00);
    sensor_write_register(0x31D, 0x00);
    sensor_write_register(0x31E, 0x00);
    sensor_write_register(0x31F, 0x00);
    sensor_write_register(0x320, 0x00);
    sensor_write_register(0x321, 0x80);
    sensor_write_register(0x322, 0x0C);
    sensor_write_register(0x323, 0x00);
    sensor_write_register(0x324, 0x00);
    sensor_write_register(0x325, 0x00);
    sensor_write_register(0x326, 0x00);
    sensor_write_register(0x327, 0x00);
    sensor_write_register(0x328, 0x05);
    sensor_write_register(0x329, 0x80);
    sensor_write_register(0x32A, 0x00);
    sensor_write_register(0x32B, 0x00);
    sensor_write_register(0x32C, 0x04);
    sensor_write_register(0x32D, 0x04);
    sensor_write_register(0x32E, 0x00);
    sensor_write_register(0x32F, 0x00);
    sensor_write_register(0x330, 0x9B);
    sensor_write_register(0x331, 0x71);
    sensor_write_register(0x332, 0x33);
    sensor_write_register(0x333, 0x37);
    sensor_write_register(0x334, 0xB3);
    sensor_write_register(0x335, 0x19);
    sensor_write_register(0x336, 0x97);
    sensor_write_register(0x337, 0xB1);
    sensor_write_register(0x338, 0x19);
    sensor_write_register(0x339, 0x01);
    sensor_write_register(0x33A, 0x50);
    sensor_write_register(0x33B, 0x00);
    sensor_write_register(0x33C, 0x35);
    sensor_write_register(0x33D, 0xB0);
    sensor_write_register(0x33E, 0x03);
    sensor_write_register(0x33F, 0xD1);
    sensor_write_register(0x340, 0x71);
    sensor_write_register(0x341, 0x1D);
    sensor_write_register(0x342, 0x00);
    sensor_write_register(0x343, 0x00);
    sensor_write_register(0x344, 0x00);
    sensor_write_register(0x345, 0x00);
    sensor_write_register(0x346, 0x02);
    sensor_write_register(0x347, 0x30);
    sensor_write_register(0x348, 0x00);
    sensor_write_register(0x349, 0x00);
    sensor_write_register(0x34A, 0x00);
    sensor_write_register(0x34B, 0x03);
    sensor_write_register(0x34C, 0x00);
    sensor_write_register(0x34D, 0x02);
    sensor_write_register(0x34E, 0x10);
    sensor_write_register(0x34F, 0xA0);
    sensor_write_register(0x350, 0x00);
    sensor_write_register(0x351, 0x07);
    sensor_write_register(0x352, 0x40);
    sensor_write_register(0x353, 0x80);
    sensor_write_register(0x354, 0x00);
    sensor_write_register(0x355, 0x02);
    sensor_write_register(0x356, 0x50);
    sensor_write_register(0x357, 0x02);
    sensor_write_register(0x358, 0x23);
    sensor_write_register(0x359, 0xE4);
    sensor_write_register(0x35A, 0x45);
    sensor_write_register(0x35B, 0x33);
    sensor_write_register(0x35C, 0x79);
    sensor_write_register(0x35D, 0xD1);
    sensor_write_register(0x35E, 0xCC);
    sensor_write_register(0x35F, 0x2F);
    sensor_write_register(0x360, 0xB6);
    sensor_write_register(0x361, 0xA1);
    sensor_write_register(0x362, 0x17);
    sensor_write_register(0x363, 0xCB);
    sensor_write_register(0x364, 0xE8);
    sensor_write_register(0x365, 0xC5);
    sensor_write_register(0x366, 0x32);
    sensor_write_register(0x367, 0xC0);
    sensor_write_register(0x368, 0xA8);
    sensor_write_register(0x369, 0xC6);
    sensor_write_register(0x36A, 0x5E);
    sensor_write_register(0x36B, 0x20);
    sensor_write_register(0x36C, 0x63);
    sensor_write_register(0x36D, 0x0D);
    sensor_write_register(0x36E, 0x6D);
    sensor_write_register(0x36F, 0x44);
    sensor_write_register(0x370, 0xA6);
    sensor_write_register(0x371, 0x32);
    sensor_write_register(0x372, 0x24);
    sensor_write_register(0x373, 0x50);
    sensor_write_register(0x374, 0xC4);
    sensor_write_register(0x375, 0x2F);
    sensor_write_register(0x376, 0xF4);
    sensor_write_register(0x377, 0x42);
    sensor_write_register(0x378, 0x82);
    sensor_write_register(0x379, 0x13);
    sensor_write_register(0x37A, 0x90);
    sensor_write_register(0x37B, 0x00);
    sensor_write_register(0x37C, 0x10);
    sensor_write_register(0x37D, 0x8A);
    sensor_write_register(0x37E, 0x60);
    sensor_write_register(0x37F, 0xC4);
    sensor_write_register(0x380, 0x2F);
    sensor_write_register(0x381, 0x84);
    sensor_write_register(0x382, 0xF1);
    sensor_write_register(0x383, 0x0B);
    sensor_write_register(0x384, 0xCD);
    sensor_write_register(0x385, 0x70);
    sensor_write_register(0x386, 0x42);
    sensor_write_register(0x387, 0x16);
    sensor_write_register(0x388, 0x00);
    sensor_write_register(0x389, 0x61);
    sensor_write_register(0x38A, 0x0B);
    sensor_write_register(0x38B, 0x29);
    sensor_write_register(0x38C, 0x74);
    sensor_write_register(0x38D, 0x81);
    sensor_write_register(0x38E, 0x10);
    sensor_write_register(0x38F, 0xBA);
    sensor_write_register(0x390, 0x18);
    sensor_write_register(0x391, 0x22);
    sensor_write_register(0x392, 0x11);
    sensor_write_register(0x393, 0xE9);
    sensor_write_register(0x394, 0x60);
    sensor_write_register(0x395, 0x07);
    sensor_write_register(0x396, 0x09);
    sensor_write_register(0x397, 0xF6);
    sensor_write_register(0x398, 0x40);
    sensor_write_register(0x399, 0x02);
    sensor_write_register(0x39A, 0x3C);
    sensor_write_register(0x39B, 0x00);
    sensor_write_register(0x39C, 0x00);
    sensor_write_register(0x39D, 0x00);
    sensor_write_register(0x39E, 0x00);
    sensor_write_register(0x39F, 0x00);
    sensor_write_register(0x3A0, 0x80);
    sensor_write_register(0x3A1, 0x0B);
    sensor_write_register(0x3A2, 0x64);
    sensor_write_register(0x3A3, 0x90);
    sensor_write_register(0x3A4, 0x8D);
    sensor_write_register(0x3A5, 0x6E);
    sensor_write_register(0x3A6, 0x98);
    sensor_write_register(0x3A7, 0x40);
    sensor_write_register(0x3A8, 0x05);
    sensor_write_register(0x3A9, 0xD1);
    sensor_write_register(0x3AA, 0xA8);
    sensor_write_register(0x3AB, 0x86);
    sensor_write_register(0x3AC, 0x09);
    sensor_write_register(0x3AD, 0x54);
    sensor_write_register(0x3AE, 0x10);
    sensor_write_register(0x3AF, 0x8D);
    sensor_write_register(0x3B0, 0x6A);
    sensor_write_register(0x3B1, 0xE8);
    sensor_write_register(0x3B2, 0x82);
    sensor_write_register(0x3B3, 0x17);
    sensor_write_register(0x3B4, 0x1C);
    sensor_write_register(0x3B5, 0x60);
    sensor_write_register(0x3B6, 0xC1);
    sensor_write_register(0x3B7, 0x31);
    sensor_write_register(0x3B8, 0xAE);
    sensor_write_register(0x3B9, 0xD1);
    sensor_write_register(0x3BA, 0x81);
    sensor_write_register(0x3BB, 0x16);
    sensor_write_register(0x3BC, 0x20);
    sensor_write_register(0x3BD, 0x03);
    sensor_write_register(0x3BE, 0x1B);
    sensor_write_register(0x3BF, 0x24);
    sensor_write_register(0x3C0, 0xE0);
    sensor_write_register(0x3C1, 0xC1);
    sensor_write_register(0x3C2, 0x33);
    sensor_write_register(0x3C3, 0xBE);
    sensor_write_register(0x3C4, 0x51);
    sensor_write_register(0x3C5, 0x82);
    sensor_write_register(0x3C6, 0x1E);
    sensor_write_register(0x3C7, 0x40);
    sensor_write_register(0x3C8, 0x03);
    sensor_write_register(0x3C9, 0x1C);
    sensor_write_register(0x3CA, 0x34);
    sensor_write_register(0x3CB, 0xD0);
    sensor_write_register(0x3CC, 0x81);
    sensor_write_register(0x3CD, 0x02);
    sensor_write_register(0x3CE, 0x16);
    sensor_write_register(0x3CF, 0x00);
    sensor_write_register(0x3D0, 0x02);
    sensor_write_register(0x3D1, 0x04);
    sensor_write_register(0x3D2, 0x00);
    sensor_write_register(0x3D3, 0x00);
    sensor_write_register(0x3D4, 0x00);
    sensor_write_register(0x3D5, 0x80);
    sensor_write_register(0x3D6, 0x00);
    sensor_write_register(0x3D7, 0x00);
    sensor_write_register(0x3D8, 0x23);
    sensor_write_register(0x3D9, 0x01);
    sensor_write_register(0x3DA, 0x03);
    sensor_write_register(0x3DB, 0x02);
    sensor_write_register(0x3DC, 0x00);
    sensor_write_register(0x3DD, 0x00);
    sensor_write_register(0x3DE, 0x00);
    sensor_write_register(0x3DF, 0x00);
    sensor_write_register(0x3E0, 0x22);
    sensor_write_register(0x3E1, 0x00);
    sensor_write_register(0x3E2, 0x00);
    sensor_write_register(0x3E3, 0x00);
    sensor_write_register(0x3E4, 0x3F);
    sensor_write_register(0x3E5, 0x17);
    sensor_write_register(0x3E6, 0x15);
    sensor_write_register(0x3E7, 0x00);
    sensor_write_register(0x3E8, 0x00);
    sensor_write_register(0x3E9, 0x00);
    sensor_write_register(0x3EA, 0x00);
    sensor_write_register(0x3EB, 0x00);
    sensor_write_register(0x3EC, 0x00);
    sensor_write_register(0x3ED, 0x00);
    sensor_write_register(0x3EE, 0x00);
    sensor_write_register(0x3EF, 0x00);
    sensor_write_register(0x3F0, 0x00);
    sensor_write_register(0x3F1, 0x00);
    sensor_write_register(0x3F2, 0x00);
    sensor_write_register(0x3F3, 0x00);
    sensor_write_register(0x3F4, 0x00);
    sensor_write_register(0x3F5, 0x00);
    sensor_write_register(0x3F6, 0x00);
    sensor_write_register(0x3F7, 0x00);
    sensor_write_register(0x3F8, 0x00);
    sensor_write_register(0x3F9, 0x00);
    sensor_write_register(0x3FA, 0x00);
    sensor_write_register(0x3FB, 0x00);
    sensor_write_register(0x3FC, 0x00);
    sensor_write_register(0x3FD, 0x00);
    sensor_write_register(0x3FE, 0x00);
    sensor_write_register(0x3FF, 0x00);
    
    // standby cancel
//  sensor_write_register(0x200, 0x30);

    // waiting for internal regular stabilization
    //usleep(200000);
    
    
      // sensor_write_register(0x226, 0x00);
       
       // standby cancel
       sensor_write_register(0x200, 0x30);
    // XVS,XHS output start
    //sensor_write_register(0x229, 0xC0);

    // waiting for image stabilization
    usleep(200000);

    printf("-------Sony IMX222 Sensor Initial OK!-------\n");

}



void sensor_init_720p_60fps()
{
        sensor_write_register(0x200, 0x31); //sensor_write_register(0x200, 0x01)
        sensor_write_register(0x211, 0x00);
        sensor_write_register(0x22D, 0x40);
        sensor_write_register(0x202, 0x01);
        sensor_write_register(0x216, 0xF0);
        sensor_write_register(0x217, 0x00);
        sensor_write_register(0x214, 0x40);
        sensor_write_register(0x215, 0x01);
        sensor_write_register(0x218, 0x40);
        sensor_write_register(0x219, 0x05);
        sensor_write_register(0x2CE, 0x00);
        sensor_write_register(0x2CF, 0x00);
        sensor_write_register(0x2D0, 0x00);
        sensor_write_register(0x29A, 0x4C);
        sensor_write_register(0x29B, 0x04);
        //sensor_write_register(0x212, 0x82);//  sensor_write_register(0x212, 0x80);
        sensor_write_register(0x212, 0x80);
        
        sensor_write_register(0x20F, 0x00);
        sensor_write_register(0x20D, 0x00);
        sensor_write_register(0x208, 0x00);
        sensor_write_register(0x209, 0x00);
        sensor_write_register(0x21E, 0x00);
        sensor_write_register(0x220, 0x3C);
        sensor_write_register(0x221, 0x00);
      //  sensor_write_register(0x222, 0x01); // sensor_write_register(0x222, 0xC0)

        sensor_write_register(0x222, 0xC0);
         
        sensor_write_register(0x205, 0xEE);
        sensor_write_register(0x206, 0x02);
        sensor_write_register(0x203, 0x39);
        sensor_write_register(0x204, 0x03);
        sensor_write_register(0x23B, 0xE0);

            // master mode start
        sensor_write_register(0x22C, 0x00);    //  sensor_write_register(0x22C, 0x01);
    
        sensor_write_register(0x201, 0x00);
    
        sensor_write_register(0x207, 0x00);
    
        sensor_write_register(0x20A, 0x00);
        sensor_write_register(0x20B, 0x00);
        sensor_write_register(0x20C, 0x00);
    
        sensor_write_register(0x20E, 0x00);
        sensor_write_register(0x210, 0x00);
        sensor_write_register(0x213, 0x40);
        sensor_write_register(0x21A, 0xE9);
        sensor_write_register(0x21B, 0x02);
        sensor_write_register(0x21C, 0x50);
        sensor_write_register(0x21D, 0x00);
    
        sensor_write_register(0x21F, 0x31);
    
        sensor_write_register(0x223, 0x08);
        sensor_write_register(0x224, 0x30);
        sensor_write_register(0x225, 0x00);
        sensor_write_register(0x226, 0x80);
        sensor_write_register(0x227, 0x20);
        sensor_write_register(0x228, 0x34);
        sensor_write_register(0x229, 0x63);
        sensor_write_register(0x22A, 0x00);
        sensor_write_register(0x22B, 0x00);
        
        sensor_write_register(0x22E, 0x00);
        sensor_write_register(0x22F, 0x02);
    
        sensor_write_register(0x230, 0x30);
        sensor_write_register(0x231, 0x20);
        sensor_write_register(0x232, 0x00);
        sensor_write_register(0x233, 0x14);
        sensor_write_register(0x234, 0x20);
        sensor_write_register(0x235, 0x60);
        sensor_write_register(0x236, 0x00);
        sensor_write_register(0x237, 0x23);
        sensor_write_register(0x238, 0x01);
        sensor_write_register(0x239, 0x00);
        sensor_write_register(0x23A, 0xA8);
        sensor_write_register(0x23B, 0xE0);
        sensor_write_register(0x23C, 0x06);
        sensor_write_register(0x23D, 0x00);
        sensor_write_register(0x23E, 0x10);
        sensor_write_register(0x23F, 0x00);
        sensor_write_register(0x240, 0x42);
        sensor_write_register(0x241, 0x23);
        sensor_write_register(0x242, 0x3C);
        sensor_write_register(0x243, 0x01);
        sensor_write_register(0x244, 0x00);
        sensor_write_register(0x245, 0x00);
        sensor_write_register(0x246, 0x00);
        sensor_write_register(0x247, 0x00);
        sensor_write_register(0x248, 0x00);
        sensor_write_register(0x249, 0x00);
        sensor_write_register(0x24A, 0x00);
        sensor_write_register(0x24B, 0x00);
        sensor_write_register(0x24C, 0x01);
        sensor_write_register(0x24D, 0x00);
        sensor_write_register(0x24E, 0x01);
        sensor_write_register(0x24F, 0x07);
        sensor_write_register(0x250, 0x10);
        sensor_write_register(0x251, 0x18);
        sensor_write_register(0x252, 0x12);
        sensor_write_register(0x253, 0x00);
        sensor_write_register(0x254, 0x00);
        sensor_write_register(0x255, 0x00);
        sensor_write_register(0x256, 0x00);
        sensor_write_register(0x257, 0x00);
        sensor_write_register(0x258, 0xE0);
        sensor_write_register(0x259, 0x01);
        sensor_write_register(0x25A, 0xE0);
        sensor_write_register(0x25B, 0x01);
        sensor_write_register(0x25C, 0x00);
        sensor_write_register(0x25D, 0x00);
        sensor_write_register(0x25E, 0x00);
        sensor_write_register(0x25F, 0x00);
        sensor_write_register(0x260, 0x00);
        sensor_write_register(0x261, 0x00);
        sensor_write_register(0x262, 0x76);
        sensor_write_register(0x263, 0x00);
        sensor_write_register(0x264, 0x01);
        sensor_write_register(0x265, 0x00);
        sensor_write_register(0x266, 0x00);
        sensor_write_register(0x267, 0x00);
        sensor_write_register(0x268, 0x00);
        sensor_write_register(0x269, 0x00);
        sensor_write_register(0x26A, 0x00);
        sensor_write_register(0x26B, 0x00);
        sensor_write_register(0x26C, 0x00);
        sensor_write_register(0x26D, 0x00);
        sensor_write_register(0x26E, 0x00);
        sensor_write_register(0x26F, 0x00);
        sensor_write_register(0x270, 0x00);
        sensor_write_register(0x271, 0x00);
        sensor_write_register(0x272, 0x00);
        sensor_write_register(0x273, 0x01);
        sensor_write_register(0x274, 0x06);
        sensor_write_register(0x275, 0x07);
        sensor_write_register(0x276, 0x80);
        sensor_write_register(0x277, 0x00);
        sensor_write_register(0x278, 0x40);
        sensor_write_register(0x279, 0x08);
        sensor_write_register(0x27A, 0x00);
        sensor_write_register(0x27B, 0x00);
        sensor_write_register(0x27C, 0x10);
        sensor_write_register(0x27D, 0x00);
        sensor_write_register(0x27E, 0x00);
        sensor_write_register(0x27F, 0x00);
        sensor_write_register(0x280, 0x06);
        sensor_write_register(0x281, 0x19);
        sensor_write_register(0x282, 0x00);
        sensor_write_register(0x283, 0x64);
        sensor_write_register(0x284, 0x00);
        sensor_write_register(0x285, 0x01);
        sensor_write_register(0x286, 0x00);
        sensor_write_register(0x287, 0x00);
        sensor_write_register(0x288, 0x00);
        sensor_write_register(0x289, 0x00);
        sensor_write_register(0x28A, 0x00);
        sensor_write_register(0x28B, 0x00);
        sensor_write_register(0x28C, 0x00);
        sensor_write_register(0x28D, 0x00);
        sensor_write_register(0x28E, 0x00);
        sensor_write_register(0x28F, 0x00);
        sensor_write_register(0x290, 0x00);
        sensor_write_register(0x291, 0x00);
        sensor_write_register(0x292, 0x01);
        sensor_write_register(0x293, 0x01);
        sensor_write_register(0x294, 0x00);
        sensor_write_register(0x295, 0xFF);
        sensor_write_register(0x296, 0x0F);
        sensor_write_register(0x297, 0x00);
        sensor_write_register(0x298, 0x26);
        sensor_write_register(0x299, 0x02);

        sensor_write_register(0x29A, 0x4C); //new
        sensor_write_register(0x29B, 0x04); //new
    
        sensor_write_register(0x29C, 0x9C);
        sensor_write_register(0x29D, 0x01);
        sensor_write_register(0x29E, 0x39);
        sensor_write_register(0x29F, 0x03);
        sensor_write_register(0x2A0, 0x01);
        sensor_write_register(0x2A1, 0x05);
        sensor_write_register(0x2A2, 0xD0);
        sensor_write_register(0x2A3, 0x07);
        sensor_write_register(0x2A4, 0x00);
        sensor_write_register(0x2A5, 0x02);
        sensor_write_register(0x2A6, 0x0B);
        sensor_write_register(0x2A7, 0x0F);
        sensor_write_register(0x2A8, 0x24);
        sensor_write_register(0x2A9, 0x00);
        sensor_write_register(0x2AA, 0x28);
        sensor_write_register(0x2AB, 0x00);
        sensor_write_register(0x2AC, 0xE8);
        sensor_write_register(0x2AD, 0x04);
        sensor_write_register(0x2AE, 0xEC);
        sensor_write_register(0x2AF, 0x04);
        sensor_write_register(0x2B0, 0x00);
        sensor_write_register(0x2B1, 0x00);
        sensor_write_register(0x2B2, 0x03);
        sensor_write_register(0x2B3, 0x05);
        sensor_write_register(0x2B4, 0x00);
        sensor_write_register(0x2B5, 0x0F);
        sensor_write_register(0x2B6, 0x10);
        sensor_write_register(0x2B7, 0x00);
        sensor_write_register(0x2B8, 0x28);
        sensor_write_register(0x2B9, 0x00);
        sensor_write_register(0x2BA, 0xBF);
        sensor_write_register(0x2BB, 0x07);
        sensor_write_register(0x2BC, 0xCF);
        sensor_write_register(0x2BD, 0x07);
        sensor_write_register(0x2BE, 0xCF);
        sensor_write_register(0x2BF, 0x07);
        sensor_write_register(0x2C0, 0xCF);
        sensor_write_register(0x2C1, 0x07);
        sensor_write_register(0x2C2, 0xD0);
        sensor_write_register(0x2C3, 0x07);
        sensor_write_register(0x2C4, 0x01);
        sensor_write_register(0x2C5, 0x02);
        sensor_write_register(0x2C6, 0x03);
        sensor_write_register(0x2C7, 0x04);
        sensor_write_register(0x2C8, 0x05);
        sensor_write_register(0x2C9, 0x06);
        sensor_write_register(0x2CA, 0x07);
        sensor_write_register(0x2CB, 0x08);
        sensor_write_register(0x2CC, 0x01);
        sensor_write_register(0x2CD, 0x03);
    
        sensor_write_register(0x2D1, 0x00);
        sensor_write_register(0x2D2, 0x00);
        sensor_write_register(0x2D3, 0x00);
        sensor_write_register(0x2D4, 0x00);
        sensor_write_register(0x2D5, 0x00);
        sensor_write_register(0x2D6, 0x00);
        sensor_write_register(0x2D7, 0x00);
        sensor_write_register(0x2D8, 0x00);
        sensor_write_register(0x2D9, 0x00);
        sensor_write_register(0x2DA, 0x00);
        sensor_write_register(0x2DB, 0x00);
        sensor_write_register(0x2DC, 0x00);
        sensor_write_register(0x2DD, 0x00);
        sensor_write_register(0x2DE, 0x00);
        sensor_write_register(0x2DF, 0x00);
        sensor_write_register(0x2E0, 0x00);
        sensor_write_register(0x2E1, 0x00);
        sensor_write_register(0x2E2, 0x00);
        sensor_write_register(0x2E3, 0x00);
        sensor_write_register(0x2E4, 0x00);
        sensor_write_register(0x2E5, 0x00);
        sensor_write_register(0x2E6, 0x00);
        sensor_write_register(0x2E7, 0x00);
        sensor_write_register(0x2E8, 0x00);
        sensor_write_register(0x2E9, 0x00);
        sensor_write_register(0x2EA, 0x00);
        sensor_write_register(0x2EB, 0x00);
        sensor_write_register(0x2EC, 0x00);
        sensor_write_register(0x2ED, 0x00);
        sensor_write_register(0x2EE, 0x00);
        sensor_write_register(0x2EF, 0x00);
        sensor_write_register(0x2F0, 0x00);
        sensor_write_register(0x2F1, 0x00);
        sensor_write_register(0x2F2, 0x00);
        sensor_write_register(0x2F3, 0x00);
        sensor_write_register(0x2F4, 0x00);
        sensor_write_register(0x2F5, 0x00);
        sensor_write_register(0x2F6, 0x00);
        sensor_write_register(0x2F7, 0x00);
        sensor_write_register(0x2F8, 0x00);
        sensor_write_register(0x2F9, 0x00);
        sensor_write_register(0x2FA, 0x00);
        sensor_write_register(0x2FB, 0x00);
        sensor_write_register(0x2FC, 0x00);
        sensor_write_register(0x2FD, 0x00);
        sensor_write_register(0x2FE, 0x00);
        sensor_write_register(0x2FF, 0x00);
    
        // chip_id = 0x3
        
        sensor_write_register(0x300, 0x00);
        sensor_write_register(0x301, 0x00);
        sensor_write_register(0x302, 0x00);
        sensor_write_register(0x303, 0x00);
        sensor_write_register(0x304, 0x00);
        sensor_write_register(0x305, 0x00);
        sensor_write_register(0x306, 0x00);
        sensor_write_register(0x307, 0xFA);
        sensor_write_register(0x308, 0xFA);
        sensor_write_register(0x309, 0x41);
        sensor_write_register(0x30A, 0x31);
        sensor_write_register(0x30B, 0x38);
        sensor_write_register(0x30C, 0x04);
        sensor_write_register(0x30D, 0x00);
        sensor_write_register(0x30E, 0x1A);
        sensor_write_register(0x30F, 0x10);
        sensor_write_register(0x310, 0x00);
        sensor_write_register(0x311, 0x00);
        sensor_write_register(0x312, 0x10);
        sensor_write_register(0x313, 0x00);
        sensor_write_register(0x314, 0x00);
        sensor_write_register(0x315, 0x06);
        sensor_write_register(0x316, 0x33);
        sensor_write_register(0x317, 0x0D);
        sensor_write_register(0x318, 0x00);
        sensor_write_register(0x319, 0x00);
        sensor_write_register(0x31A, 0x00);
        sensor_write_register(0x31B, 0x00);
        sensor_write_register(0x31C, 0x00);
        sensor_write_register(0x31D, 0x00);
        sensor_write_register(0x31E, 0x00);
        sensor_write_register(0x31F, 0x00);
        sensor_write_register(0x320, 0x00);
        sensor_write_register(0x321, 0x80);
        sensor_write_register(0x322, 0x0C);
        sensor_write_register(0x323, 0x00);
        sensor_write_register(0x324, 0x00);
        sensor_write_register(0x325, 0x00);
        sensor_write_register(0x326, 0x00);
        sensor_write_register(0x327, 0x00);
        sensor_write_register(0x328, 0x05);
        sensor_write_register(0x329, 0x80);
        sensor_write_register(0x32A, 0x00);
        sensor_write_register(0x32B, 0x00);
        sensor_write_register(0x32C, 0x04);
        sensor_write_register(0x32D, 0x04);
        sensor_write_register(0x32E, 0x00);
        sensor_write_register(0x32F, 0x00);
        sensor_write_register(0x330, 0x9B);
        sensor_write_register(0x331, 0x71);
        sensor_write_register(0x332, 0x33);
        sensor_write_register(0x333, 0x37);
        sensor_write_register(0x334, 0xB3);
        sensor_write_register(0x335, 0x19);
        sensor_write_register(0x336, 0x97);
        sensor_write_register(0x337, 0xB1);
        sensor_write_register(0x338, 0x19);
        sensor_write_register(0x339, 0x01);
        sensor_write_register(0x33A, 0x50);
        sensor_write_register(0x33B, 0x00);
        sensor_write_register(0x33C, 0x35);
        sensor_write_register(0x33D, 0xB0);
        sensor_write_register(0x33E, 0x03);
        sensor_write_register(0x33F, 0xD1);
        sensor_write_register(0x340, 0x71);
        sensor_write_register(0x341, 0x1D);
        sensor_write_register(0x342, 0x00);
        sensor_write_register(0x343, 0x00);
        sensor_write_register(0x344, 0x00);
        sensor_write_register(0x345, 0x00);
        sensor_write_register(0x346, 0x02);
        sensor_write_register(0x347, 0x30);
        sensor_write_register(0x348, 0x00);
        sensor_write_register(0x349, 0x00);
        sensor_write_register(0x34A, 0x00);
        sensor_write_register(0x34B, 0x03);
        sensor_write_register(0x34C, 0x00);
        sensor_write_register(0x34D, 0x02);
        sensor_write_register(0x34E, 0x10);
        sensor_write_register(0x34F, 0xA0);
        sensor_write_register(0x350, 0x00);
        sensor_write_register(0x351, 0x07);
        sensor_write_register(0x352, 0x40);
        sensor_write_register(0x353, 0x80);
        sensor_write_register(0x354, 0x00);
        sensor_write_register(0x355, 0x02);
        sensor_write_register(0x356, 0x50);
        sensor_write_register(0x357, 0x02);
        sensor_write_register(0x358, 0x23);
        sensor_write_register(0x359, 0xE4);
        sensor_write_register(0x35A, 0x45);
        sensor_write_register(0x35B, 0x33);
        sensor_write_register(0x35C, 0x79);
        sensor_write_register(0x35D, 0xD1);
        sensor_write_register(0x35E, 0xCC);
        sensor_write_register(0x35F, 0x2F);
        sensor_write_register(0x360, 0xB6);
        sensor_write_register(0x361, 0xA1);
        sensor_write_register(0x362, 0x17);
        sensor_write_register(0x363, 0xCB);
        sensor_write_register(0x364, 0xE8);
        sensor_write_register(0x365, 0xC5);
        sensor_write_register(0x366, 0x32);
        sensor_write_register(0x367, 0xC0);
        sensor_write_register(0x368, 0xA8);
        sensor_write_register(0x369, 0xC6);
        sensor_write_register(0x36A, 0x5E);
        sensor_write_register(0x36B, 0x20);
        sensor_write_register(0x36C, 0x63);
        sensor_write_register(0x36D, 0x0D);
        sensor_write_register(0x36E, 0x6D);
        sensor_write_register(0x36F, 0x44);
        sensor_write_register(0x370, 0xA6);
        sensor_write_register(0x371, 0x32);
        sensor_write_register(0x372, 0x24);
        sensor_write_register(0x373, 0x50);
        sensor_write_register(0x374, 0xC4);
        sensor_write_register(0x375, 0x2F);
        sensor_write_register(0x376, 0xF4);
        sensor_write_register(0x377, 0x42);
        sensor_write_register(0x378, 0x82);
        sensor_write_register(0x379, 0x13);
        sensor_write_register(0x37A, 0x90);
        sensor_write_register(0x37B, 0x00);
        sensor_write_register(0x37C, 0x10);
        sensor_write_register(0x37D, 0x8A);
        sensor_write_register(0x37E, 0x60);
        sensor_write_register(0x37F, 0xC4);
        sensor_write_register(0x380, 0x2F);
        sensor_write_register(0x381, 0x84);
        sensor_write_register(0x382, 0xF1);
        sensor_write_register(0x383, 0x0B);
        sensor_write_register(0x384, 0xCD);
        sensor_write_register(0x385, 0x70);
        sensor_write_register(0x386, 0x42);
        sensor_write_register(0x387, 0x16);
        sensor_write_register(0x388, 0x00);
        sensor_write_register(0x389, 0x61);
        sensor_write_register(0x38A, 0x0B);
        sensor_write_register(0x38B, 0x29);
        sensor_write_register(0x38C, 0x74);
        sensor_write_register(0x38D, 0x81);
        sensor_write_register(0x38E, 0x10);
        sensor_write_register(0x38F, 0xBA);
        sensor_write_register(0x390, 0x18);
        sensor_write_register(0x391, 0x22);
        sensor_write_register(0x392, 0x11);
        sensor_write_register(0x393, 0xE9);
        sensor_write_register(0x394, 0x60);
        sensor_write_register(0x395, 0x07);
        sensor_write_register(0x396, 0x09);
        sensor_write_register(0x397, 0xF6);
        sensor_write_register(0x398, 0x40);
        sensor_write_register(0x399, 0x02);
        sensor_write_register(0x39A, 0x3C);
        sensor_write_register(0x39B, 0x00);
        sensor_write_register(0x39C, 0x00);
        sensor_write_register(0x39D, 0x00);
        sensor_write_register(0x39E, 0x00);
        sensor_write_register(0x39F, 0x00);
        sensor_write_register(0x3A0, 0x80);
        sensor_write_register(0x3A1, 0x0B);
        sensor_write_register(0x3A2, 0x64);
        sensor_write_register(0x3A3, 0x90);
        sensor_write_register(0x3A4, 0x8D);
        sensor_write_register(0x3A5, 0x6E);
        sensor_write_register(0x3A6, 0x98);
        sensor_write_register(0x3A7, 0x40);
        sensor_write_register(0x3A8, 0x05);
        sensor_write_register(0x3A9, 0xD1);
        sensor_write_register(0x3AA, 0xA8);
        sensor_write_register(0x3AB, 0x86);
        sensor_write_register(0x3AC, 0x09);
        sensor_write_register(0x3AD, 0x54);
        sensor_write_register(0x3AE, 0x10);
        sensor_write_register(0x3AF, 0x8D);
        sensor_write_register(0x3B0, 0x6A);
        sensor_write_register(0x3B1, 0xE8);
        sensor_write_register(0x3B2, 0x82);
        sensor_write_register(0x3B3, 0x17);
        sensor_write_register(0x3B4, 0x1C);
        sensor_write_register(0x3B5, 0x60);
        sensor_write_register(0x3B6, 0xC1);
        sensor_write_register(0x3B7, 0x31);
        sensor_write_register(0x3B8, 0xAE);
        sensor_write_register(0x3B9, 0xD1);
        sensor_write_register(0x3BA, 0x81);
        sensor_write_register(0x3BB, 0x16);
        sensor_write_register(0x3BC, 0x20);
        sensor_write_register(0x3BD, 0x03);
        sensor_write_register(0x3BE, 0x1B);
        sensor_write_register(0x3BF, 0x24);
        sensor_write_register(0x3C0, 0xE0);
        sensor_write_register(0x3C1, 0xC1);
        sensor_write_register(0x3C2, 0x33);
        sensor_write_register(0x3C3, 0xBE);
        sensor_write_register(0x3C4, 0x51);
        sensor_write_register(0x3C5, 0x82);
        sensor_write_register(0x3C6, 0x1E);
        sensor_write_register(0x3C7, 0x40);
        sensor_write_register(0x3C8, 0x03);
        sensor_write_register(0x3C9, 0x1C);
        sensor_write_register(0x3CA, 0x34);
        sensor_write_register(0x3CB, 0xD0);
        sensor_write_register(0x3CC, 0x81);
        sensor_write_register(0x3CD, 0x02);
        sensor_write_register(0x3CE, 0x16);
        sensor_write_register(0x3CF, 0x00);
        sensor_write_register(0x3D0, 0x02);
        sensor_write_register(0x3D1, 0x04);
        sensor_write_register(0x3D2, 0x00);
        sensor_write_register(0x3D3, 0x00);
        sensor_write_register(0x3D4, 0x00);
        sensor_write_register(0x3D5, 0x80);
        sensor_write_register(0x3D6, 0x00);
        sensor_write_register(0x3D7, 0x00);
        sensor_write_register(0x3D8, 0x23);
        sensor_write_register(0x3D9, 0x01);
        sensor_write_register(0x3DA, 0x03);
        sensor_write_register(0x3DB, 0x02);
        sensor_write_register(0x3DC, 0x00);
        sensor_write_register(0x3DD, 0x00);
        sensor_write_register(0x3DE, 0x00);
        sensor_write_register(0x3DF, 0x00);
        sensor_write_register(0x3E0, 0x22);
        sensor_write_register(0x3E1, 0x00);
        sensor_write_register(0x3E2, 0x00);
        sensor_write_register(0x3E3, 0x00);
        sensor_write_register(0x3E4, 0x3F);
        sensor_write_register(0x3E5, 0x17);
        sensor_write_register(0x3E6, 0x15);
        sensor_write_register(0x3E7, 0x00);
        sensor_write_register(0x3E8, 0x00);
        sensor_write_register(0x3E9, 0x00);
        sensor_write_register(0x3EA, 0x00);
        sensor_write_register(0x3EB, 0x00);
        sensor_write_register(0x3EC, 0x00);
        sensor_write_register(0x3ED, 0x00);
        sensor_write_register(0x3EE, 0x00);
        sensor_write_register(0x3EF, 0x00);
        sensor_write_register(0x3F0, 0x00);
        sensor_write_register(0x3F1, 0x00);
        sensor_write_register(0x3F2, 0x00);
        sensor_write_register(0x3F3, 0x00);
        sensor_write_register(0x3F4, 0x00);
        sensor_write_register(0x3F5, 0x00);
        sensor_write_register(0x3F6, 0x00);
        sensor_write_register(0x3F7, 0x00);
        sensor_write_register(0x3F8, 0x00);
        sensor_write_register(0x3F9, 0x00);
        sensor_write_register(0x3FA, 0x00);
        sensor_write_register(0x3FB, 0x00);
        sensor_write_register(0x3FC, 0x00);
        sensor_write_register(0x3FD, 0x00);
        sensor_write_register(0x3FE, 0x00);
        sensor_write_register(0x3FF, 0x00);
                   
        // standby cancel
        sensor_write_register(0x200, 0x30);
        // XVS,XHS output start
        //sensor_write_register(0x229, 0xC0);
    
        // waiting for image stabilization
        usleep(200000);

	printf("-------Sony IMX222 Sensor Initial OK!-------\n");
}
