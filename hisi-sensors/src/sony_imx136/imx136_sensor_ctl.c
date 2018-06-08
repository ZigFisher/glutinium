/******************************************************************************

  Copyright (C), 2001-2013, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : sony136_sensor_ctl.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/06/19
  Description   : Sony IMX136 sensor driver
  History       :
  1.Date        : 2013/06/19
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

#include "hi_spi.h"

static int g_fd = -1;

int sensor_spi_init(void)
{
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

//#define HD1080p60fps_12bitLVDS_4ch_Linear
//#define HD1080p30fps_12bitLVDS_4ch_BuiltInWDR
//#define HD720p30fps_12bitLVDS_4ch_Linear
//#define HD720p60fps_12bitLVDS_4ch_Linear
#define HD1080p30fps_12bitLVDS_4ch_Linear

void sensor_init()
{
    /* 1. sensor spi init */
    sensor_spi_init();
#if 0   /* test: 12bit crop */
    sensor_write_register(0x200, 0x01);     /* Standby */
    usleep(200000);

    sensor_write_register(0x205, 0x01);     /* 12 bit */
    sensor_write_register(0x206, 0x00);     /* All-pix scan mode */
    sensor_write_register(0x207, 0x40);     /* crop mode */
    sensor_write_register(0x209, 0x01);     /* Frame rate (data rate) setting: 30fps */
    sensor_write_register(0x20A, 0xF0);     /* Black level offset */
    sensor_write_register(0x218, 0x65);     /* Vertical span[7:0] */
    sensor_write_register(0x219, 0x04);     /* Vertical span[15:8] */
    sensor_write_register(0x21B, 0x30);     /* Horizontal span[7:0] */
    sensor_write_register(0x21C, 0x11);     /* Horizontal span[15:8] */
    sensor_write_register(0x220, 0x08);     /* Shutter[7:0] */
    sensor_write_register(0x221, 0x00);     /* Shutter[15:8] */

    sensor_write_register(0x238, 0x08);
    sensor_write_register(0x239, 0x00);
    sensor_write_register(0x23A, 0x38);
    sensor_write_register(0x23B, 0x04);
    sensor_write_register(0x23C, 0x08);
    sensor_write_register(0x23D, 0x00);
    sensor_write_register(0x23E, 0x7F);
    sensor_write_register(0x23F, 0x07);
    
    sensor_write_register(0x244, 0xE1);     /* Serial low-voltage LVDS 4ch DDR output, 12bit */
    sensor_write_register(0x25B, 0x00);     /* INCK setting0 */
    sensor_write_register(0x25C, 0x30);     /* INCK setting1 */
    sensor_write_register(0x25D, 0x04);     /* INCK setting2 */
    sensor_write_register(0x25E, 0x30);     /* INCK setting3 */
    sensor_write_register(0x25F, 0x04);     /* INCK setting4 */

    sensor_write_register(0x30F, 0x0E);     /* */
    sensor_write_register(0x316, 0x02);     /* */

    sensor_write_register(0x436, 0x71);     /* */
    sensor_write_register(0x439, 0xF1);     /* */
    sensor_write_register(0x441, 0xF2);     /* */
    sensor_write_register(0x442, 0x21);     /* */
    sensor_write_register(0x443, 0x21);     /* */
    sensor_write_register(0x448, 0xF2);     /* */
    sensor_write_register(0x449, 0x21);     /* */
    sensor_write_register(0x44A, 0x21);     /* */
    sensor_write_register(0x452, 0x01);     /* */
    sensor_write_register(0x454, 0xB1);     /* */
    /* waiting for image stabilization */
    usleep(200000);
    sensor_write_register(0x200, 0x00);     /* release standy */
    usleep(200000);
    sensor_write_register(0x202, 0x00);     /* Master mode operation start */
    usleep(200000);
    sensor_write_register(0x249, 0x0A);     /* HSYNC and VSYNC output */
    usleep(200000);
#else    
/* default: "IMX136 Register settingHD1080p 30fps 12bitLVDS serial 4chLinear mode INCK=37.125MHz" */
    sensor_write_register(0x200, 0x01);     /* Standby */
    usleep(200000);

    sensor_write_register(0x205, 0x01);     /* 12 bit */
    sensor_write_register(0x206, 0x00);     /* All-pix scan mode */
    sensor_write_register(0x207, 0x10);     /* 1080p mode */
    sensor_write_register(0x209, 0x01);     /* Frame rate (data rate) setting: 30fps */
    sensor_write_register(0x20A, 0xF0);     /* Black level offset */
    sensor_write_register(0x218, 0x65);     /* Vertical span[7:0] */
    sensor_write_register(0x219, 0x04);     /* Vertical span[15:8] */
    sensor_write_register(0x21B, 0x98);     /* Horizontal span[7:0] */
    sensor_write_register(0x21C, 0x08);     /* Horizontal span[15:8] */
    sensor_write_register(0x220, 0x08);     /* Shutter[7:0] */
    sensor_write_register(0x221, 0x00);     /* Shutter[15:8] */
    sensor_write_register(0x244, 0xE1);     /* Serial low-voltage LVDS 4ch DDR output, 12bit */
    sensor_write_register(0x25B, 0x00);     /* INCK setting0 */
    sensor_write_register(0x25C, 0x30);     /* INCK setting1 */
    sensor_write_register(0x25D, 0x04);     /* INCK setting2 */
    sensor_write_register(0x25E, 0x30);     /* INCK setting3 */
    sensor_write_register(0x25F, 0x04);     /* INCK setting4 */

    sensor_write_register(0x30F, 0x0E);     /* */
    sensor_write_register(0x316, 0x02);     /* */

    sensor_write_register(0x436, 0x71);     /* */
    sensor_write_register(0x439, 0xF1);     /* */
    sensor_write_register(0x441, 0xF2);     /* */
    sensor_write_register(0x442, 0x21);     /* */
    sensor_write_register(0x443, 0x21);     /* */
    sensor_write_register(0x448, 0xF2);     /* */
    sensor_write_register(0x449, 0x21);     /* */
    sensor_write_register(0x44A, 0x21);     /* */
    sensor_write_register(0x452, 0x01);     /* */
    sensor_write_register(0x454, 0xB1);     /* */

#ifdef HD1080p30fps_12bitLVDS_4ch_Linear /* default config */
    sensor_write_register(0x207, 0x10);     /* 1080p mode */
    sensor_write_register(0x209, 0x01);     /* Frame rate (data rate) setting: 30fps */
    sensor_write_register(0x20C, 0x00);     /* WDR only */
    sensor_write_register(0x20F, 0x01);     /* WDR only */
    sensor_write_register(0x210, 0x01);     /* WDR only */
    sensor_write_register(0x212, 0xF0);     /* WDR only */
    sensor_write_register(0x218, 0x65);     /* Vertical span[7:0] */
    sensor_write_register(0x219, 0x04);     /* Vertical span[15:8] */
    sensor_write_register(0x21B, 0x98);     /* Horizontal span[7:0] */
    sensor_write_register(0x21C, 0x08);     /* Horizontal span[15:8] */
    sensor_write_register(0x220, 0x08);     /* Shutter[7:0] */
    sensor_write_register(0x221, 0x00);     /* Shutter[15:8] */
    sensor_write_register(0x265, 0x20);     /* WDR only */
    sensor_write_register(0x286, 0x01);     /* WDR only */
    sensor_write_register(0x2CF, 0xD1);     /* WDR only */
    sensor_write_register(0x2D0, 0x1B);     /* WDR only */
    sensor_write_register(0x2D2, 0x5F);     /* WDR only */
    sensor_write_register(0x2D3, 0x00);     /* WDR only */
    printf("-------Sony IMX136 Sensor 1080P30fps Initial OK!-------\n");
#endif

#ifdef HD1080p60fps_12bitLVDS_4ch_Linear
    sensor_write_register(0x207, 0x10);     /* 1080p mode */
    sensor_write_register(0x209, 0x00);     /* Frame rate (data rate) setting: 60fps */
    sensor_write_register(0x20C, 0x00);     /* WDR only */
    sensor_write_register(0x20F, 0x01);     /* WDR only */
    sensor_write_register(0x210, 0x01);     /* WDR only */
    sensor_write_register(0x212, 0xF0);     /* WDR only */
    sensor_write_register(0x218, 0x65);     /* Vertical span[7:0] */
    sensor_write_register(0x219, 0x04);     /* Vertical span[15:8] */
    sensor_write_register(0x21B, 0x4C);     /* Horizontal span[7:0] */
    sensor_write_register(0x21C, 0x04);     /* Horizontal span[15:8] */
    sensor_write_register(0x220, 0x08);     /* Shutter[7:0] */
    sensor_write_register(0x221, 0x00);     /* Shutter[15:8] */
    sensor_write_register(0x265, 0x20);     /* WDR only */
    sensor_write_register(0x286, 0x01);     /* WDR only */
    sensor_write_register(0x2CF, 0xD1);     /* WDR only */
    sensor_write_register(0x2D0, 0x1B);     /* WDR only */
    sensor_write_register(0x2D2, 0x5F);     /* WDR only */
    sensor_write_register(0x2D3, 0x00);     /* WDR only */
    printf("-------Sony IMX136 Sensor 1080P60fps Initial OK!-------\n");
#endif

#ifdef HD1080p30fps_12bitLVDS_4ch_BuiltInWDR
    sensor_write_register(0x207, 0x10);     /* 1080p mode */
    sensor_write_register(0x209, 0x00);     /* Frame rate (data rate) setting: 30fps */
    sensor_write_register(0x20C, 0x00);     /* WDSEL */
    sensor_write_register(0x20F, 0x05);     /* WDR */
    sensor_write_register(0x210, 0x00);     /* WDR */
    sensor_write_register(0x212, 0x2D);     /* WDR */
    sensor_write_register(0x218, 0x65);     /* Vertical span[7:0] */
    sensor_write_register(0x219, 0x04);     /* Vertical span[15:8] */
    sensor_write_register(0x21B, 0x30);     /* Horizontal span[7:0] */
    sensor_write_register(0x21C, 0x11);     /* Horizontal span[15:8] */
    sensor_write_register(0x220, 0x1F);     /* Shutter[7:0] */
    sensor_write_register(0x221, 0x04);     /* Shutter[15:8] */
    sensor_write_register(0x223, 0x05);     /* Shutter2[7:0] */
    sensor_write_register(0x265, 0x00);     /* WDR */
    sensor_write_register(0x286, 0x10);     /* WDR */
    sensor_write_register(0x2CF, 0xE1);     /* WDR */
    sensor_write_register(0x2D0, 0x30);     /* WDR */
    sensor_write_register(0x2D2, 0xC4);     /* WDR */
    sensor_write_register(0x2D3, 0x01);     /* WDR */
    printf("-------Sony IMX136 Sensor 1080P30fps Built-In WDR Initial OK!-------\n");
#endif

#ifdef HD720p30fps_12bitLVDS_4ch_Linear
    sensor_write_register(0x207, 0x20);     /* 720p mode */
    sensor_write_register(0x209, 0x02);     /* Frame rate (data rate) setting: 30fps */
    sensor_write_register(0x20C, 0x00);     /* WDR only */
    sensor_write_register(0x20F, 0x01);     /* WDR only */
    sensor_write_register(0x210, 0x01);     /* WDR only */
    sensor_write_register(0x212, 0xF0);     /* WDR only */
    sensor_write_register(0x218, 0xEE);     /* Vertical span[7:0] */
    sensor_write_register(0x219, 0x02);     /* Vertical span[15:8] */
    sensor_write_register(0x21B, 0xE4);     /* Horizontal span[7:0] */
    sensor_write_register(0x21C, 0x0C);     /* Horizontal span[15:8] */
    sensor_write_register(0x220, 0x08);     /* Shutter[7:0] */
    sensor_write_register(0x221, 0x00);     /* Shutter[15:8] */
    sensor_write_register(0x265, 0x20);     /* WDR only */
    sensor_write_register(0x286, 0x01);     /* WDR only */
    sensor_write_register(0x2CF, 0xD1);     /* WDR only */
    sensor_write_register(0x2D0, 0x1B);     /* WDR only */
    sensor_write_register(0x2D2, 0x5F);     /* WDR only */
    sensor_write_register(0x2D3, 0x00);     /* WDR only */
    printf("-------Sony IMX136 Sensor 720P30fps Initial OK!-------\n");
#endif

#ifdef HD720p60fps_12bitLVDS_4ch_Linear
    sensor_write_register(0x207, 0x20);     /* 720p mode */
    sensor_write_register(0x209, 0x01);     /* Frame rate (data rate) setting: 60fps */
    sensor_write_register(0x20C, 0x00);     /* WDR only */
    sensor_write_register(0x20F, 0x01);     /* WDR only */
    sensor_write_register(0x210, 0x01);     /* WDR only */
    sensor_write_register(0x212, 0xF0);     /* WDR only */
    sensor_write_register(0x218, 0xEE);     /* Vertical span[7:0] */
    sensor_write_register(0x219, 0x02);     /* Vertical span[15:8] */
    sensor_write_register(0x21B, 0x72);     /* Horizontal span[7:0] */
    sensor_write_register(0x21C, 0x06);     /* Horizontal span[15:8] */
    sensor_write_register(0x220, 0x08);     /* Shutter[7:0] */
    sensor_write_register(0x221, 0x00);     /* Shutter[15:8] */
    sensor_write_register(0x265, 0x20);     /* WDR only */
    sensor_write_register(0x286, 0x01);     /* WDR only */
    sensor_write_register(0x2CF, 0xD1);     /* WDR only */
    sensor_write_register(0x2D0, 0x1B);     /* WDR only */
    sensor_write_register(0x2D2, 0x5F);     /* WDR only */
    sensor_write_register(0x2D3, 0x00);     /* WDR only */
    printf("-------Sony IMX136 Sensor 720P60fps Initial OK!-------\n");
#endif

    /* waiting for image stabilization */
    usleep(200000);
    sensor_write_register(0x200, 0x00);     /* release standy */
    usleep(200000);
    sensor_write_register(0x202, 0x00);     /* Master mode operation start */
    usleep(200000);
    sensor_write_register(0x249, 0x0A);     /* HSYNC and VSYNC output */
    usleep(200000);
#endif
}

void sensor_exit()
{
    sensor_spi_exit();

    return;
}

