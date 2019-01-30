#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "hi_comm_video.h"

#ifdef HI_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "hi_i2c.h"
#endif

const unsigned char sensor_i2c_addr	    =	0x60;		/* I2C Address of SC2035 */
const unsigned int  sensor_addr_byte	=	2;
const unsigned int  sensor_data_byte	=	1;
static int g_fd = -1;
static int flag_init = 0;

extern WDR_MODE_E genSensorMode;
extern HI_U8 gu8SensorImageMode;
extern HI_BOOL bSensorInit;

int sensor_i2c_init(void)
{
    if(g_fd >= 0)
    {
        return 0;
    }    
#ifdef HI_GPIO_I2C
    int ret;

    g_fd = open("/dev/gpioi2c_ex", 0);
    if(g_fd < 0)
    {
        printf("Open gpioi2c_ex error!\n");
        return -1;
    }
#else
    int ret;

    g_fd = open("/dev/i2c-0", O_RDWR);
    if(g_fd < 0)
    {
        printf("Open /dev/i2c-0 error!\n");
        return -1;
    }

    ret = ioctl(g_fd, I2C_SLAVE_FORCE, sensor_i2c_addr);
    if (ret < 0)
    {
        printf("CMD_SET_DEV error!\n");
        return ret;
    }
#endif

    return 0;
}

int sensor_i2c_exit(void)
{
    if (g_fd >= 0)
    {
        close(g_fd);
        g_fd = -1;
        return 0;
    }
    return -1;
}

int sensor_read_register(int addr)
{
	// TODO: 
	
	return 0;
}



int sensor_write_register(int addr, int data)
{
#ifdef HI_GPIO_I2C
    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sensor_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sensor_data_byte;

    ret = ioctl(g_fd, GPIO_I2C_WRITE, &i2c_data);

    if (ret)
    {
        printf("GPIO-I2C write faild!\n");
        return ret;
    }
#else
    if(flag_init == 0)
    {
    
	sensor_i2c_init();
	flag_init = 1;
    }

    int idx = 0;
    int ret;
    char buf[8];

    buf[idx++] = addr & 0xFF;
    if (sensor_addr_byte == 2)
    {
    	ret = ioctl(g_fd, I2C_16BIT_REG, 1);
        buf[idx++] = addr >> 8;
    }
    else
    {
    	ret = ioctl(g_fd, I2C_16BIT_REG, 0);
    }

    if (ret < 0)
    {
        printf("CMD_SET_REG_WIDTH error!\n");
        return -1;
    }

    buf[idx++] = data;
    if (sensor_data_byte == 2)
    {
    	ret = ioctl(g_fd, I2C_16BIT_DATA, 1);
        buf[idx++] = data >> 8;
    }
    else
    {
    	ret = ioctl(g_fd, I2C_16BIT_DATA, 0);
    }

    if (ret)
    {
        printf("hi_i2c write faild!\n");
        return -1;
    }

    ret = write(g_fd, buf, idx);
    if(ret < 0)
    {
    	printf("I2C_WRITE error!\n");
    	return -1;
    }
#endif
	return 0;
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

void sensor_linear_1080p30_init();

#define SENSOR_1080P_30FPS_MODE  (1)

void sensor_init()
{
    sensor_i2c_init();
#if 0
    /* When sensor first init, config all registers */
    if (HI_FALSE == bSensorInit) 
    {
        if(SENSOR_720P_25FPS_MODE == gu8SensorImageMode)
        {
            sensor_linear_720p25_init();
        }
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else 
    {
        if(SENSOR_720P_25FPS_MODE == gu8SensorImageMode)
        {
            sensor_linear_720p25_init();
        }
    }
#endif
    sensor_linear_1080p30_init();

    return ;
}

void sensor_exit()
{
    sensor_i2c_exit();
	flag_init = 0;
    return;
}


void sensor_linear_1080p30_init()
{
    //2.8AVDD_1.8DOVDD_SC2035_27Minput_67.5MPCLK_1080p_30fps_20160317
#if 1  //20160317
sensor_write_register(0x3105,0x02);  //start up timing begin
sensor_write_register(0x0103,0x01);  // reset all registers
sensor_write_register(0x3105,0x02);  
sensor_write_register(0x0100,0x00);  //start up timing end
   
sensor_write_register(0x301E,0xB0);  // mode select
    
sensor_write_register(0x320c,0x03);  // hts=2000
sensor_write_register(0x320d,0xe8);   
sensor_write_register(0x3231,0x24);  // half hts  to 2000
sensor_write_register(0x320E,0x04);  
sensor_write_register(0x320F,0x65);  
sensor_write_register(0x3211,0x08);  //x start 20160113 20160120
sensor_write_register(0x3213,0x10);  //y start
   
sensor_write_register(0x3e03,0x03);  //AEC AGC 03 : close aec/agc 00: open aec/agc
sensor_write_register(0x3e01,0x46);  //exp time
sensor_write_register(0x3e08,0x00);  //gain 1x
sensor_write_register(0x3e09,0x10);  //10-1f,16step->1/16
sensor_write_register(0x3518,0x03);  
sensor_write_register(0x5025,0x09);  
   
sensor_write_register(0x3908,0xc0);  //BLC RNCincrease blc target for rnc
sensor_write_register(0x3907,0x01);  //12.14
sensor_write_register(0x3928,0x01);  //20160315
sensor_write_register(0x3416,0x12);  //20160113
sensor_write_register(0x3401,0x1e);  //12.11
sensor_write_register(0x3402,0x0c);  //12.11
sensor_write_register(0x3403,0x70);  //12.11
sensor_write_register(0x3e0f,0x90);  
   
sensor_write_register(0x3638,0x84);  //RAMP config  20160113B
sensor_write_register(0x3637,0xbc);  //1018 20160113 20160120
sensor_write_register(0x3639,0x98);  
sensor_write_register(0x3035,0x01);  //count clk
sensor_write_register(0x3034,0xc2);  //1111   20160120
   
sensor_write_register(0x3300,0x12);  //eq  20160315
sensor_write_register(0x3301,0x08);  //cmprst  20160120 20160307
sensor_write_register(0x3308,0x30);  // tx 1111 20160307
sensor_write_register(0x3306,0x3a);  // count down 1111 20160120
sensor_write_register(0x330a,0x00);  
sensor_write_register(0x330b,0x90);  // count up
sensor_write_register(0x3303,0x30);  //ramp gap 20160307
sensor_write_register(0x3309,0x30);  //cnt up gap 20160307
sensor_write_register(0x331E,0x2c);  //integ 1st pos point 20160307
sensor_write_register(0x331F,0x2c);  //integ 2nd pos point 20160307
sensor_write_register(0x3320,0x2e);  //ofs fine 1st pos point 20160307
sensor_write_register(0x3321,0x2e);  //ofs fine 2nd pos point 20160307
sensor_write_register(0x3322,0x2e);  //20160307
sensor_write_register(0x3323,0x2e);  //20160307
   
sensor_write_register(0x3626,0x03);  //memory readout delay 0613 0926
sensor_write_register(0x3621,0x28);  //counter clock div [3] column fpn 0926
sensor_write_register(0x3F08,0x04);  //WRITE TIME
sensor_write_register(0x3F09,0x44);  //WRITE/READ TIME GAP
sensor_write_register(0x4500,0x25);  //data delay 0926
sensor_write_register(0x3c09,0x08);  // Sram start position
   
sensor_write_register(0x335D,0x20);  //prechg tx auto ctrl [5] 
sensor_write_register(0x3368,0x02);  //EXP1
sensor_write_register(0x3369,0x00);  
sensor_write_register(0x336A,0x04);  //EXP2
sensor_write_register(0x336b,0x65);  
sensor_write_register(0x330E,0x50);  // start value
sensor_write_register(0x3367,0x08);  // end value  12.14
   
sensor_write_register(0x3f00,0x06);  
sensor_write_register(0x3f04,0x01);  // sram write
sensor_write_register(0x3f05,0xdf);  // 1111  20160113 20160120 20160307
sensor_write_register(0x3905,0x1c);  
   
sensor_write_register(0x5780,0x7f);  //DPC
sensor_write_register(0x5781,0x0a);  //12.17 20160307
sensor_write_register(0x5782,0x0a);  //12.17 20160307
sensor_write_register(0x5783,0x08);  //12.17  20160307  20160317
sensor_write_register(0x5784,0x08);  //12.17  20160307  20160317
sensor_write_register(0x5785,0x18);  //12.11 ; 20160112 20160307
sensor_write_register(0x5786,0x18);  //12.11 ; 20160112 20160307
sensor_write_register(0x5787,0x18);  //12.11 20160307 20160317
sensor_write_register(0x5788,0x18);  // 20160307  20160317
sensor_write_register(0x5789,0x01);  //12.11 
sensor_write_register(0x578a,0x0f);  //12.11 
sensor_write_register(0x5000,0x06);  
                
sensor_write_register(0x3632,0x44);  //bypass NVDD analog config  20160113
sensor_write_register(0x3622,0x0e);  //enable sa1/ecl blksun  
sensor_write_register(0x3627,0x08);  //0921 20160307
sensor_write_register(0x3630,0xb4);  //94 1111  20160113  20160120
sensor_write_register(0x3633,0x97);  //vrhlp voltage 14~24 could be choosed 20160113
sensor_write_register(0x3620,0x62);  //comp and bitline current 20160307
sensor_write_register(0x363a,0x0c);  //sa1 common voltage
sensor_write_register(0x3333,0x10);  // 20160307
sensor_write_register(0x3334,0x20);  //column fpn 20160307
sensor_write_register(0x3312,0x06);  //20160307
sensor_write_register(0x3340,0x03);  //20160307
sensor_write_register(0x3341,0xb0);  //20160307
sensor_write_register(0x3342,0x02);  //20160307
sensor_write_register(0x3343,0x20);  //20160307
                 
sensor_write_register(0x303f,0x81);  //format
sensor_write_register(0x501f,0x00);  
sensor_write_register(0x3b00,0xf8);  
sensor_write_register(0x3b01,0x40);  
sensor_write_register(0x3c01,0x14);  
sensor_write_register(0x4000,0x00);  
                 
sensor_write_register(0x3d08,0x00);  //data output DVP CLK INV
sensor_write_register(0x3640,0x00);  // pad driver
                 
sensor_write_register(0x0100,0x01);  
sensor_write_register(0x303a,0x07);  // PLL  67.5M pclk
sensor_write_register(0x3039,0x8e);  
sensor_write_register(0x303f,0x82);  
sensor_write_register(0x3636,0x88);  //lpDVDD
sensor_write_register(0x3631,0x80);  //0820  20160113  20160120
sensor_write_register(0x3635,0x66);  //1018  20160113  20160120
sensor_write_register(0x3105,0x04);  //1018
sensor_write_register(0x3105,0x04);  //1018
	
#else
//20160128
	sensor_write_register(0x3105,0x02);  //start up timing begin
	sensor_write_register(0x0103,0x01);  // reset all registers
	sensor_write_register(0x3105,0x02);  
	sensor_write_register(0x0100,0x00);  //start up timing end
	  
	sensor_write_register(0x301E,0xB0);  // mode select
	   
	sensor_write_register(0x320c,0x03);  // hts=2000
	sensor_write_register(0x320d,0xe8);   
	sensor_write_register(0x3231,0x24);  // half hts  to 2000
	sensor_write_register(0x320E,0x04); 
	sensor_write_register(0x320F,0x65);  
	sensor_write_register(0x3211,0x08);  //x start 20160113 20160120
	sensor_write_register(0x3213,0x10);  //y start
	  
	sensor_write_register(0x3e03,0x03);  //AEC AGC 03 : close aec/agc 00: open aec/agc
	sensor_write_register(0x3e01,0x46);  //exp time
	sensor_write_register(0x3e08,0x00);  //gain 1x
	sensor_write_register(0x3e09,0x10);  //10-1f,16step->1/16
	sensor_write_register(0x3518,0x03);  
	sensor_write_register(0x5025,0x09);  
	  
	sensor_write_register(0x3908,0xc0);  //BLC RNCincrease blc target for rnc
	sensor_write_register(0x3907,0x01);  //12.14
	sensor_write_register(0x3416,0x12);  //20160113
	sensor_write_register(0x3401,0x1e);  //12.11
	sensor_write_register(0x3402,0x0c);  //12.11
	sensor_write_register(0x3403,0x70);  //12.11
	sensor_write_register(0x3e0f,0x90);  
	  
	sensor_write_register(0x3638,0x84);  //RAMP config  20160113B
	sensor_write_register(0x3637,0xbc);  //1018 20160113 20160120
	sensor_write_register(0x3639,0x98);  
	sensor_write_register(0x3035,0x01);  //count clk
	sensor_write_register(0x3034,0xc2);  //1111   20160120
	  
	sensor_write_register(0x3300,0x10);  //eq
	sensor_write_register(0x3301,0x14);  //cmprst  20160120
	sensor_write_register(0x3308,0x38);  // tx 1111
	sensor_write_register(0x3306,0x3a);  // count down 1111 20160120
	sensor_write_register(0x330a,0x00);  
	sensor_write_register(0x330b,0x90);  // count up
	sensor_write_register(0x3303,0x18);  //ramp gap
	sensor_write_register(0x3309,0x18);  //cnt up gap
	sensor_write_register(0x331E,0x0e);  //integ 1st pos point
	sensor_write_register(0x331F,0x0e);  //integ 2nd pos point
	sensor_write_register(0x3320,0x14);  //ofs fine 1st pos point
	sensor_write_register(0x3321,0x14);  //ofs fine 2nd pos point
	sensor_write_register(0x3322,0x14);  
	sensor_write_register(0x3323,0x14);  
	  
	sensor_write_register(0x3626,0x03);  //memory readout delay 0613 0926
	sensor_write_register(0x3621,0x28);  //counter clock div [3] column fpn 0926
	sensor_write_register(0x3F08,0x04);  //WRITE TIME
	sensor_write_register(0x3F09,0x44);  //WRITE/READ TIME GAP
	sensor_write_register(0x4500,0x25);  //data delay 0926
	sensor_write_register(0x3c09,0x08);  // Sram start position
	  
	sensor_write_register(0x335D,0x20);  //prechg tx auto ctrl [5] 
	sensor_write_register(0x3368,0x02);  //EXP1
	sensor_write_register(0x3369,0x00);  
	sensor_write_register(0x336A,0x04);  //EXP2
	sensor_write_register(0x336b,0x65);  
	sensor_write_register(0x330E,0x50);  // start value
	sensor_write_register(0x3367,0x08);  // end value  12.14
	  
	sensor_write_register(0x3f00,0x06);  
	sensor_write_register(0x3f04,0x01);  // sram write
	sensor_write_register(0x3f05,0xd8);  // 1111  20160113 20160120
	sensor_write_register(0x3905,0x1c);  
	  
	sensor_write_register(0x5780,0x7f);  //DPC
	sensor_write_register(0x5781,0x0a);  //12.17
	sensor_write_register(0x5782,0x0a);  //12.17
	sensor_write_register(0x5783,0x08);  //12.17 
	sensor_write_register(0x5784,0x08);  //12.17
	sensor_write_register(0x5785,0x10);  //12.11 ; 20160112
	sensor_write_register(0x5786,0x10);  //12.11 ; 20160112
	sensor_write_register(0x5787,0x0c);  //12.11
	sensor_write_register(0x5788,0x0c);  
	sensor_write_register(0x5789,0x01);  //12.11
	sensor_write_register(0x578a,0x0f);  //12.11
	sensor_write_register(0x5000,0x06);  
	               
	sensor_write_register(0x3632,0x44);  //bypass NVDD analog config  20160113
	sensor_write_register(0x3622,0x0e);  //enable sa1/ecl blksun  
	sensor_write_register(0x3627,0x02);  //0921
	sensor_write_register(0x3630,0xb4);  //94 1111  20160113  20160120
	sensor_write_register(0x3633,0x97);  //vrhlp voltage 14~24 could be choosed 20160113
	sensor_write_register(0x3620,0x42);  //comp and bitline current
	sensor_write_register(0x363a,0x0c);  //sa1 common voltage
	sensor_write_register(0x3334,0x60);  //column fpn
	  
	sensor_write_register(0x303f,0x81);  //format
	sensor_write_register(0x501f,0x00);  
	sensor_write_register(0x3b00,0xf8);  
	sensor_write_register(0x3b01,0x40);  
	sensor_write_register(0x3c01,0x14);  
	sensor_write_register(0x4000,0x00);  
	  
	sensor_write_register(0x3d08,0x00);  //data output DVP CLK INV
	sensor_write_register(0x3640,0x00);  // pad driver
	  
	sensor_write_register(0x0100,0x01);  
	sensor_write_register(0x303a,0x07);  // PLL  67.5M pclk
	sensor_write_register(0x3039,0x8e);   
	sensor_write_register(0x303f,0x82);  
	sensor_write_register(0x3636,0x88);  //lpDVDD
	sensor_write_register(0x3631,0x80);  //0820  20160113  20160120
	sensor_write_register(0x3635,0x66);  //1018  20160113  20160120
	sensor_write_register(0x3105,0x04);  //1018
	sensor_write_register(0x3105,0x04);  //1018

#endif

#if 0
sensor_write_register(0x3e01,0x46);  //exp time
sensor_write_register(0x3e02,0x10);  //exp time
sensor_write_register(0x3e08,0x7b);  //exp time
sensor_write_register(0x3e09,0x1f);  //exp time
#endif

    bSensorInit = HI_TRUE;
    printf("=========================================================\n");
    printf("===sc2035 sensor 1080P30fps(Parallel port) init success222!=====\n");
    printf("=========================================================\n");


    return;
}


