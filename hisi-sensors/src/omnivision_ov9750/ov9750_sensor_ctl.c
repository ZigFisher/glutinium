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

const unsigned char sensor_i2c_addr	    =	0x6C;		/* I2C Address of OV9750 */
const unsigned int  sensor_addr_byte	=	2;
const unsigned int  sensor_data_byte	=	1;
static int g_fd = -1;

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
void sensor_linear_960p30_init();

#define SENSOR_720P_30FPS_MODE  (1)
#define SENSOR_960P_30FPS_MODE  (2)


void sensor_init()
{
    sensor_i2c_init();
    
    /* When sensor first init, config all registers */
    if (HI_FALSE == bSensorInit) 
    {
        if(SENSOR_720P_30FPS_MODE == gu8SensorImageMode || SENSOR_960P_30FPS_MODE == gu8SensorImageMode)
        {
            sensor_linear_960p30_init();
        }
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else 
    {
        if(SENSOR_720P_30FPS_MODE == gu8SensorImageMode || SENSOR_960P_30FPS_MODE == gu8SensorImageMode)
        {
            sensor_linear_960p30_init();
        }
    }

    return ;
}

void sensor_exit()
{
    sensor_i2c_exit();

    return;
}

void sensor_linear_960p30_init()
{
    sensor_write_register (0x0103, 0x01);
    sensor_write_register (0x0100, 0x00);
    sensor_write_register (0x0300, 0x02);
    sensor_write_register (0x0302, 0x30);
    sensor_write_register (0x0303, 0x00);
    sensor_write_register (0x0304, 0x03);
    sensor_write_register (0x0305, 0x01);
    sensor_write_register (0x0306, 0x01);
    sensor_write_register (0x030a, 0x00);
    sensor_write_register (0x030b, 0x00);
    sensor_write_register (0x030d, 0x1e);
    sensor_write_register (0x030e, 0x04);
    sensor_write_register (0x030f, 0x04);
    sensor_write_register (0x0312, 0x01);
    sensor_write_register (0x031e, 0x04);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0x00);
    sensor_write_register (0x3002, 0x21);
    sensor_write_register (0x3005, 0xf0);
    sensor_write_register (0x3011, 0x00);
    sensor_write_register (0x3016, 0x53);
    sensor_write_register (0x3018, 0x32);
    sensor_write_register (0x301a, 0xf0);
    sensor_write_register (0x301b, 0xf0);
    sensor_write_register (0x301c, 0xf0);
    sensor_write_register (0x301d, 0xf0);
    sensor_write_register (0x301e, 0xf0);
    sensor_write_register (0x3022, 0x01);
    sensor_write_register (0x3031, 0x0c);
    sensor_write_register (0x3032, 0x80);
    sensor_write_register (0x303c, 0xff);
    sensor_write_register (0x303e, 0xff);
    sensor_write_register (0x3040, 0xf0);
    sensor_write_register (0x3041, 0x00);
    sensor_write_register (0x3042, 0xf0);
    sensor_write_register (0x3104, 0x01);
    sensor_write_register (0x3106, 0x15);
    sensor_write_register (0x3107, 0x01);
    sensor_write_register (0x3500, 0x00);
    sensor_write_register (0x3501, 0x3d);
    sensor_write_register (0x3502, 0x00);
    sensor_write_register (0x3503, 0x08);
    sensor_write_register (0x3504, 0x03);
    sensor_write_register (0x3505, 0x83);
    sensor_write_register (0x3508, 0x02);
    sensor_write_register (0x3509, 0x80);
    sensor_write_register (0x3600, 0x65);
    sensor_write_register (0x3601, 0x60);
    sensor_write_register (0x3602, 0x22);
    sensor_write_register (0x3610, 0xe8);
    sensor_write_register (0x3611, 0x56);
    sensor_write_register (0x3612, 0x48);
    sensor_write_register (0x3613, 0x5a);
    sensor_write_register (0x3614, 0x91);
    sensor_write_register (0x3615, 0x79);
    sensor_write_register (0x3617, 0x57);
    sensor_write_register (0x3621, 0x90);
    sensor_write_register (0x3622, 0x00);
    sensor_write_register (0x3623, 0x00);
    sensor_write_register (0x3625, 0x07);
    sensor_write_register (0x3633, 0x10);
    sensor_write_register (0x3634, 0x10);
    sensor_write_register (0x3635, 0x14);
    sensor_write_register (0x3636, 0x13);
    sensor_write_register (0x3650, 0x00);
    sensor_write_register (0x3652, 0xff);
    sensor_write_register (0x3654, 0x00);
    sensor_write_register (0x3653, 0x34);
    sensor_write_register (0x3655, 0x20);
    sensor_write_register (0x3656, 0xff);
    sensor_write_register (0x3657, 0xc4);
    sensor_write_register (0x365a, 0xff);
    sensor_write_register (0x365b, 0xff);
    sensor_write_register (0x365e, 0xff);
    sensor_write_register (0x365f, 0x00);
    sensor_write_register (0x3668, 0x00);
    sensor_write_register (0x366a, 0x07);
    sensor_write_register (0x366d, 0x00);
    sensor_write_register (0x366e, 0x10);
    sensor_write_register (0x3702, 0x1d);
    sensor_write_register (0x3703, 0x10);
    sensor_write_register (0x3704, 0x14);
    sensor_write_register (0x3705, 0x00);
    sensor_write_register (0x3706, 0x27);
    sensor_write_register (0x3709, 0x24);
    sensor_write_register (0x370a, 0x00);
    sensor_write_register (0x370b, 0x7d);
    sensor_write_register (0x3714, 0x24);
    sensor_write_register (0x371a, 0x5e);
    sensor_write_register (0x3730, 0x82);
    sensor_write_register (0x3733, 0x10);
    sensor_write_register (0x373e, 0x18);
    sensor_write_register (0x3755, 0x00);
    sensor_write_register (0x3758, 0x00);
    sensor_write_register (0x375b, 0x13);
    sensor_write_register (0x3772, 0x23);
    sensor_write_register (0x3773, 0x05);
    sensor_write_register (0x3774, 0x16);
    sensor_write_register (0x3775, 0x12);
    sensor_write_register (0x3776, 0x08);
    sensor_write_register (0x37a8, 0x38);
    sensor_write_register (0x37b5, 0x36);
    sensor_write_register (0x37c2, 0x04);
    sensor_write_register (0x37c5, 0x00);
    sensor_write_register (0x37c7, 0x39);
    sensor_write_register (0x37c8, 0x00);
    sensor_write_register (0x37d1, 0x13);
    sensor_write_register (0x3800, 0x00);
    sensor_write_register (0x3801, 0x00);
    sensor_write_register (0x3802, 0x00);
    sensor_write_register (0x3803, 0x04);
    sensor_write_register (0x3804, 0x05);
    sensor_write_register (0x3805, 0x0f);
    sensor_write_register (0x3806, 0x03);
    sensor_write_register (0x3807, 0xcb);
    sensor_write_register (0x3808, 0x05);
    sensor_write_register (0x3809, 0x00);
    sensor_write_register (0x380a, 0x03);
    sensor_write_register (0x380b, 0xc0);
    sensor_write_register (0x380c, 0x03);
    sensor_write_register (0x380d, 0x2a);
    sensor_write_register (0x380e, 0x03);
    sensor_write_register (0x380f, 0xdc);
    sensor_write_register (0x3810, 0x00);
    sensor_write_register (0x3811, 0x08);
    sensor_write_register (0x3812, 0x00);
    sensor_write_register (0x3813, 0x04);
    sensor_write_register (0x3814, 0x01);
    sensor_write_register (0x3815, 0x01);
    sensor_write_register (0x3816, 0x00);
    sensor_write_register (0x3817, 0x00);
    sensor_write_register (0x3818, 0x00);
    sensor_write_register (0x3819, 0x00);
    sensor_write_register (0x3820, 0x80);
    sensor_write_register (0x3821, 0x40);
    sensor_write_register (0x3826, 0x00);
    sensor_write_register (0x3827, 0x08);
    sensor_write_register (0x382a, 0x01);
    sensor_write_register (0x382b, 0x01);
    sensor_write_register (0x3836, 0x02);
    sensor_write_register (0x3838, 0x10);
    sensor_write_register (0x3861, 0x00);
    sensor_write_register (0x3862, 0x00);
    sensor_write_register (0x3863, 0x02);
    sensor_write_register (0x3b00, 0x00);
    sensor_write_register (0x3c00, 0x89);
    sensor_write_register (0x3c01, 0xab);
    sensor_write_register (0x3c02, 0x01);
    sensor_write_register (0x3c03, 0x00);
    sensor_write_register (0x3c04, 0x00);
    sensor_write_register (0x3c05, 0x03);
    sensor_write_register (0x3c06, 0x00);
    sensor_write_register (0x3c07, 0x05);
    sensor_write_register (0x3c0c, 0x00);
    sensor_write_register (0x3c0d, 0x00);
    sensor_write_register (0x3c0e, 0x00);
    sensor_write_register (0x3c0f, 0x00);
    sensor_write_register (0x3c40, 0x00);
    sensor_write_register (0x3c41, 0xa3);
    sensor_write_register (0x3c43, 0x7d);
    sensor_write_register (0x3c56, 0x80);
    sensor_write_register (0x3c80, 0x08);
    sensor_write_register (0x3c82, 0x01);
    sensor_write_register (0x3c83, 0x61);
    sensor_write_register (0x3d85, 0x17);
    sensor_write_register (0x3f08, 0x08);
    sensor_write_register (0x3f0a, 0x00);
    sensor_write_register (0x3f0b, 0x30);
    sensor_write_register (0x4000, 0xcd);
    sensor_write_register (0x4003, 0x40);
    sensor_write_register (0x4009, 0x0d);
    sensor_write_register (0x4010, 0xf0);
    sensor_write_register (0x4011, 0x70);
    sensor_write_register (0x4017, 0x10);
    sensor_write_register (0x4040, 0x00);
    sensor_write_register (0x4041, 0x00);
    sensor_write_register (0x4303, 0x00);
    sensor_write_register (0x4307, 0x30);
    sensor_write_register (0x4500, 0x30);
    sensor_write_register (0x4502, 0x40);
    sensor_write_register (0x4503, 0x06);
    sensor_write_register (0x4508, 0xaa);
    sensor_write_register (0x450b, 0x00);
    sensor_write_register (0x450c, 0x00);
    sensor_write_register (0x4600, 0x00);
    sensor_write_register (0x4601, 0x80);
    sensor_write_register (0x4700, 0x04);
    sensor_write_register (0x4704, 0x00);
    sensor_write_register (0x4705, 0x04);
    sensor_write_register (0x4837, 0x14);
    sensor_write_register (0x484a, 0x3f);
    sensor_write_register (0x5000, 0x10);
    sensor_write_register (0x5001, 0x01);
    sensor_write_register (0x5002, 0x28);
    sensor_write_register (0x5004, 0x0c);
    sensor_write_register (0x5006, 0x0c);
    sensor_write_register (0x5007, 0xe0);
    sensor_write_register (0x5008, 0x01);
    sensor_write_register (0x5009, 0xb0);
    sensor_write_register (0x502a, 0x18);
    sensor_write_register (0x5901, 0x00);
    sensor_write_register (0x5a01, 0x00);
    sensor_write_register (0x5a03, 0x00);
    sensor_write_register (0x5a04, 0x0c);
    sensor_write_register (0x5a05, 0xe0);
    sensor_write_register (0x5a06, 0x09);
    sensor_write_register (0x5a07, 0xb0);
    sensor_write_register (0x5a08, 0x06);
    sensor_write_register (0x5e00, 0x00);
    sensor_write_register (0x5e10, 0xfc);
    sensor_write_register (0x300f, 0x00);
    sensor_write_register (0x3733, 0x10);
    sensor_write_register (0x3610, 0xe8);
    sensor_write_register (0x3611, 0x56);
    sensor_write_register (0x3635, 0x14);
    sensor_write_register (0x3636, 0x13);
    sensor_write_register (0x3620, 0x84);
    sensor_write_register (0x3614, 0x96);
    sensor_write_register (0x481f, 0x30);
    sensor_write_register (0x3788, 0x00);
    sensor_write_register (0x3789, 0x04);
    sensor_write_register (0x378a, 0x01);
    sensor_write_register (0x378b, 0x60);
    sensor_write_register (0x3799, 0x27);
    sensor_write_register (0x0100, 0x01);

    bSensorInit = HI_TRUE;
    printf("====================================================================\n");
    printf("===ominivision ov9750 sensor 960P30fps(MIPI port) init success!=====\n");
    printf("====================================================================\n");
    
    return;
}


