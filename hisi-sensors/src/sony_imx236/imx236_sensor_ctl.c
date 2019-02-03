#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "hi_comm_isp.h"


#ifdef HI_GPIO_I2C
#include "gpioi2c_32.h"
#include "gpio_i2c.h"
#else
#include "hi_i2c.h"
#endif

#define SPI2IIC_ADDR_MAP_OFFSET 0x2e00

const unsigned char sensor_i2c_addr	=	0x34;		/* I2C Address of IMX236 */
const unsigned int  sensor_addr_byte	=	2;                  /* I2C addr byte of IMX236 */
const unsigned int  sensor_data_byte	=	1;                 /* I2C data byte of IMX236 */

void sensor_linner_init();
void sensor_wdr_init();

int sensor_read_register(int addr)
{
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
    i2c_data.reg_addr = addr + SPI2IIC_ADDR_MAP_OFFSET;
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
        
	return i2c_data.data;
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
    i2c_data.I2cRegAddr = addr + SPI2IIC_ADDR_MAP_OFFSET;
    i2c_data.RWData     = data   ;

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
    i2c_data.reg_addr = addr + SPI2IIC_ADDR_MAP_OFFSET;
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

void sensor_prog(int* rom) 
{
    return;
}

void sensor_linner_init()
{
    sensor_write_register(0x200, 0x01);     /* Standby */
    usleep(200000);
    
    sensor_write_register(0x205, 0x01);     /* 12 bit */
    sensor_write_register(0x209, 0x02);     /* Frame rate (data rate) setting: 30fps */
    sensor_write_register(0x20C, 0x00);     /* 00 normal mode, 02 Built-in WDR mode */
    sensor_write_register(0x20F, 0x01);     /* WDR */
    sensor_write_register(0x210, 0x01);     /* WDR */
    //sensor_write_register(0x212, 0x00);     /* WDR */
    sensor_write_register(0x212, 0xF0);    //
    sensor_write_register(0x213, 0x00);     /* WDR */
    sensor_write_register(0x215, 0x00);     /* WDR */
    sensor_write_register(0x244, 0x01);     /* cmos parallel output, 12bit */
    printf("\r\n-------Sony IMX236 Sensor Initial OK!-------\r\n");

    /* waiting for image stabilization */
    usleep(200000);
    sensor_write_register(0x200, 0x00);     /* release standy */
    usleep(200000);
    sensor_write_register(0x202, 0x00);     /* Master mode operation start */
    usleep(200000);
    sensor_write_register(0x249, 0x0A);     /* HSYNC and VSYNC output */
    usleep(200000);
    
    return;
}

void sensor_wdr_init()
{
    sensor_write_register(0x200, 0x01);     /* Standby */
    usleep(200000);
    
    sensor_write_register(0x205, 0x01);     /* 12 bit */
    sensor_write_register(0x209, 0x02);     /* Frame rate (data rate) setting: 30fps */
    sensor_write_register(0x20C, 0x02);     /* 00 normal mode, 02 Built-in WDR mode */
    sensor_write_register(0x20F, 0x05);     /* WDR */
    sensor_write_register(0x210, 0x00);     /* WDR */
    sensor_write_register(0x212, 0x2D);     /* WDR */
    sensor_write_register(0x213, 0x00);     /* WDR */
    sensor_write_register(0x215, 0x00);     /* WDR */

    sensor_write_register(0x244, 0x01);     /* cmos parallel output, 12bit */

    sensor_write_register(0x265, 0x00);     /* WDR */
    sensor_write_register(0x284, 0x10);     /* WDR */
    sensor_write_register(0x286, 0x10);     /* WDR */
    sensor_write_register(0x2CF, 0xE1);     /* WDR */
    sensor_write_register(0x2D0, 0x30);     /* WDR */
    sensor_write_register(0x2D2, 0xC4);     /* WDR */
    sensor_write_register(0x2D3, 0x01);     /* WDR */
    sensor_write_register(0x2CC, 0x21);     /* 21h 16times, 31h 32times */
    printf("\r\n-------Sony IMX236 Sensor Built-In WDR Initial OK!-------\r\n");

    /* waiting for image stabilization */
    usleep(200000);
    sensor_write_register(0x200, 0x00);     /* release standy */
    usleep(200000);
    sensor_write_register(0x202, 0x00);     /* Master mode operation start */
    usleep(200000);
    sensor_write_register(0x249, 0x0A);     /* HSYNC and VSYNC output */
    usleep(200000);
    
    return;
}
void sensor_init()
{
    sensor_write_register(0x200, 0x01);     /* Standby */
    usleep(200000);

    sensor_write_register(0x202, 0x01);     /* master mode stop */
    sensor_write_register(0x205, 0x01);     /* 12 bit */
    sensor_write_register(0x206, 0x00);     /* All-pix scan mode */
    sensor_write_register(0x207, 0x10);     /* 1080p mode */
    sensor_write_register(0x209, 0x02);     /* Frame rate (data rate) setting: 30fps */
    sensor_write_register(0x20A, 0xF0);     /* Black level offset */
    sensor_write_register(0x20B, 0x00);     /* Black level offset */
    sensor_write_register(0x20B, 0x00);     /* Black level offset */
    sensor_write_register(0x214, 0x0C);     /* AGC min gain 1.2dB; */
    sensor_write_register(0x218, 0x65);     /* Vertical span[7:0] */
    sensor_write_register(0x219, 0x04);     /* Vertical span[15:8] */
    sensor_write_register(0x21A, 0x00);     /* Vertical span[15:8] */
    sensor_write_register(0x21B, 0x30);     /* Horizontal span[7:0] */
    sensor_write_register(0x21C, 0x11);     /* Horizontal span[15:8] */
    sensor_write_register(0x220, 0xC0);     /* Shutter[7:0] */
    sensor_write_register(0x221, 0x03);     /* Shutter[15:8] */
    sensor_write_register(0x244, 0x01);     /* cmos parallel output, 12bit */
    sensor_write_register(0x246, 0x01);     
    sensor_write_register(0x247, 0x01);    
    sensor_write_register(0x248, 0x01);    
    sensor_write_register(0x249, 0x0A);     /* HSYNC,VSYNC output */
    sensor_write_register(0x254, 0x63);     
    sensor_write_register(0x25B, 0x00);     /* INCK setting0 */
    sensor_write_register(0x25C, 0x20);     /* INCK setting1 */
    sensor_write_register(0x25D, 0x06);     /* INCK setting2 */
    sensor_write_register(0x25E, 0x30);     /* INCK setting3 */
    sensor_write_register(0x25F, 0x04);     /* INCK setting4 */

    sensor_write_register(0x30F, 0x0E);     /* */
    sensor_write_register(0x316, 0x02);     /* */

    sensor_write_register(0x436, 0x71);     /* */
    sensor_write_register(0x439, 0xF1);     /* */
    sensor_write_register(0x441, 0xF2);     /* */
    sensor_write_register(0x442, 0x21);     /* */
    sensor_write_register(0x443, 0x21);     /* */
    sensor_write_register(0x448, 0xF2);     /* */
    sensor_write_register(0x449, 0x21);     /* */
    sensor_write_register(0x44A, 0x21);     /* */
    sensor_write_register(0x452, 0x01);     /* */
    sensor_write_register(0x454, 0xB1);     /* */
    
    /* waiting for image stabilization */
    usleep(200000);
    sensor_write_register(0x200, 0x00);     /* release standy */
    usleep(200000);
    sensor_write_register(0x202, 0x00);     /* Master mode operation start */
    usleep(200000);
    sensor_write_register(0x249, 0x0A);     /* HSYNC and VSYNC output */
    usleep(200000);
    sensor_linner_init();
}

