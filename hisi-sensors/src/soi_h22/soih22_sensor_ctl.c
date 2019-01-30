/******************************************************************************
  A driver program of soi h22 on HI3518A 
 ******************************************************************************
    Modification:  2013-03  Created
******************************************************************************/

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

const unsigned int sensor_i2c_addr    =    0x60;        /* I2C Address of SOI H22 */
//const unsigned int sensor_i2c_addr    =    0x6C;        /* I2C Address of SOI H22 */
const unsigned int sensor_addr_byte    =    1;
const unsigned int sensor_data_byte    =    1;


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

    ret = ioctl(fd, CMD_I2C_WRITE, &i2c_data);

    if (ret)
    {
        printf("hi_i2c write faild!\n");
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
//    printf("sensor_write_register gpioi2c_ov addr: %#x,  data: %#x \n",addr,data);
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

//    printf("sensor_write_register hi_i2c addr: %#x,  data: %#x \n",addr,data);
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
    sensor_write_register(0x0e, 0x1D);
    sensor_write_register(0x0f, 0x0B);
    sensor_write_register(0x10, 0x26);
    sensor_write_register(0x11, 0x80);
    sensor_write_register(0x1B, 0x4F);
    sensor_write_register(0x1D, 0xFF);

    sensor_write_register(0x1E, 0x9F);
    sensor_write_register(0x20, 0x72);
    sensor_write_register(0x21, 0x06);
    sensor_write_register(0x22, 0xFF);
    sensor_write_register(0x23, 0x02);
    sensor_write_register(0x24, 0x00);
    sensor_write_register(0x25, 0xE0);
    sensor_write_register(0x26, 0x25);
    sensor_write_register(0x27, 0xE9);
    sensor_write_register(0x28, 0x0D);
    sensor_write_register(0x29, 0x00);
    sensor_write_register(0x2C, 0x00);
    sensor_write_register(0x2D, 0x08);
    sensor_write_register(0x2E, 0xC4);
    sensor_write_register(0x2F, 0x20);
    sensor_write_register(0x6C, 0x90);
    sensor_write_register(0x2A, 0xD4);
    sensor_write_register(0x30, 0x90);
    sensor_write_register(0x31, 0x10);
    sensor_write_register(0x32, 0x10);
    sensor_write_register(0x33, 0x10);
    sensor_write_register(0x34, 0x32);
    sensor_write_register(0x14, 0x80);
    sensor_write_register(0x18, 0xD5);
    sensor_write_register(0x19, 0x10);

    sensor_write_register(0x0d, 0x00);
    sensor_write_register(0x1f, 0x00);

    sensor_write_register(0x13, 0x87);
    sensor_write_register(0x4A, 0x03);
    sensor_write_register(0x49, 0x06); 
    
    return ;
}



