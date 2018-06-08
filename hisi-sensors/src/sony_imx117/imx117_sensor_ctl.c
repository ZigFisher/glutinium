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
#include "pwm.h"

#define PWM_HSYNC   (5)
#define PWM_VSYNC   (6)
typedef struct hiIMX117_FRAME_ARCH_S {
    HI_U32 hsync_div;
    HI_U32 pixel_clk;
} IMX117_FRAME_ARCH_S;

extern WDR_MODE_E genSensorMode;
extern HI_BOOL bSensorInit;

static int g_fd = -1;

#define SENSOR_1080P_60FPS_MODE (0)
#define SENSOR_720P_120FPS_MODE (1)
#define SENSOR_VGA_240FPS_MODE  (2)
#define SENSOR_1080P_50FPS_MODE (3)

extern HI_U8 gu8SensorImageMode ;

/*imx117 surport 3 frame arch current*/
const IMX117_FRAME_ARCH_S stImx117FrmAch[4] = {
    { 0x61c10,  0x9a },
    { 0x30de8,  0xbf },
    { 0x18704,  0x79 },
    { 0x75300,  0xc8 }
};



int IMX117_pwmWrite(int pwmChn, unsigned int period, unsigned int duty)
{
	int ret;
	int fd = -1;
	PWM_DATA_S  stPwmData;

	fd = open("/dev/pwm", 0);
	if(fd<0)
	{
		printf("Open pwm error! @%s line:%d\n", __FUNCTION__, __LINE__);
		return -1;
	}


	stPwmData.pwm_num = pwmChn;
	stPwmData.period = period;
	stPwmData.enable = 1;
	stPwmData.duty = duty;

	//printf("pwm_num:%d, duty:%d, period:%d______enable %d\n", stPwmData.pwm_num, stPwmData.duty, stPwmData.period, stPwmData.enable);
	ret = ioctl(fd, PWM_CMD_WRITE, &stPwmData);
	close(fd);
   
	return ret;
}


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
    
    
    tx_buf[0] = 0x81;
    tx_buf[1] = addr >> 8;
    tx_buf[2] = addr & 0xff;
    tx_buf[3] = data;

    //printf("func:%s tx_buf = %#x, %#x, %#x, %#x\n", __func__, tx_buf[0], tx_buf[1], tx_buf[2], tx_buf[3]);

    memset(mesg, 0, sizeof(mesg));  
    mesg[0].tx_buf = (__u32)tx_buf;  
    mesg[0].len    = 4;  
    mesg[0].rx_buf = (__u32)rx_buf; 
    mesg[0].cs_change = 1;

    ret = ioctl(g_fd, SPI_IOC_MESSAGE(1), mesg);
    if (ret < 0) {  
        printf("SPI_IOC_MESSAGE error \n");  
        return -1;  
    }
    //printf("func:%s ret = %d, rx_buf = %#x, %#x, %#x, %#x\n", __func__, ret , rx_buf[0], rx_buf[1], rx_buf[2], rx_buf[3]);

    return 0;
}

int sensor_read_register(unsigned int addr)
{
    int ret = 0;
    struct spi_ioc_transfer mesg[1];
    unsigned char  tx_buf[8] = {0};
    unsigned char  rx_buf[8] = {0};
    

    tx_buf[0] = 0x80;
    tx_buf[1] = addr >> 8;
    tx_buf[2] = addr & 0xff;
    tx_buf[3] = 0;

    memset(mesg, 0, sizeof(mesg));
    mesg[0].tx_buf = (__u32)tx_buf;
    mesg[0].len    = 4;
    mesg[0].rx_buf = (__u32)rx_buf;
    mesg[0].cs_change = 1;

    ret = ioctl(g_fd, SPI_IOC_MESSAGE(1), mesg);
    if (ret  < 0) {  
        printf("SPI_IOC_MESSAGE error \n");  
        return -1;  
    }
    
    return rx_buf[3];
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
void sensor_linear_1080p60_init();
void sensor_linear_720p120_init();
void sensor_linear_VGA240_init();

void sensor_init()
{  
    static HI_U8 u8SensorImageModePrev = -1;
      
    /* 1. sensor spi init */
    sensor_spi_init();
     /* When sensor first init, config all registers */

    if (!((u8SensorImageModePrev == SENSOR_1080P_60FPS_MODE && gu8SensorImageMode == SENSOR_1080P_50FPS_MODE) ||
        (u8SensorImageModePrev == SENSOR_1080P_50FPS_MODE && gu8SensorImageMode == SENSOR_1080P_60FPS_MODE)))
    {

        if (SENSOR_1080P_60FPS_MODE == gu8SensorImageMode)    
        {
            sensor_linear_1080p60_init(); 
        }
        else if(SENSOR_720P_120FPS_MODE== gu8SensorImageMode)                      
        {
            sensor_linear_720p120_init();
        }
        else if(SENSOR_VGA_240FPS_MODE == gu8SensorImageMode)
        {
            sensor_linear_VGA240_init();
        }
        else if(SENSOR_1080P_50FPS_MODE == gu8SensorImageMode)
        {
            sensor_linear_1080p60_init();
        }
        else
        {    
        }

    }
    u8SensorImageModePrev = gu8SensorImageMode;
    /* 
     * next then we write2pwm regsiters to implement the frame archtecture
     * chn 5: hsync,  chn 6: pixel_clk
     */   
     
    IMX117_pwmWrite(PWM_HSYNC, stImx117FrmAch[gu8SensorImageMode].hsync_div, stImx117FrmAch[gu8SensorImageMode].hsync_div-0x10);
    IMX117_pwmWrite(PWM_VSYNC, stImx117FrmAch[gu8SensorImageMode].pixel_clk, stImx117FrmAch[gu8SensorImageMode].pixel_clk-0x10);
    
    bSensorInit = HI_TRUE;
   
    return ;
}

void sensor_exit()
{
    sensor_spi_exit();

    return;
}

void sensor_linear_VGA240_init()
{

    
    sensor_write_register (0x0000, 0x07);
                          
    sensor_write_register (0x0001, 0x00);
    sensor_write_register (0x0002, 0x00);
                          
    sensor_write_register (0x0003, 0x11);
    sensor_write_register (0x0004, 0x48);
    sensor_write_register (0x0005, 0x21);
    sensor_write_register (0x0006, 0x00);
    sensor_write_register (0x0007, 0x00);
                          
    sensor_write_register (0x0008, 0x00);
    sensor_write_register (0x0009, 0x00);
                          
    //sensor_write_register (0x000A, 0x06);
    sensor_write_register (0x000b, 0x0a);
    //sensor_write_register (0x000C, 0x00);
    sensor_write_register (0x000d, 0x00);
    //sensor_write_register (0x000E, 0x00);
    sensor_write_register (0x000f, 0x00);
    //sensor_write_register (0x0010, 0x00);
    sensor_write_register (0x0011, 0x03);
                          
    sensor_write_register (0x001a, 0x00);
    sensor_write_register (0x0026, 0x74);
    sensor_write_register (0x0027, 0x74);
    sensor_write_register (0x0028, 0x74);
                          
    sensor_write_register (0x0045, 0x32);
                          
    sensor_write_register (0x007e, 0x00);
    //sensor_write_register (0x007F, 0x00);
    sensor_write_register (0x0080, 0x00);
    sensor_write_register (0x0081, 0x00);
    sensor_write_register (0x0082, 0x00);
    sensor_write_register (0x0083, 0x00);
    sensor_write_register (0x0084, 0x00);
    sensor_write_register (0x0085, 0x00);
    sensor_write_register (0x0086, 0x00);
    sensor_write_register (0x0087, 0x00);
    sensor_write_register (0x0095, 0x00);
    sensor_write_register (0x0096, 0x00);
    sensor_write_register (0x0097, 0x00);
    sensor_write_register (0x0098, 0x00);
    sensor_write_register (0x0099, 0x00);
    sensor_write_register (0x009a, 0x00);
    sensor_write_register (0x009b, 0x00);
    sensor_write_register (0x009c, 0x00);
                          
    sensor_write_register (0x00b6, 0x00);
    sensor_write_register (0x00b7, 0x00);
    sensor_write_register (0x00b8, 0x00);
    sensor_write_register (0x00b9, 0x00);
    sensor_write_register (0x00ba, 0x00);
    sensor_write_register (0x00bb, 0x00);
    sensor_write_register (0x00bc, 0x00);
    sensor_write_register (0x00bd, 0x00);
    sensor_write_register (0x00be, 0x00);
    sensor_write_register (0x00bf, 0x00);
    sensor_write_register (0x00c0, 0x00);
    sensor_write_register (0x00c1, 0x00);
    sensor_write_register (0x00c2, 0x00);
    sensor_write_register (0x00c3, 0x00);
    sensor_write_register (0x00c4, 0x00);
    sensor_write_register (0x00c5, 0x00);
    sensor_write_register (0x00c6, 0x00);
    sensor_write_register (0x00c7, 0x00);
    sensor_write_register (0x00c8, 0x00);
    sensor_write_register (0x00c9, 0x00);
    sensor_write_register (0x00ca, 0x00);
    sensor_write_register (0x00cb, 0x00);
    sensor_write_register (0x00cc, 0x00);
    sensor_write_register (0x00ce, 0x00);
                          
    sensor_write_register (0x0222, 0x31);
    //sensor_write_register (0x0223, 0x01);
    sensor_write_register (0x0352, 0x28);
    //sensor_write_register (0x0353, 0x00);
    sensor_write_register (0x0356, 0x27);
    //sensor_write_register (0x0357, 0x00);
    sensor_write_register (0x0358, 0x01);
    sensor_write_register (0x0528, 0x0e);
    sensor_write_register (0x0529, 0x0e);
    sensor_write_register (0x052a, 0x0e);
    sensor_write_register (0x052b, 0x0e);
    sensor_write_register (0x0534, 0x10);
    sensor_write_register (0x057e, 0x00);
    sensor_write_register (0x057f, 0x10);
    sensor_write_register (0x0580, 0x0d);
    sensor_write_register (0x0581, 0x0d);
    sensor_write_register (0x0585, 0x00);
    //sensor_write_register (0x0586, 0x07);
    sensor_write_register (0x0617, 0x10);
    sensor_write_register (0x065c, 0x05);
    sensor_write_register (0x0700, 0x19);
    //sensor_write_register (0x0701, 0x19);

    sensor_write_register (0x002e, 0xff);
    sensor_write_register (0x002f, 0xff);
    sensor_write_register (0x0030, 0xff);
    sensor_write_register (0x0031, 0xff);
    sensor_write_register (0x0032, 0xff);
    sensor_write_register (0x0033, 0x00);
    
    sensor_write_register (0x0000, 0x06);
    delay_ms(2);          
    sensor_write_register (0x0000, 0x00);
    
    printf("-------Sony IMX117 Sensor VGA240 Initial OK!-------\n");
    
}

void sensor_linear_720p120_init()
{

    
    sensor_write_register (0x0000, 0x07); 
                                 
    sensor_write_register (0x0001, 0x00);
    sensor_write_register (0x0002, 0x00);
                                 
    sensor_write_register (0x0003, 0x33);
    sensor_write_register (0x0004, 0x40);
    sensor_write_register (0x0005, 0x1D);
    sensor_write_register (0x0006, 0x00);
    sensor_write_register (0x0007, 0x00);
    sensor_write_register (0x0008, 0x00);
                                 
    sensor_write_register (0x0009, 0x00);
    sensor_write_register (0x000A, 0x06);
    sensor_write_register (0x000b, 0x0a);
    sensor_write_register (0x000C, 0x00);
    sensor_write_register (0x000d, 0x00);
    sensor_write_register (0x000E, 0x00);
    sensor_write_register (0x000f, 0x00);
    sensor_write_register (0x0010, 0x00);
    sensor_write_register (0x0011, 0x03);
    sensor_write_register (0x001a, 0x00);
                                 
    sensor_write_register (0x0026, 0x74);
    sensor_write_register (0x0027, 0x74);
    sensor_write_register (0x0028, 0x74);
                                 
    sensor_write_register (0x007E, 0x00);
    sensor_write_register (0x007F, 0x00);
    sensor_write_register (0x0080, 0x00);
    sensor_write_register (0x0081, 0x00);
    sensor_write_register (0x0082, 0x00);
    sensor_write_register (0x0083, 0x00);
    sensor_write_register (0x0084, 0x00);
    sensor_write_register (0x0085, 0x00);
    sensor_write_register (0x0086, 0x00);
    sensor_write_register (0x0087, 0x00);
    sensor_write_register (0x0095, 0x00);
    sensor_write_register (0x0096, 0x00);
    sensor_write_register (0x0097, 0x00);
    sensor_write_register (0x0098, 0x00);
    sensor_write_register (0x0099, 0x00);
    sensor_write_register (0x009A, 0x00);
    sensor_write_register (0x009B, 0x00);
    sensor_write_register (0x009C, 0x00);
                                 
    sensor_write_register (0x00B6, 0x00);
    sensor_write_register (0x00B7, 0x00);
    sensor_write_register (0x00B8, 0x00);
    sensor_write_register (0x00B9, 0x00);
    sensor_write_register (0x00BA, 0x00);
    sensor_write_register (0x00BB, 0x00);
    sensor_write_register (0x00BC, 0x00);
    sensor_write_register (0x00BD, 0x00);
    sensor_write_register (0x00BE, 0x00);
    sensor_write_register (0x00BF, 0x00);
    sensor_write_register (0x00C0, 0x00);
    sensor_write_register (0x00C1, 0x00);
    sensor_write_register (0x00C2, 0x00);
    sensor_write_register (0x00C3, 0x00);
    sensor_write_register (0x00C4, 0x00);
    sensor_write_register (0x00C5, 0x00);
    sensor_write_register (0x00C6, 0x00);
    sensor_write_register (0x00C7, 0x00);
    sensor_write_register (0x00C8, 0x00);
    sensor_write_register (0x00C9, 0x00);
    sensor_write_register (0x00CA, 0x00);
    sensor_write_register (0x00CB, 0x00);
    sensor_write_register (0x00CC, 0x00);
    sensor_write_register (0x00CE, 0x00);
                                 
    sensor_write_register (0x0222, 0x31);
    sensor_write_register (0x0223, 0x01);
                                 
    sensor_write_register (0x0352, 0x28);   
    sensor_write_register (0x0353, 0x00);
    sensor_write_register (0x0356, 0x27);
    sensor_write_register (0x0357, 0x00);
                                 
    sensor_write_register (0x0358, 0x01);
                                 
    sensor_write_register (0x0528, 0x0E);
    sensor_write_register (0x0529, 0x0E);
    sensor_write_register (0x052A, 0x0E);
    sensor_write_register (0x052B, 0x0E);
    sensor_write_register (0x0534, 0x10);
    sensor_write_register (0x057E, 0x00);
    sensor_write_register (0x057F, 0x10);
    sensor_write_register (0x0580, 0x0D);
    sensor_write_register (0x0581, 0x0D);
    sensor_write_register (0x0585, 0x00);
    sensor_write_register (0x0586, 0x07);
                                 
    sensor_write_register (0x0617, 0x10);
    sensor_write_register (0x065C, 0x05);
   
    sensor_write_register (0x0700, 0x19);
    sensor_write_register (0x0701, 0x19);
  
    sensor_write_register (0x002e, 0xff);
    sensor_write_register (0x002f, 0xff);
    sensor_write_register (0x0030, 0xff);
    sensor_write_register (0x0031, 0xff);
    sensor_write_register (0x0032, 0xff);
    sensor_write_register (0x0033, 0x00);
    
    sensor_write_register (0x0000, 0x06);
    delay_ms(2);          
    sensor_write_register (0x0000, 0x00);
    
    printf("-------Sony IMX117 Sensor 720p120 Initial OK!-------\n");
}


void sensor_linear_1080p60_init()
{

    sensor_write_register (0x0000, 0x07);
    sensor_write_register (0x0001, 0x00);
    sensor_write_register (0x0002, 0x00);
    sensor_write_register (0x0003, 0x33);
    sensor_write_register (0x0004, 0x19);
    sensor_write_register (0x0005, 0x0d);
    sensor_write_register (0x0006, 0x00);
    sensor_write_register (0x0007, 0x00);
    sensor_write_register (0x0008, 0x00);
    sensor_write_register (0x0009, 0x00);
    sensor_write_register (0x000A, 0x00);
    sensor_write_register (0x000b, 0x1a);
    sensor_write_register (0x000C, 0x01);
    sensor_write_register (0x000d, 0x00);
    sensor_write_register (0x000E, 0x00);
    sensor_write_register (0x000f, 0x00);
    sensor_write_register (0x0010, 0x00);
    sensor_write_register (0x0011, 0x00);
    sensor_write_register (0x001a, 0x00);
    sensor_write_register (0x0026, 0x74);
    sensor_write_register (0x0027, 0x74);
    sensor_write_register (0x0028, 0x74);
    //sensor_write_register (0x003b, 0x11);
    //sensor_write_register (0x003c, 0x0a);

    
    sensor_write_register (0x0045, 0x32);
    sensor_write_register (0x007e, 0x20);
    sensor_write_register (0x007F, 0x01);
    sensor_write_register (0x0080, 0x00);
    sensor_write_register (0x0081, 0x00);
    sensor_write_register (0x0082, 0x00);
    sensor_write_register (0x0083, 0x00);
    sensor_write_register (0x0084, 0x00);
    sensor_write_register (0x0085, 0x00);
    sensor_write_register (0x0086, 0x00);
    sensor_write_register (0x0087, 0x00);
    sensor_write_register (0x0095, 0x00);
    sensor_write_register (0x0096, 0x00);
    sensor_write_register (0x0097, 0x00);
    sensor_write_register (0x0098, 0x00);
    sensor_write_register (0x0099, 0x00);
    sensor_write_register (0x009a, 0x00);
    sensor_write_register (0x009b, 0x00);
    sensor_write_register (0x009c, 0x00);
    sensor_write_register (0x00b6, 0x67);
    sensor_write_register (0x00b7, 0x00);
    sensor_write_register (0x00b8, 0x00);
    sensor_write_register (0x00b9, 0x00);
    sensor_write_register (0x00ba, 0x00);
    sensor_write_register (0x00bb, 0x00);
    sensor_write_register (0x00bc, 0x00);
    sensor_write_register (0x00bd, 0x00);
    sensor_write_register (0x00be, 0x00);
    sensor_write_register (0x00bf, 0x00);
    sensor_write_register (0x00c0, 0x00);
    sensor_write_register (0x00c1, 0x00);
    sensor_write_register (0x00c2, 0x00);
    sensor_write_register (0x00c3, 0x00);
    sensor_write_register (0x00c4, 0x00);
    sensor_write_register (0x00c5, 0x00);
    sensor_write_register (0x00c6, 0x00);
    sensor_write_register (0x00c7, 0x00);
    sensor_write_register (0x00c8, 0x00);
    sensor_write_register (0x00c9, 0x00);
    sensor_write_register (0x00ca, 0x00);
    sensor_write_register (0x00cb, 0x00);
    sensor_write_register (0x00cc, 0x00);
    sensor_write_register (0x00ce, 0x0e);
    sensor_write_register (0x0222, 0x31);
    sensor_write_register (0x0223, 0x01);
    sensor_write_register (0x0352, 0x1f);
    sensor_write_register (0x0353, 0x00);
    sensor_write_register (0x0356, 0x1e);
    sensor_write_register (0x0357, 0x00);
    sensor_write_register (0x0358, 0x01);
    sensor_write_register (0x0528, 0x0e);
    sensor_write_register (0x0529, 0x0e);
    sensor_write_register (0x052a, 0x0e);
    sensor_write_register (0x052b, 0x0e);
    sensor_write_register (0x0534, 0x10);
    sensor_write_register (0x057e, 0x00);
    sensor_write_register (0x057f, 0x10);
    sensor_write_register (0x0580, 0x0d);
    sensor_write_register (0x0581, 0x0d);
    sensor_write_register (0x0585, 0x00);
    sensor_write_register (0x0586, 0x07);
    sensor_write_register (0x0617, 0x10);
    sensor_write_register (0x065c, 0x05);
    sensor_write_register (0x0700, 0x19);
    sensor_write_register (0x0701, 0x19);
    
    sensor_write_register (0x002e, 0xff);
    sensor_write_register (0x002f, 0xff);
    sensor_write_register (0x0030, 0xff);
    sensor_write_register (0x0031, 0xff);
    sensor_write_register (0x0032, 0xff);
    sensor_write_register (0x0033, 0x00);

    sensor_write_register (0x0000, 0x06);
    delay_ms(2);          
    sensor_write_register (0x0000, 0x00);

    printf("-------Sony IMX117 Sensor 1080p60 Initial OK!-AAAAA------\n");


}

void sensor_wdr_init()
{
    return ;
}


