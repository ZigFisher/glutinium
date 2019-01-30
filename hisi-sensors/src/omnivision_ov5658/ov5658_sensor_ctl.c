/******************************************************************************

  Copyright (C), 2001-2013, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : imx178_sensor_ctl.c
  Version       : Initial Draft
  Author        : Hisilicon BVT ISP group
  Created       : 2014/02/20
  Description   : Sony IMX178 sensor driver
  History       :
  1.Date        : 2014/02/20
  Author        : yy
  Modification  : Created file

******************************************************************************/

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


const unsigned char sensor_i2c_addr     =    0x6c; /* I2C Address of OV5658 */       //0x6c or 0x20
const unsigned int  sensor_addr_byte    =    2;
const unsigned int  sensor_data_byte    =    1;
static int g_fd = -1;

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

void sensor_init_5M30();

void sensor_init()
{
    sensor_i2c_init();
    
    if (1 == gu8SensorImageMode)    /* SENSOR_5M_30FPS_MODE */
    {
        sensor_init_5M30();
        bSensorInit = HI_TRUE;
    }
    else
    {
        printf("Not support this mode\n");
    }
}

void sensor_exit()
{
    sensor_i2c_exit();

    return;
}

// Full 4lane 2592x1944 30fps
void sensor_init_5M30()
{
    sensor_write_register(0x0103, 0x01); 
    sensor_write_register(0x3210, 0x43);
    sensor_write_register(0x3001, 0x0e);
    sensor_write_register(0x3002, 0xc0);
    sensor_write_register(0x3011, 0x41);
    sensor_write_register(0x3012, 0x0a);
    sensor_write_register(0x3013, 0x50);
    sensor_write_register(0x3015, 0x09);
    sensor_write_register(0x3018, 0xf0);
    sensor_write_register(0x3021, 0x40);
    sensor_write_register(0x3500, 0x00);
    sensor_write_register(0x3501, 0x7b);
    sensor_write_register(0x3502, 0x00);
    sensor_write_register(0x3503, 0x07);
    sensor_write_register(0x3505, 0x00);
    sensor_write_register(0x3506, 0x00);
    sensor_write_register(0x3507, 0x02);
    sensor_write_register(0x3508, 0x00);
    sensor_write_register(0x3509, 0x08);
    sensor_write_register(0x350a, 0x00);
    sensor_write_register(0x350b, 0x80);
    sensor_write_register(0x3600, 0x4b);    
    sensor_write_register(0x3602, 0x3c);
    sensor_write_register(0x3605, 0x14);
    sensor_write_register(0x3606, 0x09);
    sensor_write_register(0x3612, 0x04);
    sensor_write_register(0x3613, 0x66);
    sensor_write_register(0x3614, 0x39);
    sensor_write_register(0x3615, 0x33);
    sensor_write_register(0x3616, 0x46);
    sensor_write_register(0x361a, 0x0a);
    sensor_write_register(0x361c, 0x76);
    sensor_write_register(0x3620, 0x40);
    sensor_write_register(0x3640, 0x03);
    sensor_write_register(0x3641, 0x60);
    sensor_write_register(0x3642, 0x00);
    sensor_write_register(0x3643, 0x90);
    sensor_write_register(0x3660, 0x04);
    sensor_write_register(0x3665, 0x00);
    sensor_write_register(0x3666, 0x20);
    sensor_write_register(0x3667, 0x00);
    sensor_write_register(0x366a, 0x80);
    sensor_write_register(0x3680, 0xe0);
    sensor_write_register(0x3692, 0x80);
    sensor_write_register(0x3700, 0x42);
    sensor_write_register(0x3701, 0x14);
    sensor_write_register(0x3702, 0xe8);
    sensor_write_register(0x3703, 0x20);
    sensor_write_register(0x3704, 0x5e);
    sensor_write_register(0x3705, 0x02);
    sensor_write_register(0x3708, 0xe3);
    sensor_write_register(0x3709, 0xc3);
    sensor_write_register(0x370a, 0x00);
    sensor_write_register(0x370b, 0x20);
    sensor_write_register(0x370c, 0x0c);
    sensor_write_register(0x370d, 0x11);
    sensor_write_register(0x370e, 0x00);
    sensor_write_register(0x370f, 0x40);
    sensor_write_register(0x3710, 0x00);
    sensor_write_register(0x3715, 0x09);
    sensor_write_register(0x371a, 0x04);
    sensor_write_register(0x371b, 0x05);
    sensor_write_register(0x371c, 0x01);
    sensor_write_register(0x371e, 0xa1);
    sensor_write_register(0x371f, 0x18);
    sensor_write_register(0x3721, 0x00);
    sensor_write_register(0x3726, 0x00);
    sensor_write_register(0x372a, 0x01);
    sensor_write_register(0x3730, 0x10);
    sensor_write_register(0x3738, 0x22);
    sensor_write_register(0x3739, 0xe5);
    sensor_write_register(0x373a, 0x50);
    sensor_write_register(0x373b, 0x02);
    sensor_write_register(0x373c, 0x2c);
    sensor_write_register(0x373f, 0x02);
    sensor_write_register(0x3740, 0x42);
    sensor_write_register(0x3741, 0x02);
    sensor_write_register(0x3743, 0x01);
    sensor_write_register(0x3744, 0x02);
    sensor_write_register(0x3747, 0x00);
    sensor_write_register(0x3754, 0xc0);
    sensor_write_register(0x3755, 0x07);
    sensor_write_register(0x3756, 0x1a);
    sensor_write_register(0x3759, 0x0f);
    sensor_write_register(0x375c, 0x04);
    sensor_write_register(0x3767, 0x00);
    sensor_write_register(0x376b, 0x44);
    sensor_write_register(0x377b, 0x44);
    sensor_write_register(0x377c, 0x30);
    sensor_write_register(0x377e, 0x30);
    sensor_write_register(0x377f, 0x08);
    sensor_write_register(0x3781, 0x0c);
    sensor_write_register(0x3785, 0x1e);
    sensor_write_register(0x378f, 0xf5);
    sensor_write_register(0x3791, 0xb0);
    sensor_write_register(0x379c, 0x0c);
    sensor_write_register(0x379d, 0x20);
    sensor_write_register(0x379e, 0x00);
    sensor_write_register(0x3796, 0x72);
    sensor_write_register(0x379a, 0x07);
    sensor_write_register(0x379b, 0xb0);
    sensor_write_register(0x37c5, 0x00);
    sensor_write_register(0x37c6, 0x00);
    sensor_write_register(0x37c7, 0x00);
    sensor_write_register(0x37c9, 0x00);
    sensor_write_register(0x37ca, 0x00);
    sensor_write_register(0x37cb, 0x00);
    sensor_write_register(0x37cc, 0x00);
    sensor_write_register(0x37cd, 0x00);
    sensor_write_register(0x37ce, 0x01);
    sensor_write_register(0x37cf, 0x00);
    sensor_write_register(0x37d1, 0x00);
    sensor_write_register(0x37de, 0x00);
    sensor_write_register(0x37df, 0x00);
    sensor_write_register(0x3800, 0x00);
    sensor_write_register(0x3801, 0x00);
    sensor_write_register(0x3802, 0x00);
    sensor_write_register(0x3803, 0x00);
    sensor_write_register(0x3804, 0x0a);
    sensor_write_register(0x3805, 0x3f);
    sensor_write_register(0x3806, 0x07);
    sensor_write_register(0x3807, 0xa3);
    sensor_write_register(0x3808, 0x0a);
    sensor_write_register(0x3809, 0x20);        //output windows size

    sensor_write_register(0x380a, 0x07);
    sensor_write_register(0x380b, 0x98);
    sensor_write_register(0x380c, 0x0c);
    sensor_write_register(0x380d, 0x98);
    sensor_write_register(0x380e, 0x07);
    sensor_write_register(0x380f, 0xc0);
    sensor_write_register(0x3810, 0x00);
    sensor_write_register(0x3811, 0x10);
    sensor_write_register(0x3812, 0x00);
    sensor_write_register(0x3813, 0x06);
    sensor_write_register(0x3814, 0x11);
    sensor_write_register(0x3815, 0x11);

    sensor_write_register(0x3820, 0x10);  //org
    //sensor_write_register(0x3820, 0x42);    //mirror
    sensor_write_register(0x3821, 0x1e);  //org
    //sensor_write_register(0x3821, 0x00);
    
    sensor_write_register(0x3823, 0x00);
    sensor_write_register(0x3824, 0x00);
    sensor_write_register(0x3825, 0x00);
    sensor_write_register(0x3826, 0x00);
    sensor_write_register(0x3827, 0x00);
    sensor_write_register(0x3829, 0x0b);
    sensor_write_register(0x382a, 0x04);
    sensor_write_register(0x382c, 0x34);
    sensor_write_register(0x382d, 0x00);
    sensor_write_register(0x3a04, 0x06);
    sensor_write_register(0x3a05, 0x14);
    sensor_write_register(0x3a06, 0x00);
    sensor_write_register(0x3a07, 0xfe);
    sensor_write_register(0x3b00, 0x00);
    sensor_write_register(0x3b02, 0x00);
    sensor_write_register(0x3b03, 0x00);
    sensor_write_register(0x3b04, 0x00);
    sensor_write_register(0x3b05, 0x00);
    sensor_write_register(0x4000, 0x18);
    sensor_write_register(0x4001, 0x04);
    sensor_write_register(0x4002, 0x45);
    sensor_write_register(0x4004, 0x04);
    sensor_write_register(0x4005, 0x18);
    sensor_write_register(0x4006, 0x20);
    sensor_write_register(0x4007, 0x98);
    sensor_write_register(0x4008, 0x24);
    sensor_write_register(0x4009, 0x10);
    sensor_write_register(0x400c, 0x00);
    sensor_write_register(0x400d, 0x00);
    sensor_write_register(0x404e, 0x37);
    sensor_write_register(0x404f, 0x8f);
    sensor_write_register(0x4058, 0x00);
    sensor_write_register(0x4100, 0x50);
    sensor_write_register(0x4101, 0x34);
    sensor_write_register(0x4102, 0x34);
    sensor_write_register(0x4104, 0xde);
    sensor_write_register(0x4300, 0xff);
    sensor_write_register(0x4307, 0x30);
    sensor_write_register(0x4311, 0x04);
    sensor_write_register(0x4315, 0x01);
    sensor_write_register(0x4501, 0x08);
    sensor_write_register(0x4502, 0x08);
    sensor_write_register(0x4816, 0x52);
    sensor_write_register(0x481f, 0x30);
    sensor_write_register(0x4826, 0x28);
    sensor_write_register(0x4837, 0x0d);
    sensor_write_register(0x4a00, 0xaa);
    sensor_write_register(0x4a02, 0x00);
    sensor_write_register(0x4a03, 0x01);
    sensor_write_register(0x4a05, 0x40);
    sensor_write_register(0x4a0a, 0x88);
    sensor_write_register(0x5000, 0x06);
    sensor_write_register(0x5001, 0x01);
    sensor_write_register(0x5002, 0x00);
    sensor_write_register(0x5003, 0x20);
    sensor_write_register(0x5013, 0x00);
    sensor_write_register(0x501f, 0x00);
    sensor_write_register(0x5043, 0x48);
    sensor_write_register(0x5780, 0x1c);
    sensor_write_register(0x5786, 0x20);
    sensor_write_register(0x5788, 0x18);
    sensor_write_register(0x578a, 0x04);
    sensor_write_register(0x578b, 0x02);
    sensor_write_register(0x578c, 0x02);
    sensor_write_register(0x578e, 0x06);
    sensor_write_register(0x578f, 0x02);
    sensor_write_register(0x5790, 0x02);
    sensor_write_register(0x5791, 0xff);
    sensor_write_register(0x5e00, 0x00);
    sensor_write_register(0x5e10, 0x0c);
    sensor_write_register(0x0100, 0x01);

    usleep(10000);

    sensor_write_register(0x5800, 0x22);
    sensor_write_register(0x5801, 0x11);
    sensor_write_register(0x5802, 0x0d);
    sensor_write_register(0x5803, 0x0d);
    sensor_write_register(0x5804, 0x12);
    sensor_write_register(0x5805, 0x26);
    sensor_write_register(0x5806, 0x09);
    sensor_write_register(0x5807, 0x07);
    sensor_write_register(0x5808, 0x05);
    sensor_write_register(0x5809, 0x05);
    sensor_write_register(0x580a, 0x07);
    sensor_write_register(0x580b, 0x0a);
    sensor_write_register(0x580c, 0x07);
    sensor_write_register(0x580d, 0x02);
    sensor_write_register(0x580e, 0x00);
    sensor_write_register(0x580f, 0x00);
    sensor_write_register(0x5810, 0x03);
    sensor_write_register(0x5811, 0x07);
    sensor_write_register(0x5812, 0x06);
    sensor_write_register(0x5813, 0x02);
    sensor_write_register(0x5814, 0x00);
    sensor_write_register(0x5815, 0x00);
    sensor_write_register(0x5816, 0x03);
    sensor_write_register(0x5817, 0x07);
    sensor_write_register(0x5818, 0x09);
    sensor_write_register(0x5819, 0x05);
    sensor_write_register(0x581a, 0x04);
    sensor_write_register(0x581b, 0x04);
    sensor_write_register(0x581c, 0x07);
    sensor_write_register(0x581d, 0x09);
    sensor_write_register(0x581e, 0x1d);
    sensor_write_register(0x581f, 0x0e);
    sensor_write_register(0x5820, 0x0b);
    sensor_write_register(0x5821, 0x0b);
    sensor_write_register(0x5822, 0x0f);
    sensor_write_register(0x5823, 0x1e);
    sensor_write_register(0x5824, 0x59);
    sensor_write_register(0x5825, 0x46);
    sensor_write_register(0x5826, 0x37);
    sensor_write_register(0x5827, 0x36);
    sensor_write_register(0x5828, 0x45);
    sensor_write_register(0x5829, 0x39);
    sensor_write_register(0x582a, 0x46);
    sensor_write_register(0x582b, 0x44);
    sensor_write_register(0x582c, 0x45);
    sensor_write_register(0x582d, 0x28);
    sensor_write_register(0x582e, 0x38);
    sensor_write_register(0x582f, 0x52);
    sensor_write_register(0x5830, 0x60);
    sensor_write_register(0x5831, 0x51);
    sensor_write_register(0x5832, 0x26);
    sensor_write_register(0x5833, 0x38);
    sensor_write_register(0x5834, 0x43);
    sensor_write_register(0x5835, 0x42);
    sensor_write_register(0x5836, 0x34);
    sensor_write_register(0x5837, 0x18);
    sensor_write_register(0x5838, 0x05);
    sensor_write_register(0x5839, 0x27);
    sensor_write_register(0x583a, 0x27);
    sensor_write_register(0x583b, 0x27);
    sensor_write_register(0x583c, 0x0a);
    sensor_write_register(0x583d, 0xbf);
    sensor_write_register(0x5842, 0x01);
    sensor_write_register(0x5843, 0x2b);
    sensor_write_register(0x5844, 0x01);
    sensor_write_register(0x5845, 0x92);
    sensor_write_register(0x5846, 0x01);
    sensor_write_register(0x5847, 0x8f);
    sensor_write_register(0x5848, 0x01);
    sensor_write_register(0x5849, 0x0c);
    
    printf("-------OV ov5658 Sensor 5M30fps Initial OK!-------\n");

}
