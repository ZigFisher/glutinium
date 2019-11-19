#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "hi_comm_video.h"

#ifdef HI_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "hi_i2c.h"
#endif

const unsigned int sensor_i2c_addr	=	0x6c;		/* I2C Address of OV9732 */
const unsigned int sensor_addr_byte	=	2;
const unsigned int sensor_data_byte	=	1;
static int g_fd = -1;
static int flag_init = 0;

extern WDR_MODE_E genSensorMode;
extern HI_U8 gu8SensorImageMode;
extern HI_BOOL bSensorInit;

int sensor_i2c_init(void)
{
    if(g_fd >= 0)
    {
        return 0;
    }    
#ifdef HI_GPIO_I2C
    int ret;

    g_fd = open("/dev/gpioi2c_ex", 0);
    if(g_fd < 0)
    {
        printf("Open gpioi2c_ex error!\n");
        return -1;
    }
#else
    int ret;

    g_fd = open("/dev/i2c-0", O_RDWR);
    if(g_fd < 0)
    {
        printf("Open /dev/i2c-0 error!\n");
        return -1;
    }

    ret = ioctl(g_fd, I2C_SLAVE_FORCE, sensor_i2c_addr);
    if (ret < 0)
    {
        printf("CMD_SET_DEV error!\n");
        return ret;
    }
#endif

    return 0;
}

int sensor_i2c_exit(void)
{
    if (g_fd >= 0)
    {
        close(g_fd);
        g_fd = -1;
        return 0;
    }
    return -1;
}

int sensor_read_register(int addr)
{
	// TODO: 
	
	return 0;
}

int sensor_write_register(int addr, int data)
{
#ifdef HI_GPIO_I2C
    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sensor_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sensor_data_byte;

    ret = ioctl(g_fd, GPIO_I2C_WRITE, &i2c_data);

    if (ret)
    {
        printf("GPIO-I2C write faild!\n");
        return ret;
    }
#else
    if(flag_init == 0)
    {
    
	sensor_i2c_init();
	flag_init = 1;
    }

    int idx = 0;
    int ret;
    char buf[8];

    buf[idx++] = addr & 0xFF;
    if (sensor_addr_byte == 2)
    {
    	ret = ioctl(g_fd, I2C_16BIT_REG, 1);
        buf[idx++] = addr >> 8;
    }
    else
    {
    	ret = ioctl(g_fd, I2C_16BIT_REG, 0);
    }

    if (ret < 0)
    {
        printf("CMD_SET_REG_WIDTH error!\n");
        return -1;
    }

    buf[idx++] = data;
    if (sensor_data_byte == 2)
    {
    	ret = ioctl(g_fd, I2C_16BIT_DATA, 1);
        buf[idx++] = data >> 8;
    }
    else
    {
    	ret = ioctl(g_fd, I2C_16BIT_DATA, 0);
    }

    if (ret)
    {
        printf("hi_i2c write faild!\n");
        return -1;
    }

    ret = write(g_fd, buf, idx);
    if(ret < 0)
    {
    	printf("I2C_WRITE error!\n");
    	return -1;
    }
#endif
	return 0;
}


void sensor_linear_720p30_init();

void sensor_init()
{
    sensor_i2c_init();

    sensor_linear_720p30_init();

    return ;
}

void sensor_exit()
{
    sensor_i2c_exit();
	flag_init = 0;
    return;
}

/* 720P30*/
void sensor_linear_720p30_init()
{
#if 1
    sensor_write_register(0x0103, 0x01); 
    sensor_write_register(0x0100, 0x00); 
    sensor_write_register(0x3001, 0x3f); 
    sensor_write_register(0x3002, 0xff); 
    sensor_write_register(0x3007, 0x00); 
    sensor_write_register(0x3009, 0x03); 
    sensor_write_register(0x3010, 0x00); 
    sensor_write_register(0x3011, 0x00); 
    sensor_write_register(0x3014, 0x36); 
    sensor_write_register(0x301e, 0x15); 
    sensor_write_register(0x3030, 0x09); 
    sensor_write_register(0x3080, 0x02); 
    sensor_write_register(0x3081, 0x3c); 
    sensor_write_register(0x3082, 0x04); 
    sensor_write_register(0x3083, 0x00); 
    sensor_write_register(0x3084, 0x02); 
    sensor_write_register(0x3085, 0x01); 
    sensor_write_register(0x3086, 0x01); 
    sensor_write_register(0x3089, 0x01); 
    sensor_write_register(0x308a, 0x00); 
    sensor_write_register(0x3103, 0x01); 
    sensor_write_register(0x3600, 0xf6); 
    sensor_write_register(0x3601, 0x72); 
    sensor_write_register(0x3610, 0x0c); 
    sensor_write_register(0x3611, 0xf0); 
    sensor_write_register(0x3612, 0x35); 
    sensor_write_register(0x3654, 0x10); 
    sensor_write_register(0x3655, 0x77); 
    sensor_write_register(0x3656, 0x77); 
    sensor_write_register(0x3657, 0x07); 
    sensor_write_register(0x3658, 0x22); 
    sensor_write_register(0x3659, 0x22); 
    sensor_write_register(0x365a, 0x02); 
    sensor_write_register(0x3700, 0x1f); 
    sensor_write_register(0x3701, 0x10); 
    sensor_write_register(0x3702, 0x0c); 
    sensor_write_register(0x3703, 0x07); 
    sensor_write_register(0x3704, 0x3c); 
    sensor_write_register(0x3705, 0x41); 
    sensor_write_register(0x370d, 0x10); 
    sensor_write_register(0x3710, 0x0c); 
    sensor_write_register(0x3783, 0x08); 
    sensor_write_register(0x3784, 0x05); 
    sensor_write_register(0x3785, 0x55); 
    sensor_write_register(0x37c0, 0x07); 
    sensor_write_register(0x3800, 0x00); 
    sensor_write_register(0x3801, 0x04); 
    sensor_write_register(0x3802, 0x00); 
    sensor_write_register(0x3803, 0x04); 
    sensor_write_register(0x3804, 0x05); 
    sensor_write_register(0x3805, 0x0b); 
    sensor_write_register(0x3806, 0x02); 
    sensor_write_register(0x3807, 0xdb); 
    sensor_write_register(0x3808, 0x05); 
    sensor_write_register(0x3809, 0x00); 
    sensor_write_register(0x380a, 0x02); 
    sensor_write_register(0x380b, 0xd0); 
    sensor_write_register(0x380c, 0x05); 
    sensor_write_register(0x380d, 0xc6); 
    sensor_write_register(0x380e, 0x03); 
    //sensor_write_register(0x380f, 0x22); 
    sensor_write_register(0x380f, 0x2c); 
    sensor_write_register(0x3810, 0x00); 
    sensor_write_register(0x3811, 0x04); 
    sensor_write_register(0x3812, 0x00); 
    sensor_write_register(0x3813, 0x04); 
    sensor_write_register(0x3816, 0x00); 
    sensor_write_register(0x3817, 0x00); 
    sensor_write_register(0x3818, 0x00); 
    sensor_write_register(0x3819, 0x04); 
    sensor_write_register(0x3820, 0x10); 
    sensor_write_register(0x3821, 0x00); 
    sensor_write_register(0x382c, 0x06); 
    sensor_write_register(0x3500, 0x00); 
    sensor_write_register(0x3501, 0x31); 
    sensor_write_register(0x3502, 0x00); 
    sensor_write_register(0x3503, 0x03); 
    sensor_write_register(0x3504, 0x00); 
    sensor_write_register(0x3505, 0x00); 
    sensor_write_register(0x3509, 0x10); 
    sensor_write_register(0x350a, 0x00); 
    sensor_write_register(0x350b, 0x40); 
    sensor_write_register(0x3d00, 0x00); 
    sensor_write_register(0x3d01, 0x00); 
    sensor_write_register(0x3d02, 0x00); 
    sensor_write_register(0x3d03, 0x00); 
    sensor_write_register(0x3d04, 0x00); 
    sensor_write_register(0x3d05, 0x00); 
    sensor_write_register(0x3d06, 0x00); 
    sensor_write_register(0x3d07, 0x00); 
    sensor_write_register(0x3d08, 0x00); 
    sensor_write_register(0x3d09, 0x00); 
    sensor_write_register(0x3d0a, 0x00); 
    sensor_write_register(0x3d0b, 0x00); 
    sensor_write_register(0x3d0c, 0x00); 
    sensor_write_register(0x3d0d, 0x00); 
    sensor_write_register(0x3d0e, 0x00); 
    sensor_write_register(0x3d0f, 0x00); 
    sensor_write_register(0x3d80, 0x00); 
    sensor_write_register(0x3d81, 0x00); 
    sensor_write_register(0x3d82, 0x38); 
    sensor_write_register(0x3d83, 0xa4); 
    sensor_write_register(0x3d84, 0x00); 
    sensor_write_register(0x3d85, 0x00); 
    sensor_write_register(0x3d86, 0x1f); 
    sensor_write_register(0x3d87, 0x03); 
    sensor_write_register(0x3d8b, 0x00); 
    sensor_write_register(0x3d8f, 0x00); 
    sensor_write_register(0x4001, 0xe0); 
    sensor_write_register(0x4004, 0x00); 
    sensor_write_register(0x4005, 0x02); 
    sensor_write_register(0x4006, 0x01); 
    sensor_write_register(0x4007, 0x40); 
    sensor_write_register(0x4009, 0x0b); 
    sensor_write_register(0x4300, 0x03); 
    sensor_write_register(0x4301, 0xff); 
    sensor_write_register(0x4304, 0x00); 
    sensor_write_register(0x4305, 0x00); 
    sensor_write_register(0x4309, 0x00); 
    sensor_write_register(0x4600, 0x00); 
    sensor_write_register(0x4601, 0x04); 
    sensor_write_register(0x4800, 0x04); 
    sensor_write_register(0x4805, 0x00); 
    sensor_write_register(0x4821, 0x3c); 
    sensor_write_register(0x4823, 0x3c); 
    sensor_write_register(0x4837, 0x2d); 
    sensor_write_register(0x4a00, 0x00); 
    sensor_write_register(0x4f00, 0x80); 
    sensor_write_register(0x4f01, 0x10); 
    sensor_write_register(0x4f02, 0x00); 
    sensor_write_register(0x4f03, 0x00); 
    sensor_write_register(0x4f04, 0x00); 
    sensor_write_register(0x4f05, 0x00); 
    sensor_write_register(0x4f06, 0x00); 
    sensor_write_register(0x4f07, 0x00); 
    sensor_write_register(0x4f08, 0x00); 
    sensor_write_register(0x4f09, 0x00); 
    sensor_write_register(0x5000, 0x3f);
    sensor_write_register(0x500c, 0x00); 
    sensor_write_register(0x500d, 0x00); 
    sensor_write_register(0x500e, 0x00); 
    sensor_write_register(0x500f, 0x00); 
    sensor_write_register(0x5010, 0x00); 
    sensor_write_register(0x5011, 0x00); 
    sensor_write_register(0x5012, 0x00); 
    sensor_write_register(0x5013, 0x00); 
    sensor_write_register(0x5014, 0x00); 
    sensor_write_register(0x5015, 0x00); 
    sensor_write_register(0x5016, 0x00); 
    sensor_write_register(0x5017, 0x00); 
    sensor_write_register(0x5080, 0x00); 
    sensor_write_register(0x5180, 0x01); 
    sensor_write_register(0x5181, 0x00); 
    sensor_write_register(0x5182, 0x01); 
    sensor_write_register(0x5183, 0x00); 
    sensor_write_register(0x5184, 0x01); 
    sensor_write_register(0x5185, 0x00); 
    sensor_write_register(0x5708, 0x06); 
    sensor_write_register(0x5781, 0x00);
    sensor_write_register(0x5782, 0x77);
    sensor_write_register(0x5783, 0x0f); 
    sensor_write_register(0x0100, 0x01); 
#else
    sensor_write_register(0x0103, 0x01); 
    sensor_write_register(0x0100, 0x00); 
    sensor_write_register(0x3001, 0x00); 
    sensor_write_register(0x3002, 0x00); 
    sensor_write_register(0x3007, 0x00); 
    sensor_write_register(0x3009, 0x02); 
    sensor_write_register(0x3010, 0x00); 
    sensor_write_register(0x3011, 0x08); 
    sensor_write_register(0x3014, 0x22); 
    sensor_write_register(0x301e, 0x15); 
    sensor_write_register(0x3030, 0x19); 
    sensor_write_register(0x3080, 0x02); 
    sensor_write_register(0x3081, 0x3c); 
    sensor_write_register(0x3082, 0x04); 
    sensor_write_register(0x3083, 0x00); 
    sensor_write_register(0x3084, 0x02); 
    sensor_write_register(0x3085, 0x01); 
    sensor_write_register(0x3086, 0x01); 
    sensor_write_register(0x3089, 0x01); 
    sensor_write_register(0x308a, 0x00); 
    sensor_write_register(0x3103, 0x01); 
    sensor_write_register(0x3600, 0xf6); 
    sensor_write_register(0x3601, 0x72); 
    sensor_write_register(0x3610, 0x0c); 
    sensor_write_register(0x3611, 0xf0); 
    sensor_write_register(0x3612, 0x35); 
    sensor_write_register(0x3654, 0x10); 
    sensor_write_register(0x3655, 0x77); 
    sensor_write_register(0x3656, 0x77); 
    sensor_write_register(0x3657, 0x07); 
    sensor_write_register(0x3658, 0x22); 
    sensor_write_register(0x3659, 0x22); 
    sensor_write_register(0x365a, 0x02); 
    sensor_write_register(0x3700, 0x1f); 
    sensor_write_register(0x3701, 0x10); 
    sensor_write_register(0x3702, 0x0c); 
    sensor_write_register(0x3703, 0x07); 
    sensor_write_register(0x3704, 0x3c); 
    sensor_write_register(0x3705, 0x41); 
    sensor_write_register(0x370d, 0x10); 
    sensor_write_register(0x3710, 0x0c); 
    sensor_write_register(0x3783, 0x08); 
    sensor_write_register(0x3784, 0x05); 
    sensor_write_register(0x3785, 0x55); 
    sensor_write_register(0x37c0, 0x07); 
    sensor_write_register(0x3800, 0x00); 
    sensor_write_register(0x3801, 0x04); 
    sensor_write_register(0x3802, 0x00); 
    sensor_write_register(0x3803, 0x04); 
    sensor_write_register(0x3804, 0x05); 
    sensor_write_register(0x3805, 0x0b); 
    sensor_write_register(0x3806, 0x02); 
    sensor_write_register(0x3807, 0xdb); 
    sensor_write_register(0x3808, 0x05); 
    sensor_write_register(0x3809, 0x00); 
    sensor_write_register(0x380a, 0x02); 
    sensor_write_register(0x380b, 0xd0); 
    sensor_write_register(0x380c, 0x05); 
    sensor_write_register(0x380d, 0xc6); 
    sensor_write_register(0x380e, 0x03); 
    sensor_write_register(0x380f, 0x22); 
//    sensor_write_register(0x380f, 0x2c); 
    sensor_write_register(0x3810, 0x00); 
    sensor_write_register(0x3811, 0x04); 
    sensor_write_register(0x3812, 0x00); 
    sensor_write_register(0x3813, 0x04); 
    sensor_write_register(0x3816, 0x00); 
    sensor_write_register(0x3817, 0x00); 
    sensor_write_register(0x3818, 0x00); 
    sensor_write_register(0x3819, 0x04); 
    sensor_write_register(0x3820, 0x10); 
    sensor_write_register(0x3821, 0x00); 
    sensor_write_register(0x382c, 0x06); 
    sensor_write_register(0x3500, 0x00); 
    sensor_write_register(0x3501, 0x31); 
    sensor_write_register(0x3502, 0x00); 
    sensor_write_register(0x3503, 0x03); 
    sensor_write_register(0x3504, 0x00); 
    sensor_write_register(0x3505, 0x00); 
    sensor_write_register(0x3509, 0x10); 
    sensor_write_register(0x350a, 0x00); 
    sensor_write_register(0x350b, 0x40); 
    sensor_write_register(0x3d00, 0x00); 
    sensor_write_register(0x3d01, 0x00); 
    sensor_write_register(0x3d02, 0x00); 
    sensor_write_register(0x3d03, 0x00); 
    sensor_write_register(0x3d04, 0x00); 
    sensor_write_register(0x3d05, 0x00); 
    sensor_write_register(0x3d06, 0x00); 
    sensor_write_register(0x3d07, 0x00); 
    sensor_write_register(0x3d08, 0x00); 
    sensor_write_register(0x3d09, 0x00); 
    sensor_write_register(0x3d0a, 0x00); 
    sensor_write_register(0x3d0b, 0x00); 
    sensor_write_register(0x3d0c, 0x00); 
    sensor_write_register(0x3d0d, 0x00); 
    sensor_write_register(0x3d0e, 0x00); 
    sensor_write_register(0x3d0f, 0x00); 
    sensor_write_register(0x3d80, 0x00); 
    sensor_write_register(0x3d81, 0x00); 
    sensor_write_register(0x3d82, 0x38); 
    sensor_write_register(0x3d83, 0xa4); 
    sensor_write_register(0x3d84, 0x00); 
    sensor_write_register(0x3d85, 0x00); 
    sensor_write_register(0x3d86, 0x1f); 
    sensor_write_register(0x3d87, 0x03); 
    sensor_write_register(0x3d8b, 0x00); 
    sensor_write_register(0x3d8f, 0x00); 
    sensor_write_register(0x4001, 0xe0); 
    sensor_write_register(0x4004, 0x00); 
    sensor_write_register(0x4005, 0x02); 
    sensor_write_register(0x4006, 0x01); 
    sensor_write_register(0x4007, 0x40); 
    sensor_write_register(0x4009, 0x0b); 
    sensor_write_register(0x4300, 0x03); 
    sensor_write_register(0x4301, 0xff); 
    sensor_write_register(0x4304, 0x00); 
    sensor_write_register(0x4305, 0x00); 
    sensor_write_register(0x4309, 0x00); 
    sensor_write_register(0x4600, 0x00); 
    sensor_write_register(0x4601, 0x04); 
    sensor_write_register(0x4800, 0x00); 
    sensor_write_register(0x4805, 0x00); 
    sensor_write_register(0x4821, 0x50); 
    sensor_write_register(0x4823, 0x50); 
    sensor_write_register(0x4837, 0x2d); 
    sensor_write_register(0x4a00, 0x00); 
    sensor_write_register(0x4f00, 0x80); 
    sensor_write_register(0x4f01, 0x10); 
    sensor_write_register(0x4f02, 0x00); 
    sensor_write_register(0x4f03, 0x00); 
    sensor_write_register(0x4f04, 0x00); 
    sensor_write_register(0x4f05, 0x00); 
    sensor_write_register(0x4f06, 0x00); 
    sensor_write_register(0x4f07, 0x00); 
    sensor_write_register(0x4f08, 0x00); 
    sensor_write_register(0x4f09, 0x00); 
    sensor_write_register(0x5000, 0x3f);
    sensor_write_register(0x500c, 0x00); 
    sensor_write_register(0x500d, 0x00); 
    sensor_write_register(0x500e, 0x00); 
    sensor_write_register(0x500f, 0x00); 
    sensor_write_register(0x5010, 0x00); 
    sensor_write_register(0x5011, 0x00); 
    sensor_write_register(0x5012, 0x00); 
    sensor_write_register(0x5013, 0x00); 
    sensor_write_register(0x5014, 0x00); 
    sensor_write_register(0x5015, 0x00); 
    sensor_write_register(0x5016, 0x00); 
    sensor_write_register(0x5017, 0x00); 
    sensor_write_register(0x5080, 0x00); 
    sensor_write_register(0x5180, 0x01); 
    sensor_write_register(0x5181, 0x00); 
    sensor_write_register(0x5182, 0x01); 
    sensor_write_register(0x5183, 0x00); 
    sensor_write_register(0x5184, 0x01); 
    sensor_write_register(0x5185, 0x00); 
    sensor_write_register(0x5708, 0x06); 
    sensor_write_register(0x5781, 0x00);
    sensor_write_register(0x5782, 0x77);
    sensor_write_register(0x5783, 0x0f); 
    sensor_write_register(0x0100, 0x01); 
#endif	

    bSensorInit = HI_TRUE;
    printf("=========================================================\n");
    printf("===ominivision ov9732 sensor 720P30fps(Parallel port) init success!=====\n");
    printf("=========================================================\n");

    return;
}


