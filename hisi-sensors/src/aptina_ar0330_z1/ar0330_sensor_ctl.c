#include <stdio.h>
#include <stdlib.h>
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

const unsigned char sensor_i2c_addr	=	0x20;              /* I2C Address of AR0330 */
const unsigned int  sensor_addr_byte	=	2;                    /* ADDR byte of AR0330 */
const unsigned int  sensor_data_byte	=	2;                    /* DATA byte of AR0330 */

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
    I2C_DATA_S  i2c_data;
    
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
    if(fd < 0)
    {
        printf("Open i2c device error!\n");
        return -1;
    }

    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sensor_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sensor_data_byte;

    ret = ioctl(fd,  CMD_I2C_WRITE, &i2c_data);
    if(ret)
    {  
        printf("i2c write failed!\n");
        return -1 ;
    }

	close(fd);
#endif
	return 0;
}

static void delay_ms(int ms) { 
    usleep(ms*1000);
}

void sensor_init()
{
    sensor_write_register(0x301A, 0x10D9);
    delay_ms(100);
	
    sensor_write_register(0x301A, 0x10D8);

	
    sensor_write_register(0x30FE, 0x0080);
    sensor_write_register(0x31E0, 0x0303);
    sensor_write_register(0x3ECE, 0x08FF);
    sensor_write_register(0x3ED0, 0xE4F6);
    sensor_write_register(0x3ED2, 0x0146);
    sensor_write_register(0x3ED4, 0x8F6C);
    sensor_write_register(0x3ED6, 0x66CC);
    sensor_write_register(0x3ED8, 0x8C42);
    sensor_write_register(0x3EDA, 0x889B);
    sensor_write_register(0x3EDC, 0x8863);
    sensor_write_register(0x3EDE, 0xAA04);
    sensor_write_register(0x3EE0, 0x15F0);
    sensor_write_register(0x3EE6, 0x008C);
    sensor_write_register(0x3EE8, 0x2024);
    sensor_write_register(0x3EEA, 0xFF1F);
    sensor_write_register(0x3F06, 0x046A);
    sensor_write_register(0x3064, 0x1802);

	
    sensor_write_register(0x3EDA, 0x88BC);
    sensor_write_register(0x3EDC, 0xAA63);

	
    sensor_write_register(0x305E, 0x00A0);

	
    sensor_write_register(0x302A, 0x0006);
    sensor_write_register(0x302C, 0x0001);
    sensor_write_register(0x302E, 0x0002);
    sensor_write_register(0x3030, 0x0025);
    sensor_write_register(0x3036, 0x000C);
    sensor_write_register(0x3038, 0x0001);
    sensor_write_register(0x31AC, 0x0C0C);

	
    sensor_write_register(0x31AE, 0x0301);
    sensor_write_register(0x3002, 0x00EA);
    sensor_write_register(0x3004, 0x00C6);
    sensor_write_register(0x3006, 0x0521);
    sensor_write_register(0x3008, 0x0845);
    sensor_write_register(0x300A, 0x0444);
    sensor_write_register(0x300C, 0x0469);
    sensor_write_register(0x3012, 0x0147);
    sensor_write_register(0x3014, 0x0000);
    sensor_write_register(0x30A2, 0x0001);
    sensor_write_register(0x30A6, 0x0001);
    sensor_write_register(0x308C, 0x0006);
    sensor_write_register(0x308A, 0x0006);
    sensor_write_register(0x3090, 0x0605);
    sensor_write_register(0x308E, 0x0905);
    sensor_write_register(0x30AA, 0x0B94);
    sensor_write_register(0x303E, 0x04E0);
    sensor_write_register(0x3016, 0x0B92);
    sensor_write_register(0x3018, 0x0000);
    sensor_write_register(0x30AE, 0x0001);
    sensor_write_register(0x30A8, 0x0001);
    sensor_write_register(0x3040, 0x0000);
    sensor_write_register(0x3042, 0x0000);
    sensor_write_register(0x30BA, 0x002C);

	
    sensor_write_register(0x3088, 0x80BA);
    sensor_write_register(0x3086, 0x0253);
    sensor_write_register(0x301A, 0x10DC);

    printf("Aptina AR0330 sensor 1080p 30fps init success!\n");
}







