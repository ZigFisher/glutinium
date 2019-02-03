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

static const unsigned char sensor_i2c_addr	=	0x6c;		/* I2C Address of OV2715 */
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
	printf("OV2715 sensor init success!\n");
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

int setup_30fps(void)
{
    int delay = 0;
    int ret;

    sensor_write_register(0x3103, 0x03); //PLL clock select
    sensor_write_register(0x3008, 0x82); //System control RESET
    //A long pause to ensure reset has happened
    while(delay < 8000000)
        delay++;
	usleep(600000);
    sensor_write_register(0x3103 , 0x03);

    //use external regulator
    ret = sensor_read_register(0x3030);                // ...eh ?  cannot find this control reg in any documentation
    printf("3030 returned %02x\n",ret);
    ret |= 0x20;

	sensor_write_register(0x3030 , 0x20);              // ...eh ?  cannot find this control reg in any documentation

    sensor_write_register(0x3017, 0x7f); //Pad IO
    sensor_write_register(0x3018, 0xfc); //Pad IO
    sensor_write_register(0x302C, 0xC0); //Pad Drive strength
    sensor_write_register(0x3818, 0xA0); //Vertical flip !!
    sensor_write_register(0x3801, 0xa0); //HREF Timing
    sensor_write_register(0x3706, 0x61); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3712, 0x0c); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3630, 0x6d); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3801, 0xb4); //HREF Timing
    sensor_write_register(0x3621, 0x04); //!!!!!! Not in Data Sheet (bit3) !!!!!!!
    sensor_write_register(0x3604, 0x60); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3603, 0xa7); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3631, 0x26); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3600, 0x04); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3620, 0x07); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3620, 0x37); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3623, 0x40); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3623, 0x00); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3702, 0x9e); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3703, 0x74); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3704, 0x10); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x370d, 0x07); //!!!!!! Not in Data Sheet (bits[2:0]) !!!!!!!
    sensor_write_register(0x3713, 0x8b); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3714, 0x74); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3710, 0x9e); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3801, 0xc4); //Horizontal Start
    sensor_write_register(0x3605, 0x05); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3606, 0x12); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x302d, 0x90); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x370b, 0x40); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x380d, 0x74); //Horizontal Size
    sensor_write_register(0x5181, 0x20); //AWB Delta
    sensor_write_register(0x518f, 0x00); //AWB Frame Counter
    sensor_write_register(0x4301, 0xff); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x4303, 0x00); //!!!!!! Not in Data Sheet !!!!!!!
    sensor_write_register(0x3a00, 0x78); //AEC control
    sensor_write_register(0x300f, 0x88); //PLL1 CTRL
//    sensor_write_register(0x3011, 0x28); //PLL1 CTRL2
    sensor_write_register(0x3011, 0x20); //PLL1 CTRL2 -- set 72meg freq
    sensor_write_register(0x3a1a, 0x06); //!!!!!! Data Sheet says NOT USED !!!!!!!
    sensor_write_register(0x3a18, 0x00); //AGC G CEIL
    sensor_write_register(0x3a19, 0x7a); //AGC G CEIL
    sensor_write_register(0x3503, 0x00); //AGC AEC VTS CTRL
    sensor_write_register(0x3a13, 0x54); //AEC CTRL pre gain
    sensor_write_register(0x5688, 0x03); //!!!!!! Data Sheet says NOT USED !!!!!!!
    sensor_write_register(0x5684, 0x07); //AVG start V msb
    sensor_write_register(0x5685, 0xa0); //AVG start V lsb
    sensor_write_register(0x5686, 0x04); //AVG end   V msb
    sensor_write_register(0x5687, 0x43); //AVG end   V lsb
    sensor_write_register(0x3a0f, 0x40); //AEC CTRL high limit enter
    sensor_write_register(0x3a10, 0x38); //AEC CTRL low limit enter
    sensor_write_register(0x3a1b, 0x48); //AEC CTRL high limit go out
    sensor_write_register(0x3a1e, 0x30); //AEC CTRL low limit go out
    sensor_write_register(0x3a11, 0x90); //AEC CTRL fast zone high limit
    sensor_write_register(0x3a1f, 0x10); //AEC CTRL fast zone low limit

    //AEC and AGC on
    //sensor_write_register(0x3503, 0x00);
    //AWB on
    //sensor_write_register(0x3406, 0x00);

    //VTS, AEC and AGC to manual
    sensor_write_register(0x3503, 0x07);

    //set exposure
    sensor_write_register(0x3500, 0x0);
    sensor_write_register(0x3501, 0x43);
    sensor_write_register(0x3502, 0x00);

    //set VTS
    sensor_write_register(0x350C, 0x04);
    sensor_write_register(0x350D, 0x36);

    //set gain
    sensor_write_register(0x350A, 0x01);
    sensor_write_register(0x350B, 0x01);

    //enable group write register
    sensor_write_register(0x3212, 0x80);

    //AEC control.. 78 was the suggested value but I have tried a few others!!
//    sensor_write_register(0x3A00, 0x08);
//    sensor_write_register(0x3A00, 0x78);
//    sensor_write_register(0x3A00, 0x7C);

    //put colour bars on
//    sensor_write_register(0x503D, 0xA0);


    return 0;
}

/*****************************************************************************
 Prototype       : ov_sensor_init
 Description     : True 1080P@30fps at our FPGA board.
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
	sensor_write_register( 0x3103, 0x02);	// pll clock: [1]from pre divider
	sensor_write_register( 0x3008, 0x82);	// system control: [7]software reset mode
    //A long pause to ensure reset has happened
    while(delay < 8000000)
        delay++;
	usleep(600000);
    //sensor_write_register( 0x3103 , 0x03); //导致其他器件I2C异常
	sensor_write_register( 0x3030, 0x20);	// voltage change: 4x drive cap
	sensor_write_register( 0x3017, 0x7f);	// pad output enable1: vsync href data9~6
	sensor_write_register( 0x3018, 0xfc);	// pad output enable2: date5~0
	sensor_write_register( 0x302C, 0xC0); 
	sensor_write_register( 0x3818, 0xA0); 
	sensor_write_register( 0x3801, 0xa0); //no difference
	sensor_write_register( 0x3706, 0x61);	
	sensor_write_register( 0x3712, 0x0c);
	sensor_write_register( 0x3630, 0x6d);
	sensor_write_register( 0x3801, 0xb4);
	sensor_write_register( 0x3621, 0x04);
	sensor_write_register( 0x3604, 0x60);
	sensor_write_register( 0x3603, 0xa7);
	sensor_write_register( 0x3631, 0x26);
	sensor_write_register( 0x3600, 0x04);
	sensor_write_register( 0x3620, 0x37);
	sensor_write_register( 0x3623, 0x00);
	sensor_write_register( 0x3702, 0x9e);
	sensor_write_register( 0x3703, 0x74);
	sensor_write_register( 0x3704, 0x10);
	sensor_write_register( 0x370d, 0x07);
	sensor_write_register( 0x3713, 0x8b);
	sensor_write_register( 0x3714, 0x74);
	sensor_write_register( 0x3710, 0x9e);
	sensor_write_register( 0x3801, 0xc4);
	sensor_write_register( 0x3605, 0x05);
	sensor_write_register( 0x3606, 0x12);
	sensor_write_register( 0x302d, 0x90);
	sensor_write_register( 0x370b, 0x40);
	sensor_write_register( 0x380d, 0x74);
	sensor_write_register( 0x5181, 0x20);
	sensor_write_register( 0x518f, 0x00);
	sensor_write_register( 0x4301, 0xff);
	sensor_write_register( 0x4303, 0x00);
	sensor_write_register( 0x3a00, 0x78);
	sensor_write_register( 0x300f, 0x88);
	sensor_write_register( 0x3011, 0x28);
	sensor_write_register( 0x3a1a, 0x06);
	sensor_write_register( 0x3a18, 0x00);
	sensor_write_register( 0x3a19, 0x7a);
	sensor_write_register( 0x3a13, 0x54);
	sensor_write_register( 0x382e, 0x0f);
	sensor_write_register( 0x381a, 0x1a);
	sensor_write_register( 0x5688, 0x03);
	sensor_write_register( 0x5684, 0x07);
	sensor_write_register( 0x5685, 0xa0);
	sensor_write_register( 0x5686, 0x04);
	sensor_write_register( 0x5687, 0x43);
	sensor_write_register( 0x3a0f, 0x40);
	sensor_write_register( 0x3a10, 0x38);
	sensor_write_register( 0x3a1b, 0x48);
	sensor_write_register( 0x3a1e, 0x30);
	sensor_write_register( 0x3a11, 0x90);
	sensor_write_register( 0x3a1f, 0x10);

#if 1
	//VTS, AEC and AGC to manual
	sensor_write_register( 0x3503, 0x07);

	sensor_write_register( 0x3406, 0x01);

	//set exposure
	sensor_write_register( 0x3500, 0x0);
	sensor_write_register( 0x3501, 0x43);
	sensor_write_register( 0x3502, 0x00);
    
	sensor_write_register( 0x350C, 0x0);
	sensor_write_register( 0x350D, 0x0);

	//set gain
	sensor_write_register( 0x350A, 0x00);
	sensor_write_register( 0x350B, 0x00);

	//enable group write register
	sensor_write_register( 0x3212, 0x80);
	// group 0 start
	sensor_write_register( 0x3212, 0x00);
	// fill the group with target registers
	// group 0 end
	sensor_write_register( 0x3212, 0x10);

	// BLC no update at gain change
	sensor_write_register( 0x401D, 0x2);

	// Disable ISP
	sensor_write_register( 0x5000, 0x0);

	// Disable AWB
	sensor_write_register( 0x5001, 0x0);
	
#endif

	// flip
	sensor_write_register( 0x3818, 0xa0);

	// mirror
	sensor_write_register( 0x3818, 0xe0);
	sensor_write_register( 0x3621, 0x14);

	sensor_write_register( 0x3400, 0x04);
	sensor_write_register( 0x3401, 0x00);
	sensor_write_register( 0x3402, 0x04);
	sensor_write_register( 0x3403, 0x00);
	sensor_write_register( 0x3404, 0x04);
	sensor_write_register( 0x3405, 0x00);

    
	return 0;
}

int setup_30fps_mk2(void)
{
    int delay = 0;
    int ret;

    sensor_write_register(0x3103 , 0x03);
    sensor_write_register(0x3008 , 0x82);
    //A long pause to ensure reset has happened
    while(delay < 8000000)
        delay++;
    sensor_write_register(0x3103 , 0x03);

    //use external regulator
    ret = sensor_read_register(0x3030);
    //printf("3030 returned %02x\n",ret);
    ret |= 0x20;

    sensor_write_register(0x3017 , 0x7f);
    sensor_write_register(0x3018 , 0xfc);
    sensor_write_register(0x3706 , 0x61);
    sensor_write_register(0x3712 , 0x0c);
    sensor_write_register(0x3630 , 0x6d);
    sensor_write_register(0x3801 , 0xb4);
    sensor_write_register(0x3621 , 0x04);
    sensor_write_register(0x3604 , 0x60);
    sensor_write_register(0x3603 , 0xa7);
    sensor_write_register(0x3631 , 0x26);
    sensor_write_register(0x3600 , 0x04);
    sensor_write_register(0x3620 , 0x37);
    sensor_write_register(0x3623 , 0x00);
    sensor_write_register(0x3702 , 0x9e);
    sensor_write_register(0x3703 , 0x74);
    sensor_write_register(0x3704 , 0x10);
    sensor_write_register(0x370d , 0x07);
    sensor_write_register(0x3713 , 0x8b);
    sensor_write_register(0x3714 , 0x74);
    sensor_write_register(0x3710 , 0x9e);
    sensor_write_register(0x3801 , 0xc4);
    sensor_write_register(0x3605 , 0x05);
    sensor_write_register(0x3606 , 0x12);
    sensor_write_register(0x302d , 0x90);
    sensor_write_register(0x370b , 0x40);
    sensor_write_register(0x380d , 0x74);
    sensor_write_register(0x5181 , 0x20);
    sensor_write_register(0x518f , 0x00);
    sensor_write_register(0x4301 , 0xff);
    sensor_write_register(0x4303 , 0x00);
    sensor_write_register(0x3a00 , 0x78);
    sensor_write_register(0x300f , 0x88);
    sensor_write_register(0x3011 , 0x28);
    sensor_write_register(0x3a1a , 0x06);
    sensor_write_register(0x3a18 , 0x00);
    sensor_write_register(0x3a19 , 0x7a);
    sensor_write_register(0x3a13 , 0x54);
    sensor_write_register(0x382e , 0x0f);
    sensor_write_register(0x381a , 0x1a);
    sensor_write_register(0x5688 , 0x03);
    sensor_write_register(0x5684 , 0x07);
    sensor_write_register(0x5685 , 0xa0);
    sensor_write_register(0x5686 , 0x04);
    sensor_write_register(0x5687 , 0x43);
    sensor_write_register(0x3a0f , 0x40);
    sensor_write_register(0x3a10 , 0x38);
    sensor_write_register(0x3a1b , 0x48);
    sensor_write_register(0x3a1e , 0x30);
    sensor_write_register(0x3a11 , 0x90);
    sensor_write_register(0x3a1f , 0x10);

//   //AEC and AGC on
    sensor_write_register(0x3503, 0x00);
//    //AWB on
    sensor_write_register(0x3406, 0x00);

    //VTS, AEC and AGC to manual
//    sensor_write_register(0x3503, 0x07);

    //set exposure
//    sensor_write_register(0x3500, 0x0F);
//    sensor_write_register(0x3501, 0xFF);
//    sensor_write_register(0x3502, 0xFF);

    //set VTS
//    sensor_write_register(0x350C, 0x00);
//    sensor_write_register(0x350D, 0xFF);

    //set gain
//    sensor_write_register(0x350A, 0x01);
//    sensor_write_register(0x350B, 0xFF);

    //AEC control.. 78 was the suggested value but I have tried a few others!!
//    sensor_write_register(0x3A00, 0x08);
//   sensor_write_register(0x3A00, 0x78);
//   sensor_write_register(0x3A00, 0x7C);

    //put colour bars on
//    sensor_write_register(0x503D, 0xA8);


return 0;

}

int setup_30fps_720p(void)
{
    int delay=0;
    int ret;

    sensor_write_register(0x3103, 0x03);
    sensor_write_register(0x3008, 0x82);
    //A long pause to ensure reset has happened
    while(delay < 8000000)
        delay++;

    sensor_write_register(0x3103, 0x03);

    //use external regulator
    ret = sensor_read_register(0x3030);
    //printf("3030 returned %02x\n",ret);
    ret |= 0x20;

    sensor_write_register(0x3017, 0x7f);
    sensor_write_register(0x3018, 0xfc);
    sensor_write_register(0x3801, 0xa0);
    sensor_write_register(0x3706, 0x61);
    sensor_write_register(0x3712, 0x0c);
    sensor_write_register(0x3630, 0x6d);
    sensor_write_register(0x3801, 0xb4);
    sensor_write_register(0x3621, 0x04);
    sensor_write_register(0x3604, 0x60);
    sensor_write_register(0x3603, 0xa7);
    sensor_write_register(0x3631, 0x26);
    sensor_write_register(0x3600, 0x04);
    sensor_write_register(0x3620, 0x07);
    sensor_write_register(0x3623, 0x40);
    sensor_write_register(0x3702, 0x9e);
    sensor_write_register(0x3703, 0x74);
    sensor_write_register(0x3704, 0x10);
    sensor_write_register(0x370d, 0x07);
    sensor_write_register(0x3713, 0x8b);
    sensor_write_register(0x3714, 0x74);
    sensor_write_register(0x3710, 0x9e);
    sensor_write_register(0x3801, 0xc4);
    sensor_write_register(0x3605, 0x05);
    sensor_write_register(0x3606, 0x12);
    sensor_write_register(0x302d, 0x90);
    sensor_write_register(0x370b, 0x40);
    sensor_write_register(0x380d, 0x74);
    sensor_write_register(0x5181, 0x20);
    sensor_write_register(0x518f, 0x00);
    sensor_write_register(0x4301, 0xff);
    sensor_write_register(0x4303, 0x00);
    sensor_write_register(0x3a00, 0x78);
    sensor_write_register(0x300f, 0x88);
    sensor_write_register(0x3011, 0x28);
    sensor_write_register(0x3a1a, 0x06);
    sensor_write_register(0x3a18, 0x00);
    sensor_write_register(0x3a19, 0x7a);
    sensor_write_register(0x3503, 0x00);
    sensor_write_register(0x3a13, 0x54);
    sensor_write_register(0x381c, 0x10);
    sensor_write_register(0x381d, 0xb8);
    sensor_write_register(0x381e, 0x02);
    sensor_write_register(0x381f, 0xdc);
    sensor_write_register(0x3820, 0x0a);
    sensor_write_register(0x3821, 0x29);
    sensor_write_register(0x3804, 0x05);
    sensor_write_register(0x3805, 0x00);
    sensor_write_register(0x3806, 0x02);
    sensor_write_register(0x3807, 0xd0);
    sensor_write_register(0x3808, 0x05);
    sensor_write_register(0x3809, 0x00);
    sensor_write_register(0x380a, 0x02);
    sensor_write_register(0x380b, 0xd0);
    sensor_write_register(0x380e, 0x02);
    sensor_write_register(0x380f, 0xe8);
    sensor_write_register(0x380c, 0x07);
    sensor_write_register(0x380d, 0x00);
    sensor_write_register(0x5688, 0x03);
    sensor_write_register(0x5684, 0x05);
    sensor_write_register(0x5685, 0x00);
    sensor_write_register(0x5686, 0x02);
    sensor_write_register(0x5687, 0xd0);
    sensor_write_register(0x3a08, 0x1b);
    sensor_write_register(0x3a09, 0xe6);
    sensor_write_register(0x3a0a, 0x17);
    sensor_write_register(0x3a0b, 0x40);
    sensor_write_register(0x3a0e, 0x01);
    sensor_write_register(0x3a0d, 0x02);
    sensor_write_register(0x3a0f, 0x40);
    sensor_write_register(0x3a10, 0x38);
    sensor_write_register(0x3a1b, 0x48);
    sensor_write_register(0x3a1e, 0x30);
    sensor_write_register(0x3a11, 0x90);
    sensor_write_register(0x3a1f, 0x10);

//   //AEC and AGC on
    sensor_write_register(0x3503, 0x00);
//    //AWB on
    sensor_write_register(0x3406, 0x00);

    //VTS, AEC and AGC to manual
    sensor_write_register(0x3503, 0x07);

    //set exposure
//    sensor_write_register(0x3500, 0x0F);
//    sensor_write_register(0x3501, 0xFF);
//    sensor_write_register(0x3502, 0xFF);

    //set VTS
//    sensor_write_register(0x350C, 0x00);
//    sensor_write_register(0x350D, 0xFF);

    //set gain
//    sensor_write_register(0x350A, 0x01);
//    sensor_write_register(0x350B, 0xFF);

    //AEC control.. 78 was the suggested value but I have tried a few others!!
//    sensor_write_register(0x3A00, 0x08);
//   sensor_write_register(0x3A00, 0x78);
//   sensor_write_register(0x3A00, 0x7C);

    //put colour bars on
//    sensor_write_register(0x503D, 0xA8);

    return 0;
}

