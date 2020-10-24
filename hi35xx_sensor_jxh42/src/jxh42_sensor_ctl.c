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

int sensor_write_reg(char addr, char data)
{
    if(flag_init == 0)
    {
        sensor_i2c_init();
        flag_init = 1;
    }

    int ret;
    char buf[3];

    buf[0] = addr;
    buf[1] = data;

    ret = ioctl(g_fd, I2C_16BIT_REG, 0);
    if (ret < 0)
    {
        printf("CMD_SET_REG_WIDTH error!\n");
        return -1;
    }

    ret = ioctl(g_fd, I2C_16BIT_DATA, 0);
    if (ret)
    {
        printf("hi_i2c write faild!\n");
        return -1;
    }

    printf("I2C write (addr, value): %02X %02X\n", buf[0], buf[1]);
    ret = write(g_fd, buf, 2);
    if(ret < 0)
    {
    	printf("I2C_WRITE error %d!\n", ret);
    	return -1;
    }

	return 0;
}

static void delay_ms(int ms) { 
    usleep(ms*1000);
}

void sensor_prog(int* rom) 
{
    int i = 0;
    printf("Called sensor_prog!\n");
    /*while (1) {
        int lookup = rom[i++];
        int addr = (lookup >> 16) & 0xFFFF;
        int data = lookup & 0xFFFF;
        if (addr == 0xFFFE) {
            delay_ms(data);
        } else if (addr == 0xFFFF) {
            return;
        } else {
            sensor_write_reg(addr, data);
        }
    }*/
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
    /*sensor_write_reg(0x12, 0x40);
    sensor_write_reg(0xC, 0x40);
    sensor_write_reg(0xD, 0x40);
    sensor_write_reg(0x1F, 0x40);
    sensor_write_reg(0xE, 0x1D);
    sensor_write_reg(0xF, 0x1D);
    sensor_write_reg(0x10, 0x1E);
    sensor_write_reg(0x11, 0x80);
    sensor_write_reg(0x20, 0x40);
    sensor_write_reg(0x21, 0x40);
    sensor_write_reg(0x22, 0xEE);
    sensor_write_reg(0x23, 0xEE);
    sensor_write_reg(0x24, 0xEE);
    sensor_write_reg(0x25, 0xDC);
    sensor_write_reg(0x26, 0x25);
    sensor_write_reg(0x27, 0x3B);
    sensor_write_reg(0x28, 0xD);
    sensor_write_reg(0x29, 0xD);
    sensor_write_reg(0x2A, 0x24);
    sensor_write_reg(0x2B, 0x29);
    sensor_write_reg(0x2C, 0x29);
    sensor_write_reg(0x2D, 0x29);
    sensor_write_reg(0x2E, 0xB9);
    sensor_write_reg(0x2F, 0xB9);
    sensor_write_reg(0x30, 0x92);
    sensor_write_reg(0x31, 0xA);
    sensor_write_reg(0x32, 0xAA);
    sensor_write_reg(0x33, 0x14);
    sensor_write_reg(0x34, 0x38);
    sensor_write_reg(0x35, 0x54);
    sensor_write_reg(0x42, 0x41);
    sensor_write_reg(0x43, 0x50);
    sensor_write_reg(0x1D, 0xFF);
    sensor_write_reg(0x1E, 0x9F);
    sensor_write_reg(0x6C, 0x90);
    sensor_write_reg(0x73, 0xB3);
    sensor_write_reg(0x70, 0x68);
    sensor_write_reg(0x76, 0x40);
    sensor_write_reg(0x77, 0x40);
    sensor_write_reg(0x60, 0xA4);
    sensor_write_reg(0x61, 0xFF);
    sensor_write_reg(0x62, 0x40);
    sensor_write_reg(0x65, 0x40);
    sensor_write_reg(0x66, 0x20);
    sensor_write_reg(0x67, 0x30);
    sensor_write_reg(0x68, 0x30);
    sensor_write_reg(0x69, 0x74);
    sensor_write_reg(0x6F, 0x74);
    sensor_write_reg(0x63, 0x51);
    sensor_write_reg(0x6A, 0x51);
    sensor_write_reg(0x13, 0x87);
    sensor_write_reg(0x14, 0x80);
    sensor_write_reg(0x16, 0xC0);
    sensor_write_reg(0x17, 0x40);
    sensor_write_reg(0x18, 0xBB);
    sensor_write_reg(0x38, 0x35);
    sensor_write_reg(0x39, 0x98);
    sensor_write_reg(0x4A, 0x98);
    sensor_write_reg(0x48, 0x40);
    sensor_write_reg(0x49, 0x10);
    sensor_write_reg(0x12, 0x00);*/
    
    // From HexRays
  sensor_write_reg(0x12, 0x40);
  sensor_write_reg(12, 64);
  sensor_write_reg(13, 64);
  sensor_write_reg(31, 4);
  sensor_write_reg(14, 29);
  sensor_write_reg(15, 9);
  sensor_write_reg(16, 30);
  sensor_write_reg(17, 128);
  sensor_write_reg(32, 64);
  sensor_write_reg(33, 6);
  sensor_write_reg(34, 238);
  sensor_write_reg(35, 2);
  sensor_write_reg(36, 0);
  sensor_write_reg(37, 220);
  sensor_write_reg(38, 37);
  sensor_write_reg(39, 59);
  sensor_write_reg(40, 13);
  sensor_write_reg(41, 1);
  sensor_write_reg(42, 36);
  sensor_write_reg(43, 41);
  sensor_write_reg(44, 4);
  sensor_write_reg(45, 0);
  sensor_write_reg(46, 185);
  sensor_write_reg(47, 0);
  sensor_write_reg(48, 146);
  sensor_write_reg(49, 10);
  sensor_write_reg(50, 170);
  sensor_write_reg(51, 20);
  sensor_write_reg(52, 56);
  sensor_write_reg(53, 84);
  sensor_write_reg(66, 65);
  sensor_write_reg(67, 80);
  sensor_write_reg(29, 255);
  sensor_write_reg(30, 159);
  sensor_write_reg(108, 144);
  sensor_write_reg(115, 179);
  sensor_write_reg(112, 104);
  sensor_write_reg(118, 64);
  sensor_write_reg(119, 6);
  sensor_write_reg(96, 164);
  sensor_write_reg(97, 255);
  sensor_write_reg(98, 64);
  sensor_write_reg(101, 0);
  sensor_write_reg(102, 32);
  sensor_write_reg(103, 48);
  sensor_write_reg(104, 4);
  sensor_write_reg(105, 116);
  sensor_write_reg(111, 4);
  sensor_write_reg(99, 81);
  sensor_write_reg(106, 9);
  sensor_write_reg(19, 135);
  sensor_write_reg(20, 128);
  sensor_write_reg(22, 192);
  sensor_write_reg(23, 64);
  sensor_write_reg(24, 187);
  sensor_write_reg(56, 53);
  sensor_write_reg(57, 152);
  sensor_write_reg(74, 3);
  sensor_write_reg(72, 64);
  sensor_write_reg(73, 16);
  sensor_write_reg(18, 0);

  /*sensor_write_reg(18, 64);
  sensor_write_reg(12, 64);
  sensor_write_reg(13, 64);
  sensor_write_reg(31, 4);
  sensor_write_reg(14, 29);
  sensor_write_reg(15, 9);
  sensor_write_reg(16, 30);
  sensor_write_reg(17, -128);
  sensor_write_reg(32, 64);
  sensor_write_reg(33, 6);
  sensor_write_reg(34, -18);
  sensor_write_reg(35, 2);
  sensor_write_reg(36, 0);
  sensor_write_reg(37, -36);
  sensor_write_reg(38, 37);
  sensor_write_reg(39, 59);
  sensor_write_reg(40, 13);
  sensor_write_reg(41, 1);
  sensor_write_reg(42, 36);
  sensor_write_reg(43, 41);
  sensor_write_reg(44, 4);
  sensor_write_reg(45, 0);
  sensor_write_reg(46, -71);
  sensor_write_reg(47, 0);
  sensor_write_reg(48, -110);
  sensor_write_reg(49, 10);
  sensor_write_reg(50, -86);
  sensor_write_reg(51, 20);
  sensor_write_reg(52, 56);
  sensor_write_reg(53, 84);
  sensor_write_reg(66, 65);
  sensor_write_reg(67, 80);
  sensor_write_reg(29, -1);
  sensor_write_reg(30, -97);
  sensor_write_reg(108, -112);
  sensor_write_reg(115, -77);
  sensor_write_reg(112, 104);
  sensor_write_reg(118, 64);
  sensor_write_reg(119, 6);
  sensor_write_reg(96, -92);
  sensor_write_reg(97, -1);
  sensor_write_reg(98, 64);
  sensor_write_reg(101, 0);
  sensor_write_reg(102, 32);
  sensor_write_reg(103, 48);
  sensor_write_reg(104, 4);
  sensor_write_reg(105, 116);
  sensor_write_reg(111, 4);
  sensor_write_reg(99, 81);
  sensor_write_reg(106, 9);
  sensor_write_reg(19, -121);
  sensor_write_reg(20, -128);
  sensor_write_reg(22, -64);
  sensor_write_reg(23, 64);
  sensor_write_reg(24, -69);
  sensor_write_reg(56, 53);
  sensor_write_reg(57, -104);
  sensor_write_reg(74, 3);
  sensor_write_reg(72, 64);
  sensor_write_reg(73, 16);
  sensor_write_reg(18, 0);*/
  
    bSensorInit = HI_TRUE;
    printf("=========================================================\n");
    printf("===SOI H42 based on ominivision ov9712 sensor 720P30fps(Parallel port) patches by Z ver 4=====\n");
    printf("=========================================================\n");

    return;
}


