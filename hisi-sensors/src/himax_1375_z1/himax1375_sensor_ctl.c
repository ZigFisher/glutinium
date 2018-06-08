
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef HI_GPIO_I2C
#include "gpioi2c_ov.h"
#include "gpio_i2c.h"
#else
#include "hi_i2c.h"
#endif

const unsigned int sensor_i2c_addr  = 0x48;        /* I2C Address of himax1375 */
const unsigned int sensor_addr_byte = 2;
const unsigned int sensor_data_byte = 1;

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
        close(fd);
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
        close(fd);
        return -1;
    }

    close(fd);
#endif

    return i2c_data.data;
}

int sensor_write_register(int addr, int data)
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

    value = ((sensor_i2c_addr&0xff)<<24) | ((addr&0xff)<<16) | (data&0xff);

    ret = ioctl(fd, GPIO_I2C_WRITE, &value);

    if (ret)
    {
        printf("GPIO-I2C write faild!\n");
        close(fd);
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
        close(fd);
        return -1;
    }

    close(fd);
#endif
    return 0;
}

int sensor_write_register_bit(int addr, int data, int mask)
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
        close(fd);
        return -1;
    }

    value &= 0xff;
    value &= ~mask;
    value |= data & mask;

    value = ((sensor_i2c_addr&0xff)<<24) | ((addr&0xff)<<16) | (value&0xff);

    ret = ioctl(fd, GPIO_I2C_WRITE, &value);
    if (ret)
    {
        printf("GPIO-I2C write faild!\n");
        close(fd);
        return -1;
    }

    close(fd);
#else
    int fd = -1;
    int ret;
    int value;
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
        close(fd);
        return -1;
    }

    value = i2c_data.data;
    value &= ~mask;
    value |= data & mask;

    i2c_data.data = value;

    ret = ioctl(fd, CMD_I2C_WRITE, &i2c_data);
    if (ret)
    {
        printf("hi_i2c write faild!\n");
        close(fd);
        return -1;
    }

    close(fd);
#endif
    return 0;
}


static void delay_ms(int ms)
{
    usleep(ms*1000);
}

void sensor_prog(int* rom)
{
    int i = 0;
    while (1)
    {
        int u32Lookup = rom[i++];
        int addr = (u32Lookup >> 16) & 0xFFFF;
        int data = u32Lookup & 0xFFFF;
        if (addr == 0xFFFE)
        {
            delay_ms(data);
        }
        else if (addr == 0xFFFF)
        {
            return;
        }
        else
        {
            sensor_write_register(addr, data);
        }
    }
}

void sensor_init()
{    
    //HM1375 Raw 1280x720
    sensor_write_register(0x0022,0x00); //Software reset 
    sensor_write_register(0x000C,0x04);  
    sensor_write_register(0x0006,0x08);
    sensor_write_register(0x000A,0x00);
    sensor_write_register(0x000F,0x10);
    sensor_write_register(0x0010,0x00);
    sensor_write_register(0x0011,0x02);
    sensor_write_register(0x0012,0x01);
    sensor_write_register(0x0013,0x02);
    sensor_write_register(0x0015,0x01); //AE (MSB)
    sensor_write_register(0x0016,0x00); //AE (LSB)
    sensor_write_register(0x0018,0x00); //AGain = 2^X
    sensor_write_register(0x001D,0x40); //DGain = X/64
    sensor_write_register(0x0020,0x10);
    sensor_write_register(0x0023,0x43);
    sensor_write_register(0x0024,0x20);
    sensor_write_register(0x0025,0x00);
    sensor_write_register(0x0026,0x6C);
    sensor_write_register(0x0027,0x22); //30:yuv ,22:raw
    sensor_write_register(0x0028,0x01);
    sensor_write_register(0x0030,0x00);
    sensor_write_register(0x0034,0x0E);
    sensor_write_register(0x0035,0x01);
    sensor_write_register(0x0036,0x00);
    sensor_write_register(0x0038,0x02);
    sensor_write_register(0x0039,0x01);
    sensor_write_register(0x003A,0x01);
    sensor_write_register(0x003B,0xFF);
    sensor_write_register(0x003C,0xFF);
    sensor_write_register(0x003D,0x40);
    sensor_write_register(0x003F,0x14);
    sensor_write_register(0x0040,0x10);
    sensor_write_register(0x0044,0x07);
    sensor_write_register(0x0045,0x35);
    sensor_write_register(0x0048,0x7F);
    sensor_write_register(0x004E,0xFF);
    sensor_write_register(0x0070,0x22);
    sensor_write_register(0x0071,0x3F);
    sensor_write_register(0x0072,0x22);
    sensor_write_register(0x0073,0x30);
    sensor_write_register(0x0074,0x13);
    sensor_write_register(0x0075,0x40);
    sensor_write_register(0x0076,0x24);
    sensor_write_register(0x0078,0x0F);
    sensor_write_register(0x007A,0x06);
    sensor_write_register(0x007B,0x14);
    sensor_write_register(0x007C,0x10);
    sensor_write_register(0x0080,0xC9);
    sensor_write_register(0x0081,0x00);
    sensor_write_register(0x0082,0x28);
    sensor_write_register(0x0083,0xB0);
    sensor_write_register(0x0084,0x60);
    sensor_write_register(0x0086,0x3E);
    sensor_write_register(0x0087,0x70);
    sensor_write_register(0x0088,0x11);
    sensor_write_register(0x0089,0x3C);
    sensor_write_register(0x008A,0x87);
    sensor_write_register(0x008D,0x64);
    sensor_write_register(0x0090,0x07);
    sensor_write_register(0x0091,0x09);
    sensor_write_register(0x0092,0x0C);
    sensor_write_register(0x0093,0x0C);
    sensor_write_register(0x0094,0x0C);
    sensor_write_register(0x0095,0x0C);
    sensor_write_register(0x0096,0x01);
    sensor_write_register(0x0097,0x00);
    sensor_write_register(0x0098,0x04);
    sensor_write_register(0x0099,0x08);
    sensor_write_register(0x009A,0x0C);
    sensor_write_register(0x0120,0x37);
    sensor_write_register(0x0121,0x81);
    sensor_write_register(0x0122,0xEB);
    sensor_write_register(0x0123,0x29);
    sensor_write_register(0x0124,0x50);
    sensor_write_register(0x0125,0xDE);
    sensor_write_register(0x0126,0xB1);
    sensor_write_register(0x013D,0x0F);
    sensor_write_register(0x013E,0x0F);
    sensor_write_register(0x013F,0x0F);
    sensor_write_register(0x0140,0x14);
    sensor_write_register(0x0141,0x0A);
    sensor_write_register(0x0142,0x14);
    sensor_write_register(0x0143,0x0A);
    sensor_write_register(0x0144,0x08);
    sensor_write_register(0x0145,0x04);
    sensor_write_register(0x0146,0x28);
    sensor_write_register(0x0147,0x3C);
    sensor_write_register(0x0148,0x28);
    sensor_write_register(0x0149,0x3C);
    sensor_write_register(0x014A,0x96);
    sensor_write_register(0x014B,0xC8);
    sensor_write_register(0x0150,0x14);
    sensor_write_register(0x0151,0x30);
    sensor_write_register(0x0152,0x54);
    sensor_write_register(0x0153,0x70);
    sensor_write_register(0x0154,0x14);
    sensor_write_register(0x0155,0x30);
    sensor_write_register(0x0156,0x54);
    sensor_write_register(0x0157,0x70);
    sensor_write_register(0x0158,0x14);
    sensor_write_register(0x0159,0x30);
    sensor_write_register(0x015A,0x54);
    sensor_write_register(0x015B,0x70);
    sensor_write_register(0x015C,0x30);
    sensor_write_register(0x015D,0x00);
    sensor_write_register(0x01D8,0x20);
    sensor_write_register(0x01D9,0x08);
    sensor_write_register(0x01DA,0x20);
    sensor_write_register(0x01DB,0x08);
    sensor_write_register(0x01DC,0x20);
    sensor_write_register(0x01DD,0x08);
    sensor_write_register(0x01DE,0x50);
    sensor_write_register(0x01E0,0x50);
    sensor_write_register(0x01E2,0x50);
    sensor_write_register(0x01E4,0x10);
    sensor_write_register(0x01E5,0x10);
    sensor_write_register(0x01E6,0x02);
    sensor_write_register(0x01E7,0x10);
    sensor_write_register(0x01E8,0x10);
    sensor_write_register(0x01E9,0x10);
    sensor_write_register(0x01EC,0x28);
    sensor_write_register(0x0220,0x00);
    sensor_write_register(0x0221,0xA0);
    sensor_write_register(0x0222,0x00);
    sensor_write_register(0x0223,0x80);
    sensor_write_register(0x0224,0x80);
    sensor_write_register(0x0225,0x00);
    sensor_write_register(0x0226,0x80);
    sensor_write_register(0x0227,0x80);
    sensor_write_register(0x0228,0x00);
    sensor_write_register(0x0229,0x80);
    sensor_write_register(0x022A,0x80);
    sensor_write_register(0x022B,0x00);
    sensor_write_register(0x022C,0x80);
    sensor_write_register(0x022D,0x12);
    sensor_write_register(0x022E,0x10);
    sensor_write_register(0x022F,0x12);
    sensor_write_register(0x0230,0x10);
    sensor_write_register(0x0231,0x12);
    sensor_write_register(0x0232,0x10);
    sensor_write_register(0x0233,0x12);
    sensor_write_register(0x0234,0x10);
    sensor_write_register(0x0235,0x88);
    sensor_write_register(0x0236,0x02);
    sensor_write_register(0x0237,0x88);
    sensor_write_register(0x0238,0x02);
    sensor_write_register(0x0239,0x88);
    sensor_write_register(0x023A,0x02);
    sensor_write_register(0x023B,0x88);
    sensor_write_register(0x023C,0x02);
    sensor_write_register(0x023D,0x04);
    sensor_write_register(0x023E,0x02);
    sensor_write_register(0x023F,0x04);
    sensor_write_register(0x0240,0x02);
    sensor_write_register(0x0241,0x04);
    sensor_write_register(0x0242,0x02);
    sensor_write_register(0x0243,0x04);
    sensor_write_register(0x0244,0x02);
    sensor_write_register(0x0251,0x10);
    sensor_write_register(0x0280,0x00);
    sensor_write_register(0x0281,0x41);
    sensor_write_register(0x0282,0x00);
    sensor_write_register(0x0283,0x6D);
    sensor_write_register(0x0284,0x00);
    sensor_write_register(0x0285,0xBC);
    sensor_write_register(0x0286,0x01);
    sensor_write_register(0x0287,0x45);
    sensor_write_register(0x0288,0x01);
    sensor_write_register(0x0289,0x7B);
    sensor_write_register(0x028A,0x01);
    sensor_write_register(0x028B,0xAC);
    sensor_write_register(0x028C,0x01);
    sensor_write_register(0x028D,0xD2);
    sensor_write_register(0x028E,0x01);
    sensor_write_register(0x028F,0xF6);
    sensor_write_register(0x0290,0x02);
    sensor_write_register(0x0291,0x16);
    sensor_write_register(0x0292,0x02);
    sensor_write_register(0x0293,0x35);
    sensor_write_register(0x0294,0x02);
    sensor_write_register(0x0295,0x6E);
    sensor_write_register(0x0296,0x02);
    sensor_write_register(0x0297,0xA2);
    sensor_write_register(0x0298,0x02);
    sensor_write_register(0x0299,0xFF);
    sensor_write_register(0x029A,0x03);
    sensor_write_register(0x029B,0x51);
    sensor_write_register(0x029C,0x03);
    sensor_write_register(0x029D,0x9B);
    sensor_write_register(0x029E,0x00);
    sensor_write_register(0x029F,0x85);
    sensor_write_register(0x02A0,0x04);
    sensor_write_register(0x02C0,0x80);
    sensor_write_register(0x02C1,0x01);
    sensor_write_register(0x02C2,0x71);
    sensor_write_register(0x02C3,0x04);
    sensor_write_register(0x02C4,0x0F);
    sensor_write_register(0x02C5,0x04);
    sensor_write_register(0x02C6,0x3D);
    sensor_write_register(0x02C7,0x04);
    sensor_write_register(0x02C8,0x94);
    sensor_write_register(0x02C9,0x01);
    sensor_write_register(0x02CA,0x57);
    sensor_write_register(0x02CB,0x04);
    sensor_write_register(0x02CC,0x0F);
    sensor_write_register(0x02CD,0x04);
    sensor_write_register(0x02CE,0x8F);
    sensor_write_register(0x02CF,0x04);
    sensor_write_register(0x02D0,0x9E);
    sensor_write_register(0x02D1,0x01);
    sensor_write_register(0x02E0,0x06);
    sensor_write_register(0x02E1,0xC0);
    sensor_write_register(0x02E2,0xE0);
    sensor_write_register(0x02F0,0x48);
    sensor_write_register(0x02F1,0x01);
    sensor_write_register(0x02F2,0x32);
    sensor_write_register(0x02F3,0x04);
    sensor_write_register(0x02F4,0x16);
    sensor_write_register(0x02F5,0x04);
    sensor_write_register(0x02F6,0x52);
    sensor_write_register(0x02F7,0x04);
    sensor_write_register(0x02F8,0xAA);
    sensor_write_register(0x02F9,0x01);
    sensor_write_register(0x02FA,0x58);
    sensor_write_register(0x02FB,0x04);
    sensor_write_register(0x02FC,0x56);
    sensor_write_register(0x02FD,0x04);
    sensor_write_register(0x02FE,0xDD);
    sensor_write_register(0x02FF,0x04);
    sensor_write_register(0x0300,0x33);
    sensor_write_register(0x0301,0x02);
    sensor_write_register(0x0324,0x00);
    sensor_write_register(0x0325,0x01);
    sensor_write_register(0x0333,0x86);
    sensor_write_register(0x0334,0x00);
    sensor_write_register(0x0335,0x86);
    sensor_write_register(0x0340,0x40);
    sensor_write_register(0x0341,0x44);
    sensor_write_register(0x0342,0x4A);
    sensor_write_register(0x0343,0x2B);
    sensor_write_register(0x0344,0x94);
    sensor_write_register(0x0345,0x3F);
    sensor_write_register(0x0346,0x8E);
    sensor_write_register(0x0347,0x51);
    sensor_write_register(0x0348,0x75);
    sensor_write_register(0x0349,0x5C);
    sensor_write_register(0x034A,0x6A);
    sensor_write_register(0x034B,0x68);
    sensor_write_register(0x034C,0x5E);
    sensor_write_register(0x0350,0x7C);
    sensor_write_register(0x0351,0x78);
    sensor_write_register(0x0352,0x08);
    sensor_write_register(0x0353,0x04);
    sensor_write_register(0x0354,0x80);
    sensor_write_register(0x0355,0x9A);
    sensor_write_register(0x0356,0xCC);
    sensor_write_register(0x0357,0xFF);
    sensor_write_register(0x0358,0xFF);
    sensor_write_register(0x035A,0xFF);
    sensor_write_register(0x035B,0x00);
    sensor_write_register(0x035C,0x70);
    sensor_write_register(0x035D,0x80);
    sensor_write_register(0x035F,0xA0);
    sensor_write_register(0x0488,0x30);
    sensor_write_register(0x0360,0xDF);
    sensor_write_register(0x0361,0x00);
    sensor_write_register(0x0362,0xFF);
    sensor_write_register(0x0363,0x03);
    sensor_write_register(0x0364,0xFF);
    sensor_write_register(0x037B,0x11);
    sensor_write_register(0x037C,0x1E);
    sensor_write_register(0x0380,0xFC); //[0]:AE , [1]:AWB 
    sensor_write_register(0x0383,0x50);
    sensor_write_register(0x038A,0x64);
    sensor_write_register(0x038B,0x64);
    sensor_write_register(0x038E,0x3C);
    sensor_write_register(0x0391,0x2A);
    sensor_write_register(0x0393,0x1E);
    sensor_write_register(0x0394,0x64);
    sensor_write_register(0x0395,0x23);
    sensor_write_register(0x0398,0x03);
    sensor_write_register(0x0399,0x45);
    sensor_write_register(0x039A,0x06);
    sensor_write_register(0x039B,0x8B);
    sensor_write_register(0x039C,0x0D);
    sensor_write_register(0x039D,0x16);
    sensor_write_register(0x039E,0x0A);
    sensor_write_register(0x039F,0x10);
    sensor_write_register(0x03A0,0x10);
    sensor_write_register(0x03A1,0xE5);
    sensor_write_register(0x03A2,0x06);
    sensor_write_register(0x03A4,0x18);
    sensor_write_register(0x03A5,0x48);
    sensor_write_register(0x03A6,0x2D);
    sensor_write_register(0x03A7,0x78);
    sensor_write_register(0x03AC,0x5A);
    sensor_write_register(0x03AD,0x0F);
    sensor_write_register(0x03AE,0x7F);
    sensor_write_register(0x03AF,0x04);
    sensor_write_register(0x03B0,0x35);
    sensor_write_register(0x03B1,0x14);
    sensor_write_register(0x036F,0x04);
    sensor_write_register(0x0370,0x0A);
    sensor_write_register(0x0371,0x04);
    sensor_write_register(0x0372,0x00);
    sensor_write_register(0x0373,0x40);
    sensor_write_register(0x0374,0x20);
    sensor_write_register(0x0375,0x04);
    sensor_write_register(0x0376,0x00);
    sensor_write_register(0x0377,0x08);
    sensor_write_register(0x0378,0x08);
    sensor_write_register(0x0379,0x04);
    sensor_write_register(0x037A,0x08);

    //black level of RGGB
    sensor_write_register(0x0420,0x00);
    sensor_write_register(0x0421,0x00);
    sensor_write_register(0x0422,0x00);
    sensor_write_register(0x0423,0x00);
    
    sensor_write_register(0x0430,0x10);
    sensor_write_register(0x0431,0x60);
    sensor_write_register(0x0432,0x10);
    sensor_write_register(0x0433,0x20);
    sensor_write_register(0x0434,0x00);
    sensor_write_register(0x0435,0x30);
    sensor_write_register(0x0436,0x00);
    sensor_write_register(0x0450,0xFD);
    sensor_write_register(0x0451,0xD8);
    sensor_write_register(0x0452,0xA0);
    sensor_write_register(0x0453,0x50);
    sensor_write_register(0x0454,0x00);
    sensor_write_register(0x0459,0x04);
    sensor_write_register(0x045A,0x00);
    sensor_write_register(0x045B,0x30);
    sensor_write_register(0x045C,0x01);
    sensor_write_register(0x045D,0x70);
    sensor_write_register(0x0460,0x00);
    sensor_write_register(0x0461,0x00);
    sensor_write_register(0x0462,0x00);
    sensor_write_register(0x0465,0x16);
    sensor_write_register(0x0466,0x14);
    sensor_write_register(0x0478,0x00);
    sensor_write_register(0x0480,0x60);
    sensor_write_register(0x0481,0x06);
    sensor_write_register(0x0482,0x0C);
    sensor_write_register(0x04B0,0x4C);
    sensor_write_register(0x04B1,0x86);
    sensor_write_register(0x04B2,0x00);
    sensor_write_register(0x04B3,0x18);
    sensor_write_register(0x04B4,0x00);
    sensor_write_register(0x04B5,0x00);
    sensor_write_register(0x04B6,0x30);
    sensor_write_register(0x04B7,0x00);
    sensor_write_register(0x04B8,0x00);
    sensor_write_register(0x04B9,0x10);
    sensor_write_register(0x04BA,0x00);
    sensor_write_register(0x04BB,0x00);
    sensor_write_register(0x04BD,0x00);
    sensor_write_register(0x04D0,0x56);
    sensor_write_register(0x04D6,0x30);
    sensor_write_register(0x04DD,0x10);
    sensor_write_register(0x04D9,0x16);
    sensor_write_register(0x04D3,0x18);
    sensor_write_register(0x0540,0x00);
    sensor_write_register(0x0541,0xD0);
    sensor_write_register(0x0542,0x00);
    sensor_write_register(0x0543,0xFA);
    sensor_write_register(0x0580,0x50);
    sensor_write_register(0x0581,0x30);
    sensor_write_register(0x0582,0x2D);
    sensor_write_register(0x0583,0x16);
    sensor_write_register(0x0584,0x1E);
    sensor_write_register(0x0585,0x0F);
    sensor_write_register(0x0586,0x08);
    sensor_write_register(0x0587,0x10);
    sensor_write_register(0x0590,0x10);
    sensor_write_register(0x0591,0x10);
    sensor_write_register(0x0592,0x05);
    sensor_write_register(0x0593,0x05);
    sensor_write_register(0x0594,0x04);
    sensor_write_register(0x0595,0x06);
    sensor_write_register(0x05B0,0x04);
    sensor_write_register(0x05B1,0x00);
    sensor_write_register(0x05E4,0x08);
    sensor_write_register(0x05E5,0x00);
    sensor_write_register(0x05E6,0x07);
    sensor_write_register(0x05E7,0x05);
    sensor_write_register(0x05E8,0x0A);
    sensor_write_register(0x05E9,0x00);
    sensor_write_register(0x05EA,0xD9);
    sensor_write_register(0x05EB,0x02);
    sensor_write_register(0x0666,0x02);
    sensor_write_register(0x0667,0xE0);
    sensor_write_register(0x067F,0x19);
    sensor_write_register(0x067C,0x00);
    sensor_write_register(0x067D,0x00);
    sensor_write_register(0x0682,0x00);
    sensor_write_register(0x0683,0x00);
    sensor_write_register(0x0688,0x00);
    sensor_write_register(0x0689,0x00);
    sensor_write_register(0x068E,0x00);
    sensor_write_register(0x068F,0x00);
    sensor_write_register(0x0695,0x00);
    sensor_write_register(0x0694,0x00);
    sensor_write_register(0x0697,0x19);
    sensor_write_register(0x069B,0x00);
    sensor_write_register(0x069C,0x30);
    sensor_write_register(0x0720,0x00);
    sensor_write_register(0x0725,0x6A);
    sensor_write_register(0x0726,0x03);
    sensor_write_register(0x072B,0x64);
    sensor_write_register(0x072C,0x64);
    sensor_write_register(0x072D,0x20);
    sensor_write_register(0x072E,0x82);
    sensor_write_register(0x072F,0x08);
    sensor_write_register(0x0800,0x16);
    sensor_write_register(0x0801,0x30);
    sensor_write_register(0x0802,0x00);
    sensor_write_register(0x0803,0x68);
    sensor_write_register(0x0804,0x01);
    sensor_write_register(0x0805,0x28);
    sensor_write_register(0x0806,0x10);
    sensor_write_register(0x0808,0x1D);
    sensor_write_register(0x0809,0x18);
    sensor_write_register(0x080A,0x10);
    sensor_write_register(0x080B,0x07);
    sensor_write_register(0x080D,0x0F);
    sensor_write_register(0x080E,0x0F);
    sensor_write_register(0x0810,0x00);
    sensor_write_register(0x0811,0x08);
    sensor_write_register(0x0812,0x20);
    sensor_write_register(0x0857,0x0A);
    sensor_write_register(0x0858,0x30);
    sensor_write_register(0x0859,0x01);
    sensor_write_register(0x085A,0x03);
    sensor_write_register(0x085B,0x40);
    sensor_write_register(0x085C,0x03);
    sensor_write_register(0x085D,0x7F);
    sensor_write_register(0x085E,0x02);
    sensor_write_register(0x085F,0xD0);
    sensor_write_register(0x0860,0x03);
    sensor_write_register(0x0861,0x7F);
    sensor_write_register(0x0862,0x02);
    sensor_write_register(0x0863,0xD0);
    sensor_write_register(0x0864,0x00);
    sensor_write_register(0x0865,0x7F);
    sensor_write_register(0x0866,0x01);
    sensor_write_register(0x0867,0x00);
    sensor_write_register(0x0868,0x40);
    sensor_write_register(0x0869,0x01);
    sensor_write_register(0x086A,0x00);
    sensor_write_register(0x086B,0x40);
    sensor_write_register(0x086C,0x01);
    sensor_write_register(0x086D,0x00);
    sensor_write_register(0x086E,0x40);
    sensor_write_register(0x0870,0x00);
    sensor_write_register(0x0871,0x14);
    sensor_write_register(0x0872,0x01);
    sensor_write_register(0x0873,0x20);
    sensor_write_register(0x0874,0x00);
    sensor_write_register(0x0875,0x14);
    sensor_write_register(0x0876,0x00);
    sensor_write_register(0x0877,0xEC);
    sensor_write_register(0x0815,0x00);
    sensor_write_register(0x0816,0x4C);
    sensor_write_register(0x0817,0x00);
    sensor_write_register(0x0818,0x7B);
    sensor_write_register(0x0819,0x00);
    sensor_write_register(0x081A,0xCA);
    sensor_write_register(0x081B,0x01);
    sensor_write_register(0x081C,0x3E);
    sensor_write_register(0x081D,0x01);
    sensor_write_register(0x081E,0x77);
    sensor_write_register(0x081F,0x01);
    sensor_write_register(0x0820,0xAA);
    sensor_write_register(0x0821,0x01);
    sensor_write_register(0x0822,0xCE);
    sensor_write_register(0x0823,0x01);
    sensor_write_register(0x0824,0xEE);
    sensor_write_register(0x0825,0x02);
    sensor_write_register(0x0826,0x16);
    sensor_write_register(0x0827,0x02);
    sensor_write_register(0x0828,0x33);
    sensor_write_register(0x0829,0x02);
    sensor_write_register(0x082A,0x65);
    sensor_write_register(0x082B,0x02);
    sensor_write_register(0x082C,0x91);
    sensor_write_register(0x082D,0x02);
    sensor_write_register(0x082E,0xDC);
    sensor_write_register(0x082F,0x03);
    sensor_write_register(0x0830,0x28);
    sensor_write_register(0x0831,0x03);
    sensor_write_register(0x0832,0x74);
    sensor_write_register(0x0833,0x03);
    sensor_write_register(0x0834,0xFF);
    sensor_write_register(0x0882,0x00);
    sensor_write_register(0x0883,0x3E);
    sensor_write_register(0x0884,0x00);
    sensor_write_register(0x0885,0x70);
    sensor_write_register(0x0886,0x00);
    sensor_write_register(0x0887,0xB8);
    sensor_write_register(0x0888,0x01);
    sensor_write_register(0x0889,0x28);
    sensor_write_register(0x088A,0x01);
    sensor_write_register(0x088B,0x5B);
    sensor_write_register(0x088C,0x01);
    sensor_write_register(0x088D,0x8A);
    sensor_write_register(0x088E,0x01);
    sensor_write_register(0x088F,0xB1);
    sensor_write_register(0x0890,0x01);
    sensor_write_register(0x0891,0xD9);
    sensor_write_register(0x0892,0x01);
    sensor_write_register(0x0893,0xEE);
    sensor_write_register(0x0894,0x02);
    sensor_write_register(0x0895,0x0F);
    sensor_write_register(0x0896,0x02);
    sensor_write_register(0x0897,0x4C);
    sensor_write_register(0x0898,0x02);
    sensor_write_register(0x0899,0x74);
    sensor_write_register(0x089A,0x02);
    sensor_write_register(0x089B,0xC3);
    sensor_write_register(0x089C,0x03);
    sensor_write_register(0x089D,0x0F);
    sensor_write_register(0x089E,0x03);
    sensor_write_register(0x089F,0x57);
    sensor_write_register(0x08A0,0x03);
    sensor_write_register(0x08A1,0xFF);
    //close shading
    sensor_write_register(0x0122,0xAB);
    sensor_write_register(0x0100,0x01);
    sensor_write_register(0x0101,0x01);
    sensor_write_register(0x0000,0x01);
    sensor_write_register(0x002C,0x00);
    sensor_write_register(0x0005,0x01);
    
    return ;
}

