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

const unsigned char sensor_i2c_addr	    =	0x60;		/* I2C Address of OV9712 */
const unsigned int  sensor_addr_byte	=	1;
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

void sensor_linear_720p25_init();

#define SENSOR_720P_25FPS_MODE  (1)

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
    sensor_linear_720p25_init();

    return ;
}

void sensor_exit()
{
    sensor_i2c_exit();
	flag_init = 0;
    return;
}

/* 720P30 and 720P25 */
void sensor_linear_720p25_init()
{
    //Reset 
    sensor_write_register(0x12, 0x80);
    sensor_write_register(0x09, 0x10);

    //Core Settings
    sensor_write_register(0x1e, 0x07);
    sensor_write_register(0x5f, 0x18);
    sensor_write_register(0x69, 0x04);
    sensor_write_register(0x65, 0x2a);
    sensor_write_register(0x68, 0x0a);
    sensor_write_register(0x39, 0x28);
    sensor_write_register(0x4d, 0x90);
    sensor_write_register(0xc1, 0x80);
    sensor_write_register(0x0c, 0x30);
    sensor_write_register(0x6d, 0x02);

    //DSP
    //sensor_write_register(0x96, 0xf1);
    sensor_write_register(0x96, 0x01);
    sensor_write_register(0xbc, 0x68);

    //Resolution and Format
    sensor_write_register(0x12, 0x00);
    sensor_write_register(0x3b, 0x00);
    sensor_write_register(0x97, 0x80);
    sensor_write_register(0x17, 0x25);
    sensor_write_register(0x18, 0xA2);
    sensor_write_register(0x19, 0x01);
    sensor_write_register(0x1a, 0xCA);
    sensor_write_register(0x03, 0x0A);
    sensor_write_register(0x32, 0x07);
    sensor_write_register(0x98, 0x00);
    sensor_write_register(0x99, 0x28);
    sensor_write_register(0x9a, 0x00);
    sensor_write_register(0x57, 0x00);
    sensor_write_register(0x58, 0xB4);
    sensor_write_register(0x59, 0xA0);
    sensor_write_register(0x4c, 0x13);
    sensor_write_register(0x4b, 0x36);
    sensor_write_register(0x3d, 0x3c);
    sensor_write_register(0x3e, 0x03);
    sensor_write_register(0xbd, 0xA0);
    sensor_write_register(0xbe, 0xb4);
    sensor_write_register(0x37, 0x02);
    sensor_write_register(0x60, 0x9d);

    //YAVG
    sensor_write_register(0x4e, 0x55);
    sensor_write_register(0x4f, 0x55);
    sensor_write_register(0x50, 0x55);
    sensor_write_register(0x51, 0x55);
    sensor_write_register(0x24, 0x55);
    sensor_write_register(0x25, 0x40);
    sensor_write_register(0x26, 0xa1);

    //Clock
    sensor_write_register(0x5c, 0x52);
    sensor_write_register(0x5d, 0x00);
    sensor_write_register(0x11, 0x01);
    sensor_write_register(0x2a, 0x9c);
    sensor_write_register(0x2b, 0x06);
    sensor_write_register(0x2d, 0x00);
    sensor_write_register(0x2e, 0x00);

    //General
    sensor_write_register(0x13, 0xA5);
    sensor_write_register(0x14, 0x40);

    //Banding
    sensor_write_register(0x4a, 0x00);
    sensor_write_register(0x49, 0xce);
    sensor_write_register(0x22, 0x03);
    sensor_write_register(0x09, 0x00);

    //close AE_AWB
    sensor_write_register(0x13, 0x80);
    sensor_write_register(0x16, 0x00);
    sensor_write_register(0x10, 0xf0);
    sensor_write_register(0x00, 0x3f);
    sensor_write_register(0x38, 0x00);
    sensor_write_register(0x01, 0x40);
    sensor_write_register(0x02, 0x40);
    sensor_write_register(0x05, 0x40);
    sensor_write_register(0x06, 0x00);
    sensor_write_register(0x07, 0x00);

    //BLC
    sensor_write_register(0x41, 0x84);

    bSensorInit = HI_TRUE;
    printf("=========================================================\n");
    printf("===ominivision ov9712 sensor 720P30fps(Parallel port) init success!=====\n");
    printf("=========================================================\n");

    return;
}


