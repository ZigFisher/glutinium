#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef HI_GPIO_I2C
#include "gpioi2c_16.h"
#include "gpio_i2c.h"
#else
#include "hi_i2c.h"
#endif

const unsigned int  sensor_i2c_addr	=	0x90;		/* I2C Address of MT9P006 */
const unsigned int  sensor_addr_byte	=	1;
const unsigned int  sensor_data_byte	=	2;

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
    sensor_write_register(0x0D, 0x0001);      //RESET_REG
    sensor_write_register(0x0D, 0x0000);      //RESET_REG
    usleep(100000);

    sensor_write_register(0x1e, 0x0006);
    sensor_write_register(0x4f, 0x0011);
    sensor_write_register(0x57, 0x0002);
  
    sensor_write_register(0x10, 0x0051);      // PLL Control         pll power on, disable
    sensor_write_register(0x11, 0x1801);      // PLL_Config1      
    sensor_write_register(0x12, 0x0002);      // PLL_Config2         
    usleep(1000);                              // Allow PLL to lock   
    sensor_write_register(0x10, 0x0053);
    usleep(200000);

    sensor_write_register(0x07, 0x1f8e);
    sensor_write_register(0x01, 0x01af);     //Sensor row start
    sensor_write_register(0x02, 0x014c);     //Sensor column start
    sensor_write_register(0x03, 0x043b);     //Row width = 1084 - 1
    sensor_write_register(0x04, 0x0781);     //Column width = 1922 - 1
    sensor_write_register(0x05, 0x01c4);     //HORZ_BLANK_REG 
    sensor_write_register(0x06, 0x002f);     //VERT_BLANK_REG  Max 0x07FF

    sensor_write_register(0x22, 0x0000);     //Row mode
    sensor_write_register(0x23, 0x0000);     //Column mode

    sensor_write_register(0x70, 0x0079);
    sensor_write_register(0x71, 0x7800);
    sensor_write_register(0x72, 0x7800);
    sensor_write_register(0x73, 0x0300);
    sensor_write_register(0x74, 0x0300);
    sensor_write_register(0x75, 0x3c00);
    sensor_write_register(0x76, 0x4e3d);
    sensor_write_register(0x77, 0x4e3d);
    sensor_write_register(0x78, 0x774f);
    sensor_write_register(0x79, 0x7900);
    sensor_write_register(0x7a, 0x7900);
    sensor_write_register(0x7b, 0x7800);
    sensor_write_register(0x7c, 0x7800);
    sensor_write_register(0x7e, 0x7800);
    sensor_write_register(0x7f, 0x7800);

    sensor_write_register(0x29, 0x0481);
    sensor_write_register(0x3f, 0x0007);
    sensor_write_register(0x41, 0x0003);
    sensor_write_register(0x48, 0x0018);
    sensor_write_register(0x5f, 0x1c16);

    sensor_write_register(0x08, 0x0000);
    sensor_write_register(0x09, 0x009d);
    sensor_write_register(0x0c, 0x0000);

    sensor_write_register(0x3e, 0x0007);     // When gain <=4x, set to 0x0007  (blooming fix); when  gain > 4x , set to 0x0087 (hot pixels optimization)
 
    printf("Aptina MT9P006 sensor 1080P30fps init success!\n");
}



