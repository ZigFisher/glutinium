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
#include "i2c.h"
#endif

static const unsigned char sensor_i2c_addr	=	0x90;		/* I2C Address of MT9P031 */
static const unsigned int  sensor_addr_byte	=	1;
static const unsigned int  sensor_data_byte	=	2;
static const unsigned char pca9543_i2c_addr	=	0xe6;		/* I2C Address of PCA_9543 */


int sensor_read_register(int addr)
{
	// TODO: 
	
	return 0;
}

int sensor_write_register(int addr, int data)
{
#ifdef HI_GPIO_I2C
	int fd = -1;
	int ret, value;
	
	fd = open("/dev/gpioi2c_16", 0);
    if(fd<0)
    {
    	printf("Open gpioi2c_16 error!\n");
    	return -1;
    }
    
    value = ((sensor_i2c_addr&0xff)<<24) | ((addr&0xff)<<16) | (data&0xffff);
    
    ret = ioctl(fd, GPIO_I2C_WRITE, &value);
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
	
	fd = open("/dev/i2c", 0);
    if(fd < 0)
    {
    	printf("Open i2c device error!\n");
    	return -1;
    }

    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte = sensor_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte = sensor_data_byte;

    ret = ioctl(fd, I2C_CMD_WRITE, &i2c_data);
    if(ret)
    {  
        printf("i2c write failed!\n");
        return -1 ;
    }

	close(fd);
#endif

	return 0;
}

int general_i2c_write(unsigned char dev_addr, unsigned int addr, unsigned int data)
{
#ifdef HI_GPIO_I2C
	int fd = -1;
	int ret, value;
	
	fd = open("/dev/gpioi2c", 0);
    if(fd<0)
    {
    	printf("Open gpioi2c error!\n");
    	return -1;
    }
    
    value = ((dev_addr&0xff)<<24) | ((addr&0xff)<<16) | (data&0xffff);
    
    ret = ioctl(fd, GPIO_I2C_WRITE, &value);
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
	
	fd = open("/dev/i2c", 0);
    if(fd < 0)
    {
    	printf("Open i2c device error!\n");
    	return -1;
    }

    i2c_data.dev_addr = dev_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte = 1;
    i2c_data.data = data;
    i2c_data.data_byte = 1;
    
    ret = ioctl(fd, I2C_CMD_WRITE, &i2c_data);
    if(ret)
    {  
        printf("i2c write failed!\n");
        return -1 ;
    }

	close(fd);
#endif

	return 0;
}


static void delay_ms(int ms) 
{

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
#ifdef HI_FPGA
	int framerate = 23;
#else
	int framerate = 30;
#endif	

	// PCA_9543 Init
	general_i2c_write(pca9543_i2c_addr, 0x1, 0x1);
	usleep(1000);
	
	sensor_write_register(0x0D,1);//RESET_REG
	usleep(1000);
	sensor_write_register(0x0D,0);//RESET_REG
	usleep(600000);
	//demo setup
	sensor_write_register(0x08, 0x0000);	//(7) SHUTTER_WIDTH_HI
    sensor_write_register(0x09, 0x00E6);	//(8) INTEG_TIME_REG
	sensor_write_register(0x0C, 0x0613);	//(7) SHUTTER_DELAY_REG
	sensor_write_register(0x2B, 0x0008);	//(3) GREEN1_GAIN_REG
	sensor_write_register(0x2C, 0x0008);	//(3) BLUE_GAIN_REG
	sensor_write_register(0x2D, 0x0008);	//(3) RED_GAIN_REG
	sensor_write_register(0x2E, 0x0008);	//(3) GREEN2_GAIN_REG
	// set 72 meg
	sensor_write_register(0x05,0x0600);// HORZ_BLANK_REG (HB)=1536
	sensor_write_register(0x10,0x0051);// PLL_CONTROL; POWER UP PLL

	switch ( framerate )
	{
	    case 4 :
			// 14MHz PixClk	1080p@5fps	== 4.6Hz
			sensor_write_register(0x11, 0x1801);	// PLL_CONFIG_1: m=24, n=1
			sensor_write_register(0x12, 0x0013);	// PLL_CONFIG_2: p1=16, p2=0  -> our=24Mhz*m/(n+1)/(p1+1) = 24*24/2/20=14
	        break;
	    case 5 :
			// 16MHz PixClk	1080p@6fps	== 5.2Hz
			sensor_write_register(0x11, 0x1801);	// PLL_CONFIG_1: m=24, n=1
			sensor_write_register(0x12, 0x0011);	// PLL_CONFIG_2: p1=16, p2=0  -> our=24Mhz*m/(n+1)/(p1+1) = 24*24/2/18=16
	        break;
	    case 12 :
			// 36MHz PixClk	1080p@12fps
			sensor_write_register(0x11, 0x1801);	// PLL_CONFIG_1: m=24, n=1
			sensor_write_register(0x12, 0x0007);	// PLL_CONFIG_2: p1=7, p2=0  -> our=24Mhz*m/(n+1)/(p1+1) = 24*24/2/8=36
	        break;
	    case 15 :
			// 48MHz PixClk	1080p@15fps
			sensor_write_register(0x11, 0x1801);	// PLL_CONFIG_1: m=24, n=1
			sensor_write_register(0x12, 0x0005);	// PLL_CONFIG_2: p1=3, p2=0  -> our=24Mhz*m/(n+1)/(p1+1) = 24*24/2/6=48
	        break;
	    case 23 :
			// 72MHz PixClk	1080p@23fps
			sensor_write_register(0x11, 0x1801);	// PLL_CONFIG_1: m=24, n=1
			sensor_write_register(0x12, 0x0003);	// PLL_CONFIG_2: p1=3, p2=0  -> our=24Mhz*m/(n+1)/(p1+1) = 24*24/2/4=72
	        break;
	    case 30 :
			// 96MHz PixClk	1080p@31fps
			sensor_write_register(0x11, 0x1001);	// PLL_CONFIG_1: m=16, n=1
			sensor_write_register(0x12, 0x0001);	// PLL_CONFIG_2: p1=1, p2=0  -> our=24Mhz*m/(n+1)/(p1+1) = 24*16/2/2=96
	        break;
	    default:
			// 72MHz PixClk	1080p@23fps
			sensor_write_register(0x11, 0x1801);	// PLL_CONFIG_1: m=24, n=1
			sensor_write_register(0x12, 0x0003);	// PLL_CONFIG_2: p1=3, p2=0  -> our=24Mhz*m/(n+1)/(p1+1) = 24*24/2/4=72
	}

	usleep(1000);			// Wait 1ms for VCO to lock
	sensor_write_register(0x10, 0x0053); 	// PLL_CONTROL; USE PLL
	usleep(200*1000);
	sensor_write_register(0x05, 0x01C2);	//(1) HORZ_BLANK_REG
	sensor_write_register(0x07, 0x1F8E - 4);	//Enable Parallel Fifo Data
	usleep(200*1000);
	// set geometry
#if 0
	sensor_write_register(0x01, 0x0020);		// ROW_WINDOW_START_REG
	sensor_write_register(0x02, 0x0018);		// COL_WINDOW_START_REG
#else
	// remove dark rows
	sensor_write_register(0x01, 0x0036);		// ROW_WINDOW_START_REG
	sensor_write_register(0x02, 0x0010);		// COL_WINDOW_START_REG
#endif
	sensor_write_register(0x03, 0x05FF);	 	// ROW_WINDOW_SIZE_REG=1536
	sensor_write_register(0x04, 0x07FF);	 	// COL_WINDOW_SIZE_REG=2048

	sensor_write_register(0x22, 0x0000);	 	// ROW_MODE, ROW_SKIP=0. ROW_BIN=0
	sensor_write_register(0x23, 0x0000);	 	// COL_MODE, COL_SKIP=0, COL_BIN=0
	sensor_write_register(0x08, 0x0000);		//(1) SHUTTER_WIDTH_HI
	sensor_write_register(0x09, 0x0296);		//(1) INTEG_TIME_REG
	sensor_write_register(0x0C, 0x0000);		//(1) SHUTTER_DELAY_REG


	sensor_write_register(0x1e, 0x4406);		// Continuous LV mode
//	sensor_write_register(0x1e, 0x4806);		// XOR LV mode
//	sensor_write_register(0x1e, 0x4006);		// Default LV mode	

	sensor_write_register(0x06, 0x09);			// Vertical blanking
	sensor_write_register(0x09, 0x0469);		//(1) INTEG_TIME_REG		

	printf("Aptina MT9P031 sensor 3M fr(%d) init success!\n", framerate);
}

