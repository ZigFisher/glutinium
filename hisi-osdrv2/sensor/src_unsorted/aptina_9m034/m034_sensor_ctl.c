#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "m034_sensor_config.h"

#ifdef HI_GPIO_I2C
#include "gpioi2c_32.h"
#include "gpio_i2c.h"
#else
#include "hi_i2c.h"
#endif

const unsigned char sensor_i2c_addr	=	0x20;		/* I2C Address of 9m034 */
const unsigned int  sensor_addr_byte	=	2;
const unsigned int  sensor_data_byte	=	2;

int sensor_read_register(int addr)
{
	// TODO: 
	
	return 0;
}

int sensor_write_register(int addr, int data)
{
#ifdef HI_GPIO_I2C
    int fd = -1;
    int ret;
    Aptina_9M034_DATA  i2c_data;
    
    fd = open("/dev/gpioi2c_32", 0);
    if(fd<0)
    {
        printf("Open gpioi2c_32 error!\n");
        return -1;
    }

    i2c_data.I2cDevAddr = sensor_i2c_addr; 
    i2c_data.I2cRegAddr = addr    ;
    i2c_data.RWData     = data   ;

    ret = ioctl(fd, GPIO_I2C_WRITE, &i2c_data);

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
    printf("linear mode\n");

    /* program sensor to linear mode */
    sensor_prog(sensor_rom_30_lin);

    /* Enable DCG */
    sensor_write_register(0x3100, 0x001E);

    /* Enable 1.25x analog gain */
    sensor_write_register(0x3EE4, 0xD308); 
}


