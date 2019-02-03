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
#include "i2c.h"
#endif

static const unsigned char sensor_i2c_addr	=	0x6c;		/* I2C Address of OV5653 */
static const unsigned int  sensor_addr_byte	=	2;
static const unsigned int  sensor_data_byte	=	1;


int sensor_read_register(int addr)
{
#ifdef HI_GPIO_I2C
	int fd = -1;
	int ret, value;
	unsigned char data;
	
	fd = open("/dev/gpioi2c_ov", 0);
    if(fd<0)
    {
    	printf("Open gpioi2c_ov error!\n");
    	return -1;
    }
    
    value = ((sensor_i2c_addr&0xff)<<24) | ((addr&0xffff)<<8);

    ret = ioctl(fd, GPIO_I2C_READ, &value);
	if (ret)
	{
	    printf("GPIO-I2C-OV write faild!\n");
		return -1;
	}

	data = value&0xff;

	close(fd);
	return data;
#else
	int fd = -1;
	int ret;
	unsigned int data;
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
    i2c_data.data_byte = sensor_data_byte;

    ret = ioctl(fd, I2C_CMD_READ, &i2c_data);
    if(ret)
    {  
        printf("i2c read failed!\n");
        return -1 ;
    }

	data = i2c_data.data;

	close(fd);
	return data;
#endif		
}

int sensor_write_register(int addr, int data)
{
#ifdef HI_GPIO_I2C
	int fd = -1;
	int ret, value;
	
	fd = open("/dev/gpioi2c_ov", 0);
    if(fd<0)
    {
    	printf("Open gpioi2c_ov error!\n");
    	return -1;
    }
    
    value = ((sensor_i2c_addr&0xff)<<24) | ((addr&0xffff)<<8) | (data&0xff);
    
    ret = ioctl(fd, GPIO_I2C_WRITE, &value);
	if (ret)
	{
	    printf("GPIO-I2C-OV write faild!\n");
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

int ov_sensor_init(void);
void sensor_init()
{
	ov_sensor_init();
	printf("OV5653 sensor init success!!\n");
}


void setup_sensor(int isp_mode)
{
	if(0 == isp_mode) /* ISP 'normal' isp_mode */
	{
	}
	else if(1 == isp_mode) /* ISP pixel calibration isp_mode */
	{
		sensor_write_register( 0x350C, 0x15);
		sensor_write_register( 0x350D, 0x90);
		sensor_write_register( 0x3500, 0x01);  
		sensor_write_register( 0x3501, 0x80);
		sensor_write_register( 0x3502, 0x00);
		sensor_write_register( 0x350A, 0x00);
		sensor_write_register( 0x350B, 0x00);
	}
}

/*****************************************************************************
 Prototype       : ov_sensor_init
 Description     : 5m.
 Input           : oid  **
 Output          : None
 Return Value    : 
 Process         : 
 Note			 : 

  History         
  1.Date         : 2010/11/8
    Author       : x00100808
    Modification : Created function

*****************************************************************************/
int ov_sensor_init(void)
{
    int delay = 0;
	sensor_write_register(0x3008,0x82);
		
	while(delay < 8000000)
        delay++;
	usleep(600000);
	
	sensor_write_register(0x3008,0x42);
	
	while(delay < 8000000)
        delay++;
	usleep(600000);
	
	sensor_write_register(0x3103,0x93);
	sensor_write_register(0x3b07,0x0c);
	sensor_write_register(0x3017,0xff);
	sensor_write_register(0x3018,0xfc);
	sensor_write_register(0x3706,0x41);
	sensor_write_register(0x3703,0xe6);
	sensor_write_register(0x3613,0x44);
	sensor_write_register(0x3630,0x22);
	sensor_write_register(0x3605,0x04);
	sensor_write_register(0x3606,0x3f);
	sensor_write_register(0x3712,0x13);
	sensor_write_register(0x370e,0x00);
	sensor_write_register(0x370b,0x40);
	sensor_write_register(0x3600,0x54);
	sensor_write_register(0x3601,0x05);
	sensor_write_register(0x3713,0x22);
	sensor_write_register(0x3714,0x27);
	sensor_write_register(0x3631,0x22);
	sensor_write_register(0x3612,0x1a);
	sensor_write_register(0x3604,0x40);
	sensor_write_register(0x3705,0xda);
	sensor_write_register(0x370a,0x80);
	sensor_write_register(0x370c,0x00);
	sensor_write_register(0x3710,0x28);
	sensor_write_register(0x3702,0x3a);
	sensor_write_register(0x3704,0x18);
	sensor_write_register(0x3a18,0x00);
	sensor_write_register(0x3a19,0xf8);
	sensor_write_register(0x3a00,0x38);
	sensor_write_register(0x3800,0x02);
	sensor_write_register(0x3801,0x54);
	sensor_write_register(0x3803,0x0c);
	sensor_write_register(0x380c,0x0c);
	sensor_write_register(0x380d,0xb4);
	sensor_write_register(0x380e,0x07);
	sensor_write_register(0x380f,0xb0);
	sensor_write_register(0x3830,0x50);
	sensor_write_register(0x3a08,0x12);
	sensor_write_register(0x3a09,0x70);
	sensor_write_register(0x3a0a,0x0f);
	sensor_write_register(0x3a0b,0x60);
	sensor_write_register(0x3a0d,0x06);
	sensor_write_register(0x3a0e,0x06);
	sensor_write_register(0x3a13,0x54);
	sensor_write_register(0x3815,0x82);
	sensor_write_register(0x5059,0x80);
	sensor_write_register(0x3615,0x52);
	sensor_write_register(0x505a,0x0a);
	sensor_write_register(0x505b,0x2e);

	sensor_write_register(0x3a1a,0x06);
	sensor_write_register(0x3503,0x00);
	sensor_write_register(0x3623,0x01);
	sensor_write_register(0x3633,0x24);
	sensor_write_register(0x3c01,0x34);
	sensor_write_register(0x3c04,0x28);
	sensor_write_register(0x3c05,0x98);
	sensor_write_register(0x3c07,0x07);
	sensor_write_register(0x3c09,0xc2);
	sensor_write_register(0x4000,0x05);
	sensor_write_register(0x401d,0x28);
	sensor_write_register(0x4001,0x02);
	sensor_write_register(0x401c,0x46);
	sensor_write_register(0x5046,0x01);
	sensor_write_register(0x3810,0x40);
	sensor_write_register(0x3836,0x41);
	sensor_write_register(0x505f,0x04);
	sensor_write_register(0x5000,0x00);
	sensor_write_register(0x5001,0x00);
	sensor_write_register(0x5002,0x00);
	sensor_write_register(0x503d,0x00);
	sensor_write_register(0x5901,0x00);
	sensor_write_register(0x585a,0x01);
	sensor_write_register(0x585b,0x2c);
	sensor_write_register(0x585c,0x01);
	sensor_write_register(0x585d,0x93);
	sensor_write_register(0x585e,0x01);
	sensor_write_register(0x585f,0x90);
	sensor_write_register(0x5860,0x01);
	sensor_write_register(0x5861,0x0d);
	sensor_write_register(0x5180,0xc0);
	sensor_write_register(0x5184,0x00);
	sensor_write_register(0x470a,0x00);
	sensor_write_register(0x470b,0x00);
	sensor_write_register(0x470c,0x00);
	sensor_write_register(0x300f,0x8e);
	sensor_write_register(0x3603,0xa7);
	sensor_write_register(0x3632,0x55);
	sensor_write_register(0x3620,0x56);
	sensor_write_register(0x3621,0x2f);
	sensor_write_register(0x381a,0x3c);
	sensor_write_register(0x3818,0xc0);
	sensor_write_register(0x3631,0x36);
	sensor_write_register(0x3632,0x5f);
	sensor_write_register(0x3711,0x24);
	sensor_write_register(0x401f,0x03);
	sensor_write_register(0x3008,0x02);

	   //AEC and AGC to manual    
	   sensor_write_register( 0x3503, 0x03);       
	   //set exposure    
	   sensor_write_register( 0x3500, 0x0);    
	   sensor_write_register( 0x3501, 0x43);    
	   sensor_write_register( 0x3502, 0x00);    
	   //set gain to 0dB    
	   sensor_write_register( 0x350B, 0x0);           
	   //AWB manual control enable   
	   sensor_write_register( 0x3406, 0x01);        
	   //Gain for RGB Channel    
	   sensor_write_register( 0x3400, 0x04);    
	   sensor_write_register( 0x3401, 0x00);    
	   sensor_write_register( 0x3402, 0x04);    
	   sensor_write_register( 0x3403, 0x00);    
	   sensor_write_register( 0x3404, 0x04);    
	   sensor_write_register( 0x3405, 0x00);        
	   // Disable ISP    
	   sensor_write_register( 0x5000, 0x0);    
	   // Disable AWB    
	   sensor_write_register( 0x5001, 0x0);   

	return 0;
}


