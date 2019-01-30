#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef HI_GPIO_I2C
#include "gpioi2c_ex.h"
#include "gpio_i2c.h"
#else
#include "hi_i2c.h"
#endif

const unsigned char sensor_i2c_addr	=	0x64;		
const unsigned int  sensor_addr_byte	=	1;
const unsigned int  sensor_data_byte	=	1;

int sensor_read_register(int addr)
{
#ifdef HI_GPIO_I2C
    int fd = -1;
    int ret;
    int value;

    fd = open("/dev/gpioi2c_ov", 0);
    if(fd<0)
    {
        printf("Open gpioi2c_ov error!\n");
        return -1;
    }

    value = ((sensor_i2c_addr&0xff)<<24) | ((addr&0xff)<<16);

    ret = ioctl(fd, GPIO_I2C_READ, &value);
    if (ret)
    {
        printf("GPIO-I2C read faild!\n");
        return -1;
    }

    value &= 0xff;

    close(fd);
    return value;
#else
    int fd = -1;
    int ret;
    I2C_DATA_S i2c_data;

    fd = open("/dev/hi_i2c", 0);
    if(fd<0)
    {
        printf("Open hi_i2c error!\n");
        return -1;
    }

    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sensor_addr_byte;
    i2c_data.data_byte_num = sensor_data_byte;

    ret = ioctl(fd, CMD_I2C_READ, &i2c_data);

    if (ret)
    {
        printf("hi_i2c read faild!\n");
        return -1;
    }

    close(fd);
#endif

    return i2c_data.data;

	
	return 0;
}

int sensor_write_register(int addr, int data)
{
#ifdef HI_GPIO_I2C
    int fd = -1;
    int ret;
    I2C_DATA_S i2c_data;
    
    fd = open("/dev/gpioi2c_ex", 0);
    if(fd<0)
    {
        printf("Open gpioi2c_ex error!\n");
        return -1;
    }

    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sensor_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sensor_data_byte;

    ret = ioctl(fd, GPIO_I2C_WRITE, &i2c_data);

    if (ret)
    {
        printf("GPIO-I2C write faild!\n");
        return -1;
    }

    close(fd);
#else
    int fd = -1;
    int ret;
    I2C_DATA_S i2c_data;
	
    fd = open("/dev/hi_i2c", 0);
    if(fd<0)
    {
        printf("Open hi_i2c error!\n");
        return -1;
    }
    
    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sensor_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sensor_data_byte;

    ret = ioctl(fd, CMD_I2C_WRITE, &i2c_data);

    if (ret)
    {
        printf("hi_i2c write faild!\n");
        return -1;
    }

    close(fd);
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

void sensor_init()
{
    printf("-----------start init pixel plus 3100K------------");
    sensor_write_register(0x03,0x00);   // --------  A
    sensor_write_register(0x2D,0x01);
    sensor_write_register(0x29,0x9D);
    sensor_write_register(0x04,0x02);
    sensor_write_register(0x05,0x03);
    sensor_write_register(0x06,0x06);
    sensor_write_register(0x07,0x71);
    sensor_write_register(0x08,0x02);
    sensor_write_register(0x09,0xEE);
    sensor_write_register(0x0A,0x02);
    sensor_write_register(0x0B,0xEE);
    sensor_write_register(0x0C,0x00);
    sensor_write_register(0x0D,0x05);
    sensor_write_register(0x0E,0x00);
    sensor_write_register(0x0F,0x05);
    sensor_write_register(0x10,0x05);
    sensor_write_register(0x11,0x04);
    sensor_write_register(0x12,0x02);
    sensor_write_register(0x13,0xD4);
    sensor_write_register(0x14,0x00);
    sensor_write_register(0x15,0x17);
    sensor_write_register(0x16,0x02);
    sensor_write_register(0x17,0xE8);

    sensor_write_register(0x03,0x05); 
    sensor_write_register(0x04,0x00); 
    sensor_write_register(0x03,0x00); 

    sensor_write_register(0x18,0x00);
    sensor_write_register(0x19,0x00);
    sensor_write_register(0x1A,0x00);
    sensor_write_register(0x1B,0x02);
    sensor_write_register(0x1C,0x00);
    sensor_write_register(0x1D,0x00);
    sensor_write_register(0x1E,0x00);
    sensor_write_register(0x1F,0x00);
    sensor_write_register(0x20,0x08);
    sensor_write_register(0x21,0x00);
    sensor_write_register(0x22,0x00);
    sensor_write_register(0x23,0x50);
    sensor_write_register(0x24,0x00);
    sensor_write_register(0x25,0x08);
    sensor_write_register(0x26,0x41);
    sensor_write_register(0x27,0x66);
    sensor_write_register(0x28,0x30);
    sensor_write_register(0x2A,0x00);
    sensor_write_register(0x2B,0xC0);
    sensor_write_register(0x2C,0x00);
    sensor_write_register(0x2E,0x00);
    sensor_write_register(0x2F,0x00);
    sensor_write_register(0x30,0x00);
    sensor_write_register(0x31,0x00);
    sensor_write_register(0x32,0x00);
    sensor_write_register(0x33,0x02);
    sensor_write_register(0x34,0x01);
    sensor_write_register(0x35,0x2F);
    sensor_write_register(0x36,0x80);
    sensor_write_register(0x37,0x44);
    sensor_write_register(0x38,0x58);
    sensor_write_register(0x39,0x40);
    sensor_write_register(0x3A,0x18);
    sensor_write_register(0x3B,0x20);
    sensor_write_register(0x3C,0x00);
    sensor_write_register(0x3D,0x00);
    sensor_write_register(0x3E,0x00);
    sensor_write_register(0x3F,0x00);
    sensor_write_register(0x41,0x02);
    sensor_write_register(0x42,0x0B);
    sensor_write_register(0x40,0x3C);	// pll
    sensor_write_register(0x4F,0x40);
    sensor_write_register(0x50,0x00);
    sensor_write_register(0x51,0x05);
    sensor_write_register(0x52,0x79);
    sensor_write_register(0x53,0x04);
    sensor_write_register(0x54,0x8D);
    sensor_write_register(0x55,0x40);
    sensor_write_register(0x56,0x00);
    sensor_write_register(0x57,0x00);
    sensor_write_register(0x58,0x40);
    sensor_write_register(0x59,0x00);
    sensor_write_register(0x5A,0xBB);
    sensor_write_register(0x5B,0x80);
    sensor_write_register(0x5C,0x00);
    sensor_write_register(0x5D,0xE1);
    sensor_write_register(0x5E,0x00);
    sensor_write_register(0x5F,0x04);
    sensor_write_register(0x60,0x65);
    sensor_write_register(0xD0,0x02);
    sensor_write_register(0xD1,0x05);
    sensor_write_register(0xD2,0x00);
    sensor_write_register(0xD3,0x80);
    sensor_write_register(0xD4,0x05);
    sensor_write_register(0xD5,0x00); // -- 
    sensor_write_register(0xD6,0x02);
    sensor_write_register(0xD7,0xD0);
    sensor_write_register(0xD8,0x00);
    sensor_write_register(0xD9,0x00);
    sensor_write_register(0xDA,0x00);
    sensor_write_register(0xDB,0x00);
    sensor_write_register(0xDC,0x02);
    sensor_write_register(0xDD,0xEE);
    sensor_write_register(0xDE,0x00);
    sensor_write_register(0xDF,0x00);
    sensor_write_register(0xE0,0x1A);
    sensor_write_register(0xE1,0x00);
    sensor_write_register(0xE2,0x00);
    sensor_write_register(0xE3,0x01);
    sensor_write_register(0xE4,0x00);
    sensor_write_register(0xE5,0x00);
    sensor_write_register(0xE6,0x00);
    sensor_write_register(0xE7,0xFF);
    sensor_write_register(0xE8,0x00);
    sensor_write_register(0xE9,0x00);
    sensor_write_register(0xEA,0x00);
    sensor_write_register(0xEB,0x00);
    sensor_write_register(0xEC,0x00);
    sensor_write_register(0xED,0x00);
    sensor_write_register(0xEE,0x00);
    sensor_write_register(0xEF,0x00);
    sensor_write_register(0xF0,0xB6);
    sensor_write_register(0xF1,0xAB);
    sensor_write_register(0xF2,0x9D);
    sensor_write_register(0xF3,0x80);
    sensor_write_register(0xF4,0xF1);
    sensor_write_register(0xF5,0xEC);
    sensor_write_register(0xF6,0xDA);
    sensor_write_register(0xF7,0xC7);
    sensor_write_register(0xF8,0xFF);
    sensor_write_register(0xF9,0x00);
    sensor_write_register(0xFA,0x80);
    sensor_write_register(0xFB,0x10);
    sensor_write_register(0xFC,0x00);
    sensor_write_register(0xFD,0x02);
    sensor_write_register(0xFE,0x80);
    sensor_write_register(0xFF,0x00);
    sensor_write_register(0x03,0x01);   // --------  B
    sensor_write_register(0x04,0xF0);
    sensor_write_register(0x05,0x02);
    sensor_write_register(0x06,0x02);
    sensor_write_register(0x07,0x67);
    sensor_write_register(0x08,0x20);
    sensor_write_register(0x09,0x04);
    sensor_write_register(0x0A,0xF0);
    sensor_write_register(0x0B,0x10);
    sensor_write_register(0x0C,0x00);
    sensor_write_register(0x0D,0x00);
    sensor_write_register(0x0E,0x1B);
    sensor_write_register(0x0F,0x00);
    sensor_write_register(0x10,0x00);
    sensor_write_register(0x11,0x04);
    sensor_write_register(0x12,0x0A);
    sensor_write_register(0x13,0x40);
    sensor_write_register(0x14,0x01);
    sensor_write_register(0x15,0x81);
    sensor_write_register(0x16,0x05);
    sensor_write_register(0x17,0xFA);
    sensor_write_register(0x18,0xC3);
    sensor_write_register(0x19,0xC0);
    sensor_write_register(0x1A,0xF0);
    sensor_write_register(0x1B,0x00);
    sensor_write_register(0x1C,0x11);
    sensor_write_register(0x1D,0x47);
    sensor_write_register(0x1E,0x0E);
    sensor_write_register(0x1F,0x00);
    sensor_write_register(0x20,0x00);
    sensor_write_register(0x21,0xFF);
    sensor_write_register(0x22,0x00);
    sensor_write_register(0x23,0x00);
    sensor_write_register(0x24,0x00);
    sensor_write_register(0x25,0x00);
    sensor_write_register(0x26,0x03);
    sensor_write_register(0x27,0xFF);
    sensor_write_register(0x28,0x07);
    sensor_write_register(0x29,0xFF);
    sensor_write_register(0x2A,0x00);
    sensor_write_register(0x2B,0x00);
    sensor_write_register(0x2C,0x00);
    sensor_write_register(0x2D,0x00);
    sensor_write_register(0x2E,0x00);
    sensor_write_register(0x2F,0x80);
    sensor_write_register(0x30,0x00);
    sensor_write_register(0x31,0x00);
    sensor_write_register(0x32,0x00);
    sensor_write_register(0x33,0x0A);
    sensor_write_register(0x34,0x02);
    sensor_write_register(0x35,0xEA);
    sensor_write_register(0x36,0x01);
    sensor_write_register(0x37,0x12);
    sensor_write_register(0x38,0x06);
    sensor_write_register(0x39,0x22);
    sensor_write_register(0x3A,0x00);
    sensor_write_register(0x3B,0x02);
    sensor_write_register(0x3C,0x00);
    sensor_write_register(0x3D,0x08);
    sensor_write_register(0x3E,0x01);
    sensor_write_register(0x3F,0x12);
    sensor_write_register(0x40,0x06);
    sensor_write_register(0x41,0x22);
    sensor_write_register(0x42,0x00);
    sensor_write_register(0x43,0x00);
    sensor_write_register(0x44,0x00);
    sensor_write_register(0x45,0x00);
    sensor_write_register(0x46,0x00);
    sensor_write_register(0x47,0x00);
    sensor_write_register(0x48,0x01);
    sensor_write_register(0x49,0x00);
    sensor_write_register(0x4A,0x01);
    sensor_write_register(0x4B,0x00);
    sensor_write_register(0x4C,0x01);
    sensor_write_register(0x4D,0x00);
    sensor_write_register(0x4E,0x01);
    sensor_write_register(0x4F,0x02);
    sensor_write_register(0x50,0x7F);
    sensor_write_register(0x51,0x04);
    sensor_write_register(0x52,0xFF);
    sensor_write_register(0x53,0x00);
    sensor_write_register(0x54,0x0A);
    sensor_write_register(0x55,0x06);
    sensor_write_register(0x56,0x24);
    sensor_write_register(0x57,0x06);
    sensor_write_register(0x58,0x24);
    sensor_write_register(0x59,0x06);
    sensor_write_register(0x5A,0x53);
    sensor_write_register(0x5B,0x00);
    sensor_write_register(0x5C,0x02);
    sensor_write_register(0x5D,0x06);
    sensor_write_register(0x5E,0x1C);
    sensor_write_register(0x5F,0x00);
    sensor_write_register(0x60,0x02);
    sensor_write_register(0x61,0x00);
    sensor_write_register(0x62,0xA0);
    sensor_write_register(0x63,0x00);
    sensor_write_register(0x64,0x00);
    sensor_write_register(0x65,0x00);
    sensor_write_register(0x66,0x00);
    sensor_write_register(0x67,0x00);
    sensor_write_register(0x68,0xB3);
    sensor_write_register(0x69,0x00);
    sensor_write_register(0x6A,0xC6);
    sensor_write_register(0x6B,0x00);
    sensor_write_register(0x6C,0x00);
    sensor_write_register(0x6D,0x00);
    sensor_write_register(0x6E,0x00);
    sensor_write_register(0x6F,0x00);
    sensor_write_register(0x70,0xD0);
    sensor_write_register(0x71,0x06);
    sensor_write_register(0x72,0x1A);
    sensor_write_register(0x73,0x00);
    sensor_write_register(0x74,0xD6);
    sensor_write_register(0x75,0x06);
    sensor_write_register(0x76,0x18);
    sensor_write_register(0x77,0x06);
    sensor_write_register(0x78,0x46);
    sensor_write_register(0x79,0x06);
    sensor_write_register(0x7A,0x50);
    sensor_write_register(0x7B,0x00);
    sensor_write_register(0x7C,0xD6);
    sensor_write_register(0x7D,0x06);
    sensor_write_register(0x7E,0x18);
    sensor_write_register(0x7F,0x06);
    sensor_write_register(0x80,0x30);
    sensor_write_register(0x81,0x06);
    sensor_write_register(0x82,0x67);
    sensor_write_register(0x83,0x06);
    sensor_write_register(0x84,0x30);
    sensor_write_register(0x85,0x06);
    sensor_write_register(0x86,0x67);
    sensor_write_register(0x87,0x06);
    sensor_write_register(0x88,0x28);
    sensor_write_register(0x89,0x06);
    sensor_write_register(0x8A,0x2D);
    sensor_write_register(0x8B,0x00);
    sensor_write_register(0x8C,0xD8);
    sensor_write_register(0x8D,0x06);
    sensor_write_register(0x8E,0x30);
    sensor_write_register(0x8F,0x00);
    sensor_write_register(0x90,0xAD);
    sensor_write_register(0x91,0x06);
    sensor_write_register(0x92,0x30);
    sensor_write_register(0x93,0x06);
    sensor_write_register(0x94,0x71);
    sensor_write_register(0x95,0x06);
    sensor_write_register(0x96,0x30);
    sensor_write_register(0x97,0x06);
    sensor_write_register(0x98,0x41);
    sensor_write_register(0x99,0x00);
    sensor_write_register(0x9A,0xB3);
    sensor_write_register(0x9B,0x06);
    sensor_write_register(0x9C,0x24);
    sensor_write_register(0x9D,0x00);
    sensor_write_register(0x9E,0x00);
    sensor_write_register(0x9F,0x06);
    sensor_write_register(0xA0,0x56);
    sensor_write_register(0xA1,0x04);
    sensor_write_register(0xA2,0x6E);
    sensor_write_register(0xA3,0x00);
    sensor_write_register(0xA4,0x00);
    sensor_write_register(0xA5,0x00);
    sensor_write_register(0xA6,0x00);
    sensor_write_register(0xA7,0x00);
    sensor_write_register(0xA8,0x00);
    sensor_write_register(0xA9,0x00);
    sensor_write_register(0xAA,0xFF);
    sensor_write_register(0xAB,0x7F);
    sensor_write_register(0xAC,0x00);
    sensor_write_register(0xAD,0x00);
    sensor_write_register(0xAE,0x20);
    sensor_write_register(0xAF,0x20);
    sensor_write_register(0xB0,0x20);
    sensor_write_register(0xB1,0x30);
    sensor_write_register(0xB2,0x00);
    sensor_write_register(0xB3,0x40);
    sensor_write_register(0xB4,0x40);
    sensor_write_register(0xB5,0x40);
    sensor_write_register(0xB6,0x00);
    sensor_write_register(0xB7,0x00);
    sensor_write_register(0xB8,0x00);
    sensor_write_register(0xB9,0x06);
    sensor_write_register(0xBA,0x30);
    sensor_write_register(0xBB,0x06);
    sensor_write_register(0xBC,0x67);
    sensor_write_register(0xBD,0x00);
    sensor_write_register(0xBE,0x08);
    sensor_write_register(0xBF,0x00);
    sensor_write_register(0xC0,0x00);
    sensor_write_register(0xC1,0x27);
    sensor_write_register(0xC2,0x76);
    sensor_write_register(0xC3,0x00);
    sensor_write_register(0xC4,0x40);
    sensor_write_register(0xC5,0x00);
    sensor_write_register(0xC6,0x11);
    sensor_write_register(0xC7,0x70);
    sensor_write_register(0xC8,0x01);
    sensor_write_register(0xC9,0x01);
    sensor_write_register(0xCA,0x00);
    sensor_write_register(0xCB,0x00);
    sensor_write_register(0xCC,0x00);
    sensor_write_register(0xCD,0x00);
    sensor_write_register(0xCE,0x00);
    sensor_write_register(0xCF,0x00);
    sensor_write_register(0xD0,0x00);
    sensor_write_register(0xD1,0x00);
    sensor_write_register(0xD2,0x00);
    sensor_write_register(0xD3,0x00);
    sensor_write_register(0xD4,0x00);
    sensor_write_register(0xD5,0x00);
    sensor_write_register(0xD6,0x00);
    sensor_write_register(0xD7,0x00);
    sensor_write_register(0xD8,0x00);
    sensor_write_register(0x03,0x02);   // --------  C
    sensor_write_register(0x04,0x00);
    sensor_write_register(0x05,0x02);
    sensor_write_register(0x06,0x00);
    sensor_write_register(0x07,0xE0);
    sensor_write_register(0x08,0x00);
    sensor_write_register(0x09,0x00);
    sensor_write_register(0x0A,0x15);
    sensor_write_register(0x0B,0x80);
    sensor_write_register(0x0C,0x00);
    sensor_write_register(0x29,0x41);
    sensor_write_register(0x2A,0x00);
    sensor_write_register(0x2B,0x14);
    sensor_write_register(0x2C,0x00);
    sensor_write_register(0x2D,0x40);
    sensor_write_register(0x2E,0x30);
    sensor_write_register(0x2F,0x14);
    sensor_write_register(0x30,0x80);
    sensor_write_register(0x31,0x80);
    sensor_write_register(0x32,0x00);
    sensor_write_register(0x7A,0x8C);
    sensor_write_register(0x7B,0x20);
    sensor_write_register(0x7C,0x20);
    sensor_write_register(0x7D,0x00);
    sensor_write_register(0x7E,0x0A);
    sensor_write_register(0x7F,0x80);
    sensor_write_register(0x8E,0xFE);
    sensor_write_register(0x8F,0x00);
    sensor_write_register(0x90,0x00);
    sensor_write_register(0x9A,0x81);
    sensor_write_register(0x9B,0x00);
    sensor_write_register(0x9C,0x00);
    sensor_write_register(0x9D,0x00);
    sensor_write_register(0xAC,0x00);
    sensor_write_register(0xAD,0x00);
    sensor_write_register(0xAE,0x00);
    sensor_write_register(0xAF,0x0C);
    sensor_write_register(0xB0,0x04);
    sensor_write_register(0xB1,0x4B);
    sensor_write_register(0xB2,0x00);
    sensor_write_register(0xB3,0x00);
    sensor_write_register(0xB4,0x05);
    sensor_write_register(0xB5,0x05);
    sensor_write_register(0xB6,0x04);
    sensor_write_register(0xB7,0x00);
    sensor_write_register(0xB8,0x05);
    sensor_write_register(0xB9,0x02);
    sensor_write_register(0xBA,0xD4);
    sensor_write_register(0xBB,0x01);
    sensor_write_register(0xBC,0xAF);
    sensor_write_register(0xBD,0x03);
    sensor_write_register(0xBE,0x5A);
    sensor_write_register(0xBF,0x00);
    sensor_write_register(0xC0,0xF5);
    sensor_write_register(0xC1,0x01);
    sensor_write_register(0xC2,0xE4);
    sensor_write_register(0xC3,0x02);
    sensor_write_register(0xC4,0x85);
    sensor_write_register(0xC5,0x01);
    sensor_write_register(0xC6,0x6D);
    sensor_write_register(0xC7,0x00);
    sensor_write_register(0xC8,0x05);
    sensor_write_register(0xC9,0x05);
    sensor_write_register(0xCA,0x04);
    sensor_write_register(0xCB,0x00);
    sensor_write_register(0xCC,0x05);
    sensor_write_register(0xCD,0x02);
    sensor_write_register(0xCE,0xD4);
    sensor_write_register(0x03,0x03);   // --------  D
    sensor_write_register(0x16,0x3A);
    sensor_write_register(0x17,0x50);
    sensor_write_register(0x18,0x5A);
    sensor_write_register(0x19,0x20);
    sensor_write_register(0x1A,0x00);
    sensor_write_register(0x22,0x00);
    sensor_write_register(0x23,0x00);
    sensor_write_register(0x24,0x00);
    sensor_write_register(0x25,0x00);
    sensor_write_register(0x26,0x00);
    sensor_write_register(0x27,0x10);
    sensor_write_register(0x28,0x20);
    sensor_write_register(0x29,0x00);
    sensor_write_register(0x2A,0x00);
    sensor_write_register(0x2B,0x0C);
    sensor_write_register(0x2C,0x1C);
    sensor_write_register(0x2D,0x00);
    sensor_write_register(0x2E,0x5F);
    sensor_write_register(0x2F,0x3F);
    sensor_write_register(0x30,0x3F);
    sensor_write_register(0x31,0x5F);
    sensor_write_register(0x32,0x40);
    sensor_write_register(0x33,0x40);
    sensor_write_register(0x34,0x40);
    sensor_write_register(0x35,0x40);
    sensor_write_register(0x36,0x00);
    sensor_write_register(0x37,0x08);
    sensor_write_register(0x38,0x18);
    sensor_write_register(0x39,0x00);
    sensor_write_register(0x3A,0x10);
    sensor_write_register(0x3B,0x10);
    sensor_write_register(0x3C,0x10);
    sensor_write_register(0x3D,0x10);
    sensor_write_register(0x3E,0x04);
    sensor_write_register(0x3F,0x08);
    sensor_write_register(0x40,0x18);
    sensor_write_register(0x41,0x04);
    sensor_write_register(0x42,0x00);
    sensor_write_register(0x03,0x04);   // --------  E
    sensor_write_register(0x04,0x9F);
    sensor_write_register(0x05,0x64);
    sensor_write_register(0x06,0x80);
    sensor_write_register(0x07,0x80);
    sensor_write_register(0x08,0x00);
    sensor_write_register(0x09,0x01);
    sensor_write_register(0x0A,0x00);
    sensor_write_register(0x0B,0x80);
    sensor_write_register(0x0E,0x00);
    sensor_write_register(0x0F,0x20);
    sensor_write_register(0x10,0x40);
    sensor_write_register(0x11,0xEE);
    sensor_write_register(0x12,0x02);
    sensor_write_register(0x13,0xE8);
    sensor_write_register(0x14,0x02);
    sensor_write_register(0x15,0xE8);
    sensor_write_register(0x16,0x02);
    sensor_write_register(0x17,0xE8);
    sensor_write_register(0x18,0x00);
    sensor_write_register(0x19,0x00);
    sensor_write_register(0x1A,0x20);
    sensor_write_register(0x1B,0x00);
    sensor_write_register(0x1C,0xBA);
    sensor_write_register(0x1D,0x00);
    sensor_write_register(0x1E,0x00);
    sensor_write_register(0x1F,0xBA);
    sensor_write_register(0x20,0x00);
    sensor_write_register(0x21,0x00);
    sensor_write_register(0x22,0x00);
    sensor_write_register(0x23,0x80);
    sensor_write_register(0x24,0x00);
    sensor_write_register(0x25,0x01);
    sensor_write_register(0x26,0x00);
    sensor_write_register(0x27,0x00);
    sensor_write_register(0x28,0x00);
    sensor_write_register(0x29,0x27);
    sensor_write_register(0x2A,0x76);
    sensor_write_register(0x2B,0x00);
    sensor_write_register(0x2C,0xBB);
    sensor_write_register(0x2D,0x00);
    sensor_write_register(0x2E,0x02);
    sensor_write_register(0x2F,0x00);
    sensor_write_register(0x03,0x00);	// pll
    sensor_write_register(0x40,0x2C);

    
    sensor_write_register(0x03,0x01);

	printf("PixelPlus po3100k sensor 720P30fps init success!\n");
	
}


