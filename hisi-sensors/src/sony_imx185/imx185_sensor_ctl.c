/******************************************************************************

  Copyright (C), 2001-2013, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : imx185_sensor_ctl.c
  Version       : Initial Draft
  Author        : Hisilicon BVT ISP group
  Created       : 2014/05/22
  Description   : Sony IMX185 sensor driver
  History       :
  1.Date        : 2014/05/22
  Author        : w00278455
  Modification  : Created file

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
extern WDR_MODE_E genSensorMode;
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

static void delay_ms(int ms) { 
    usleep(ms*1000);
}

void sensor_prog(int* rom) 
{
    int i = 0;
    while (1) {
        int lookup = rom[i++];
        int addr = (lookup >> 16) & 0xFFFF;
        int data = lookup & 0xFFFF;
        if (addr == 0xFFFE) {
            delay_ms(data);
        } else if (addr == 0xFFFF) {
            return;
        } else {
            sensor_write_register(addr, data);
        }
    }
}

void setup_sensor(int isp_mode)
{
}

void sensor_wdr_init();
void sensor_linear_1080p30_init();

void sensor_init()
{  
    /* 1. sensor spi init */
    sensor_spi_init();
     /* When sensor first init, config all registers */
    if (HI_FALSE == bSensorInit) 
    {
        if(WDR_MODE_BUILT_IN == genSensorMode)
        {
            sensor_wdr_init();
        }
        else
        {
            sensor_linear_1080p30_init();   /* SENSOR_1080P_30FPS_MODE */
        }
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else 
    {
        if(WDR_MODE_BUILT_IN == genSensorMode)
        {
            sensor_wdr_init();
        }
        else
        {
            sensor_linear_1080p30_init();   /* SENSOR_1080P_30FPS_MODE */
        }       
    }

    return ;
}

void sensor_exit()
{
    sensor_spi_exit();

    return;
}

void sensor_linear_1080p30_init()
{
    /* imx185 1080p30 */    
    sensor_write_register (0x200, 0x01); /* standby */

    sensor_write_register (0x205, 0x01); /* ADBIT=1(12-bit), STD12EN=0*/
    sensor_write_register (0x206, 0x00); /* MODE: All-pix scan */
    sensor_write_register (0x207, 0x10); /* WINMODE: HD 1080p */
    sensor_write_register (0x209, 0x02); /* FRSEL[1:0] 2h:25fps;1h:30fps*/
    sensor_write_register (0x218, 0x65); /* VMAX[7:0] */
    sensor_write_register (0x219, 0x04); /* VMAX[15:8] */
    sensor_write_register (0x21a, 0x00); /* VMAX[16] */
    sensor_write_register (0x21b, 0x98); /* HMAX[7:0] */
    sensor_write_register (0x21c, 0x08); /* HMAX[15:8] */
    sensor_write_register (0x244, 0xE1); /* ODBIT=1, OPORTSEL=0xE :CSI-2 */

    /*diference with Built-in WDR*/
    sensor_write_register (0x20C, 0x00); /* WDMODE 2h:Built-in WDR */
    sensor_write_register (0x20F, 0x01); /* WDC_CMPEN 4h:Output compressed */
    sensor_write_register (0x210, 0x39); /*  */
    sensor_write_register (0x212, 0x50); /*  */
    sensor_write_register (0x21E, 0x01); /*  */
    sensor_write_register (0x265, 0x20); /*  */
    sensor_write_register (0x284, 0x00); /*  */
    sensor_write_register (0x286, 0x01); /*  */
    sensor_write_register (0x2CF, 0xD1); /*  */
    sensor_write_register (0x2D0, 0x1B); /*  */
    sensor_write_register (0x2D2, 0x5F); /*  */
    sensor_write_register (0x2D3, 0x00); /*  */

    /*before not set*/
    sensor_write_register (0x31D, 0x0A);
    sensor_write_register (0x323, 0x0F);
    sensor_write_register (0x347, 0x87);
    sensor_write_register (0x3E1, 0x9E);
    sensor_write_register (0x3E2, 0x01);
    sensor_write_register (0x3E5, 0x05);
    sensor_write_register (0x3E6, 0x05); 
    sensor_write_register (0x3E7, 0x3A);
    sensor_write_register (0x3E8, 0x3A);

    sensor_write_register (0x503, 0x10); /* REPETITION=1  */
    sensor_write_register (0x505, 0x03); /* PHYSICAL Lane NUM 3h:4 lanes; 2h:2 lanes */ 
    sensor_write_register (0x514, 0x08); /* OB_SIZE_V[5:0]*/
    sensor_write_register (0x515, 0x01); /* NULL0_SIZE_V[5:0]*/
    sensor_write_register (0x516, 0x04); /* NULL1_SIZE_V[5:0]*/
    sensor_write_register (0x517, 0x04); /* NULL2_SIZE_V[5:0]*/
    sensor_write_register (0x518, 0x49); /* PIC_SIZE_V[7:0]*/
    sensor_write_register (0x519, 0x04); /* PIC_SIZE_V[11:8]*/
    sensor_write_register (0x52c, 0x30); /* THSEXIT: Global Timing Setting30*/
    sensor_write_register (0x52d, 0x20); /* TCLKPRE: Global Timing Setting*/
    sensor_write_register (0x52e, 0x03); /* TLPXESC*/
    sensor_write_register (0x53e, 0x0c); /* CSI_DT_FMT[7:0]*/   /* CSI_DT_FMT=0x0c0c 12bit CSI_DT_FMT=0x0a0a 10bit*/
    sensor_write_register (0x53f, 0x0c); /* CSI_DT_FMT[15:8]*/
    sensor_write_register (0x540, 0x03); /* CSI_LANE_MODE*/
    sensor_write_register (0x543, 0x58); /* TCLK_POST*/
    sensor_write_register (0x544, 0x10); /* THS_PREPARE 10*/
    sensor_write_register (0x545, 0x30); /* THS_ZERO_MIN 30*/
    sensor_write_register (0x546, 0x18); /* THS_TRAIL 18*/
    sensor_write_register (0x547, 0x10); /* TCLK_TRAIL_MIN 10*/
    sensor_write_register (0x548, 0x10); /* TCLK_PREPARE 10*/
    sensor_write_register (0x549, 0x48); /* TCLK_ZERO 48*/
    sensor_write_register (0x54A, 0x28); /* TCPX*/
    
    
    /* INCK , CSI-2 Serial output (INCK=37.125MHz) */
    sensor_write_register (0x25C, 0x20); /* INCKSEL1*/
    sensor_write_register (0x25D, 0x00); /* INCKSEL2*/
    sensor_write_register (0x25E, 0x18); /* INCKSEL3*/
    sensor_write_register (0x25F, 0x00); /* INCKSEL4*/
    sensor_write_register (0x263, 0x74); /* INCKSEL574h*/
    sensor_write_register (0x541, 0x20); /* INCK_FREQ[7:0]*/
    sensor_write_register (0x542, 0x25); /* INCK_FREQ[15:8]*/
    sensor_write_register (0x54E, 0xb4); /* INCK_FREQ2[7:0]b4*/
    sensor_write_register (0x54F, 0x01); /* INCK_FREQ2[10:8]01*/

    /*gain, black level, exposure, etc.*/
    sensor_write_register (0x20A, 0xF0); /* BLKLEVEL[7:0]*/
    sensor_write_register (0x20B, 0x00); /* BLKLEVEL[8]*/
    sensor_write_register (0x220, 0x0A); /* SHS1[7:0]*/
    sensor_write_register (0x221, 0x00); /* SHS1[15:8]*/
    sensor_write_register (0x222, 0x00); /* SHS1[16]*/
    sensor_write_register (0x214, 0x34); /* GAIN*/
    
    /* registers must be changed */
    sensor_write_register (0x403, 0xC8);
    sensor_write_register (0x407, 0x54);
    sensor_write_register (0x413, 0x16);
    sensor_write_register (0x415, 0xF6);
    sensor_write_register (0x41A, 0x14);
    sensor_write_register (0x41B, 0x51);
    sensor_write_register (0x429, 0xE7);
    sensor_write_register (0x42A, 0xF0);
    sensor_write_register (0x42B, 0x10);
    sensor_write_register (0x431, 0xE7);
    sensor_write_register (0x432, 0xF0);
    sensor_write_register (0x433, 0x10);
    sensor_write_register (0x43C, 0xE8);
    sensor_write_register (0x43D, 0x70);
    sensor_write_register (0x443, 0x08);
    sensor_write_register (0x444, 0xE1);
    sensor_write_register (0x445, 0x10);
    sensor_write_register (0x447, 0xE7);
    sensor_write_register (0x448, 0x60);
    sensor_write_register (0x449, 0x1E);
    sensor_write_register (0x44B, 0x00);
    sensor_write_register (0x44C, 0x41);
    sensor_write_register (0x450, 0x30);
    sensor_write_register (0x451, 0x0A);
    sensor_write_register (0x452, 0xFF);
    sensor_write_register (0x453, 0xFF);
    sensor_write_register (0x454, 0xFF);
    sensor_write_register (0x455, 0x02);
    sensor_write_register (0x457, 0xF0);
    sensor_write_register (0x45A, 0xA6);
    sensor_write_register (0x45D, 0x14);
    sensor_write_register (0x45E, 0x51);
    sensor_write_register (0x460, 0x00);
    sensor_write_register (0x461, 0x61);
    sensor_write_register (0x466, 0x30);
    sensor_write_register (0x467, 0x05);
    sensor_write_register (0x475, 0xE7);
    sensor_write_register (0x481, 0xEA);
    sensor_write_register (0x482, 0x70);
    sensor_write_register (0x485, 0xFF);
    sensor_write_register (0x48A, 0xF0);
    sensor_write_register (0x48D, 0xB6);
    sensor_write_register (0x48E, 0x40);
    sensor_write_register (0x490, 0x42);
    sensor_write_register (0x491, 0x51);
    sensor_write_register (0x492, 0x1E);
    sensor_write_register (0x494, 0xC4);
    sensor_write_register (0x495, 0x20);
    sensor_write_register (0x497, 0x50);
    sensor_write_register (0x498, 0x31);
    sensor_write_register (0x499, 0x1F);
    sensor_write_register (0x49B, 0xC0);
    sensor_write_register (0x49C, 0x60);
    sensor_write_register (0x49E, 0x4C);
    sensor_write_register (0x49F, 0x71);
    sensor_write_register (0x4A0, 0x1F);
    sensor_write_register (0x4A2, 0xB6);
    sensor_write_register (0x4A3, 0xC0);
    sensor_write_register (0x4A4, 0x0B);
    sensor_write_register (0x4A9, 0x24);
    sensor_write_register (0x4AA, 0x41);
    sensor_write_register (0x4B0, 0x25);
    sensor_write_register (0x4B1, 0x51);
    sensor_write_register (0x4B7, 0x1C);
    sensor_write_register (0x4B8, 0xC1);
    sensor_write_register (0x4B9, 0x12);
    sensor_write_register (0x4BE, 0x1D);
    sensor_write_register (0x4BF, 0xD1);
    sensor_write_register (0x4C0, 0x12);
    sensor_write_register (0x4C2, 0xA8);
    sensor_write_register (0x4C3, 0xC0);
    sensor_write_register (0x4C4, 0x0A);
    sensor_write_register (0x4C5, 0x1E);
    sensor_write_register (0x4C6, 0x21);
    sensor_write_register (0x4C9, 0xB0);
    sensor_write_register (0x4CA, 0x40);
    sensor_write_register (0x4CC, 0x26);
    sensor_write_register (0x4CD, 0xA1);
    sensor_write_register (0x4D0, 0xB6);
    sensor_write_register (0x4D1, 0xC0);
    sensor_write_register (0x4D2, 0x0B);
    sensor_write_register (0x4D4, 0xE2);
    sensor_write_register (0x4D5, 0x40);
    sensor_write_register (0x4D8, 0x4E);
    sensor_write_register (0x4D9, 0xA1);
    sensor_write_register (0x4EC, 0xF0);
    
    sensor_write_register (0x200, 0x00); /* standby */
    sensor_write_register (0x202, 0x00); /* master mode start */
    sensor_write_register (0x249, 0x0A); /* XVSOUTSEL XHSOUTSEL */

    printf("-------Sony IMX185 Sensor 1080p30 Initial OK!-------\n");

    bSensorInit = HI_TRUE;

}

void sensor_wdr_init()
{
    /* 1080p30 */
    sensor_write_register (0x200, 0x01); /* standby */
    //       delay_ms(200);

    sensor_write_register (0x205, 0x01);/* ADBIT 1h:12bit*/
    sensor_write_register (0x206, 0x00);/* MODE: All-pix scan */
    sensor_write_register (0x207, 0x10);/* WINMODE 0h:All-pixel scan */
    sensor_write_register (0x209, 0x02);/* FRSEL */
    sensor_write_register (0x20A, 0xF0);/* BLKLEVEL[7:0] */
    sensor_write_register (0x20B, 0x00);/* BLKLEVEL[8] */
    sensor_write_register (0x20C, 0x02);/* WDMODE 2h:Built-in WDR */
    sensor_write_register (0x20F, 0x05);/* WDC_CMPEN 4h:Output compressed */
    sensor_write_register (0x210, 0x38);/*  */
    sensor_write_register (0x212, 0x0F);/*  */
    sensor_write_register (0x214, 0x28);/* GAIN This set only Digital Gain(Again fixed 4.5DB) */
    sensor_write_register (0x218, 0x65);/* VMAX[7:0] */
    sensor_write_register (0x219, 0x04);/* VMAX[15:8] */
    sensor_write_register (0x21A, 0x00);/* VMAX[16] */
    sensor_write_register (0x21B, 0x98);/* HMAX[7:0] */
    sensor_write_register (0x21C, 0x08);/* HMAX[15:8] */
    sensor_write_register (0x220, 0xD5);/* SHS1[7:0] Short exposure */
    sensor_write_register (0x221, 0x04);/* SHS1[15:8] Short */
    sensor_write_register (0x222, 0x00);/* SHS1[16] Short */
    sensor_write_register (0x223, 0x07);/* SHS2[7:0] Long exposure */
    sensor_write_register (0x224, 0x00);/* SHS2[15:8] Long */
    sensor_write_register (0x225, 0x00);/* SHS2[16] Long */
    sensor_write_register (0x244, 0xE1);/* CSI-2 ODBIT */
    sensor_write_register (0x265, 0x00);/*  */
    sensor_write_register (0x284, 0x0F);/*  */
    sensor_write_register (0x286, 0x10);/*  */
    sensor_write_register (0x2CF, 0xE1);/*  */
    sensor_write_register (0x2D0, 0x29);/*  */
    sensor_write_register (0x2D2, 0x9B);/*  */
    sensor_write_register (0x2D3, 0x01);/*  */

    //        sensor_write_register (0x21D, 0x08);
    //        sensor_write_register (0x512, 0x00);
    sensor_write_register (0x21E, 0x02);

    sensor_write_register (0x25C, 0x20); /* INCKSEL1*/
    sensor_write_register (0x25D, 0x00); /* INCKSEL2*/
    sensor_write_register (0x25E, 0x18); /* INCKSEL3*/
    sensor_write_register (0x25F, 0x00); /* INCKSEL4*/
    sensor_write_register (0x263, 0x74); /* INCKSEL574h*/

    sensor_write_register (0x31D, 0x0A);
    sensor_write_register (0x323, 0x0F);
    sensor_write_register (0x347, 0x87);
    sensor_write_register (0x3E1, 0x9E);
    sensor_write_register (0x3E2, 0x01);
    sensor_write_register (0x3E5, 0x05);
    sensor_write_register (0x3E6, 0x05); 
    sensor_write_register (0x3E7, 0x3A);
    sensor_write_register (0x3E8, 0x3A);

    sensor_write_register (0x541, 0x20); /* INCK_FREQ[7:0]*/
    sensor_write_register (0x542, 0x25); /* INCK_FREQ[15:8]*/
    sensor_write_register (0x54E, 0xB4); /* INCK_FREQ2[7:0]b4*/
    sensor_write_register (0x54F, 0x01); /* INCK_FREQ2[10:8]01*/
    sensor_write_register (0x503, 0x10); /* REPETITION=1  */
    sensor_write_register (0x505, 0x03); /* PHYSICAL Lane NUM 3h:4 lanes; 2h:2 lanes */ 
    sensor_write_register (0x514, 0x08); /* OB_SIZE_V[5:0]*/
    sensor_write_register (0x515, 0x01); /* NULL0_SIZE_V[5:0]*/
    sensor_write_register (0x516, 0x04); /* NULL1_SIZE_V[5:0]*/
    sensor_write_register (0x517, 0x04); /* NULL2_SIZE_V[5:0]*/
    sensor_write_register (0x518, 0x49); /* PIC_SIZE_V[7:0]*/
    sensor_write_register (0x519, 0x04); /* PIC_SIZE_V[11:8]*/
    sensor_write_register (0x52c, 0x30); /* THSEXIT: Global Timing Setting30*/
    sensor_write_register (0x52d, 0x20); /* TCLKPRE: Global Timing Setting*/
    sensor_write_register (0x52e, 0x03); /* TLPXESC*/
    sensor_write_register (0x53e, 0x0c); /* CSI_DT_FMT[7:0]*/   /* CSI_DT_FMT=0x0c0c 12bit CSI_DT_FMT=0x0a0a 10bit*/
    sensor_write_register (0x53f, 0x0c); /* CSI_DT_FMT[15:8]*/
    sensor_write_register (0x540, 0x03); /* CSI_LANE_MODE*/
    sensor_write_register (0x543, 0x58); /* TCLK_POST*/
    sensor_write_register (0x544, 0x10); /* THS_PREPARE 10*/
    sensor_write_register (0x545, 0x30); /* THS_ZERO_MIN 30*/
    sensor_write_register (0x546, 0x18); /* THS_TRAIL 18*/
    sensor_write_register (0x547, 0x10); /* TCLK_TRAIL_MIN 10*/
    sensor_write_register (0x548, 0x10); /* TCLK_PREPARE 10*/
    sensor_write_register (0x549, 0x48); /* TCLK_ZERO 48*/
    sensor_write_register (0x54A, 0x28); /* TCPX*/

    /* registers must be changed */
    sensor_write_register (0x403, 0xC8);
    sensor_write_register (0x407, 0x54);
    sensor_write_register (0x413, 0x16);
    sensor_write_register (0x415, 0xF6);
    sensor_write_register (0x41A, 0x14);
    sensor_write_register (0x41B, 0x51);
    sensor_write_register (0x429, 0xE7);
    sensor_write_register (0x42A, 0xF0);
    sensor_write_register (0x42B, 0x10);
    sensor_write_register (0x431, 0xE7);
    sensor_write_register (0x432, 0xF0);
    sensor_write_register (0x433, 0x10);
    sensor_write_register (0x43C, 0xE8);
    sensor_write_register (0x43D, 0x70);
    sensor_write_register (0x443, 0x08);
    sensor_write_register (0x444, 0xE1);
    sensor_write_register (0x445, 0x10);
    sensor_write_register (0x447, 0xE7);
    sensor_write_register (0x448, 0x60);
    sensor_write_register (0x449, 0x1E);
    sensor_write_register (0x44B, 0x00);
    sensor_write_register (0x44C, 0x41);
    sensor_write_register (0x450, 0x30);
    sensor_write_register (0x451, 0x0A);
    sensor_write_register (0x452, 0xFF);
    sensor_write_register (0x453, 0xFF);
    sensor_write_register (0x454, 0xFF);
    sensor_write_register (0x455, 0x02);
    sensor_write_register (0x457, 0xF0);
    sensor_write_register (0x45A, 0xA6);
    sensor_write_register (0x45D, 0x14);
    sensor_write_register (0x45E, 0x51);
    sensor_write_register (0x460, 0x00);
    sensor_write_register (0x461, 0x61);
    sensor_write_register (0x466, 0x30);
    sensor_write_register (0x467, 0x05);
    sensor_write_register (0x475, 0xE7);
    sensor_write_register (0x481, 0xEA);
    sensor_write_register (0x482, 0x70);
    sensor_write_register (0x485, 0xFF);
    sensor_write_register (0x48A, 0xF0);
    sensor_write_register (0x48D, 0xB6);
    sensor_write_register (0x48E, 0x40);
    sensor_write_register (0x490, 0x42);
    sensor_write_register (0x491, 0x51);
    sensor_write_register (0x492, 0x1E);
    sensor_write_register (0x494, 0xC4);
    sensor_write_register (0x495, 0x20);
    sensor_write_register (0x497, 0x50);
    sensor_write_register (0x498, 0x31);
    sensor_write_register (0x499, 0x1F);
    sensor_write_register (0x49B, 0xC0);
    sensor_write_register (0x49C, 0x60);
    sensor_write_register (0x49E, 0x4C);
    sensor_write_register (0x49F, 0x71);
    sensor_write_register (0x4A0, 0x1F);
    sensor_write_register (0x4A2, 0xB6);
    sensor_write_register (0x4A3, 0xC0);
    sensor_write_register (0x4A4, 0x0B);
    sensor_write_register (0x4A9, 0x24);
    sensor_write_register (0x4AA, 0x41);
    sensor_write_register (0x4B0, 0x25);
    sensor_write_register (0x4B1, 0x51);
    sensor_write_register (0x4B7, 0x1C);
    sensor_write_register (0x4B8, 0xC1);
    sensor_write_register (0x4B9, 0x12);
    sensor_write_register (0x4BE, 0x1D);
    sensor_write_register (0x4BF, 0xD1);
    sensor_write_register (0x4C0, 0x12);
    sensor_write_register (0x4C2, 0xA8);
    sensor_write_register (0x4C3, 0xC0);
    sensor_write_register (0x4C4, 0x0A);
    sensor_write_register (0x4C5, 0x1E);
    sensor_write_register (0x4C6, 0x21);
    sensor_write_register (0x4C9, 0xB0);
    sensor_write_register (0x4CA, 0x40);
    sensor_write_register (0x4CC, 0x26);
    sensor_write_register (0x4CD, 0xA1);
    sensor_write_register (0x4D0, 0xB6);
    sensor_write_register (0x4D1, 0xC0);
    sensor_write_register (0x4D2, 0x0B);
    sensor_write_register (0x4D4, 0xE2);
    sensor_write_register (0x4D5, 0x40);
    sensor_write_register (0x4D8, 0x4E);
    sensor_write_register (0x4D9, 0xA1);
    sensor_write_register (0x4EC, 0xF0);

    sensor_write_register (0x200, 0x00); /* standby */
    sensor_write_register (0x202, 0x00); /* master mode start */
    sensor_write_register (0x249, 0x0A);

    printf("-------Sony IMX185 Sensor Built-in WDR 1080p30 Initial OK!-------\n");

    bSensorInit = HI_TRUE;

}


