#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <hi_math.h>

#include "hi_comm_video.h"

#ifdef HI_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "hi_i2c.h"
#endif

const unsigned char sensor_i2c_addr     =    0x60;        /* I2C Address of SC2235 */
const unsigned int  sensor_addr_byte    =    2;
const unsigned int  sensor_data_byte    =    1;
static int g_fd = -1;

extern WDR_MODE_E genSensorMode;
extern HI_U8 gu8SensorImageMode;
extern HI_BOOL bSensorInit;

//sensor fps mode
#define SC2235_SENSOR_1080P_30FPS_LINEAR_MODE  (0)

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

    ret = ioctl(g_fd, I2C_SLAVE_FORCE, (sensor_i2c_addr>>1));
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

    if (sensor_addr_byte == 2) {
    		buf[idx] = (addr >> 8) & 0xff;
    		idx++;
    		buf[idx] = addr & 0xff;
    		idx++;
    	} else {
    		buf[idx] = addr & 0xff;
    		idx++;
    	}

    	if (sensor_data_byte == 2) {
    		buf[idx] = (data >> 8) & 0xff;
    		idx++;
    		buf[idx] = data & 0xff;
    		idx++;
    	} else {
    		buf[idx] = data & 0xff;
    		idx++;
    	}

    ret = write(g_fd, buf, (sensor_addr_byte + sensor_data_byte));
    if(ret < 0)
    {
        printf("I2C_WRITE error!\n");
        return -1;
    }
#endif
    return 0;
}

static void delay_ms(int ms) { 
    hi_usleep(ms*1000);
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


void sensor_linear_1080p30_init(HI_VOID);


void sensor_init(HI_VOID)
{
    sensor_i2c_init();
    switch (gu8SensorImageMode)
    {
		case 0:
			sensor_linear_1080p30_init();
		break;
		default:
            printf("SC2235_SENSOR_CTL_Not support this mode\n");
            bSensorInit = HI_FALSE;
		break;
    }
	
    return ;
}

void sensor_exit(HI_VOID)
{
    sensor_i2c_exit();

    return;
}


void sensor_linear_1080p30_init(HI_VOID)
{
	sensor_write_register(0x0103,0x01);
	delay_ms(20);
	sensor_write_register(0x0100,0x00);
	sensor_write_register(0x3621,0x28);
	sensor_write_register(0x3309,0x60);
	sensor_write_register(0x331f,0x4d);
	sensor_write_register(0x3321,0x4f);
	sensor_write_register(0x33b5,0x10);
	sensor_write_register(0x3303,0x20);
	sensor_write_register(0x331e,0x0d);
	sensor_write_register(0x3320,0x0f);
	sensor_write_register(0x3622,0x02);
	sensor_write_register(0x3633,0x42);
	sensor_write_register(0x3634,0x42);
	sensor_write_register(0x3306,0x66);
	sensor_write_register(0x330b,0xd1);
	sensor_write_register(0x3301,0x0e);
	sensor_write_register(0x320c,0x08);
	sensor_write_register(0x320d,0x98);
	sensor_write_register(0x3364,0x05);
	sensor_write_register(0x363c,0x28);
	sensor_write_register(0x363b,0x0a);
	sensor_write_register(0x3635,0xa0);
	sensor_write_register(0x4500,0x59);
    sensor_write_register(0x3d08,0x00);
	sensor_write_register(0x3908,0x11);
	sensor_write_register(0x363c,0x08);
    sensor_write_register(0x3e03,0x03);
    sensor_write_register(0x3e01,0x46);
	sensor_write_register(0x3381,0x0a);
	sensor_write_register(0x3348,0x09);
	sensor_write_register(0x3349,0x50);
	sensor_write_register(0x334a,0x02);
	sensor_write_register(0x334b,0x60);
	sensor_write_register(0x3380,0x04);
	sensor_write_register(0x3340,0x06);
	sensor_write_register(0x3341,0x50);
	sensor_write_register(0x3342,0x02);
	sensor_write_register(0x3343,0x60);
	sensor_write_register(0x3632,0x88);
	sensor_write_register(0x3309,0xa0);
	sensor_write_register(0x331f,0x8d);
	sensor_write_register(0x3321,0x8f);
	sensor_write_register(0x335e,0x01);
	sensor_write_register(0x335f,0x03);
	sensor_write_register(0x337c,0x04);
	sensor_write_register(0x337d,0x06);
	sensor_write_register(0x33a0,0x05);
	sensor_write_register(0x3301,0x05);
	sensor_write_register(0x3670,0x08);
	sensor_write_register(0x367e,0x07);
	sensor_write_register(0x367f,0x0f);
	sensor_write_register(0x3677,0x2f);
	sensor_write_register(0x3678,0x22);
	sensor_write_register(0x3679,0x43);
	sensor_write_register(0x337f,0x03);
	sensor_write_register(0x3368,0x02);
	sensor_write_register(0x3369,0x00);
	sensor_write_register(0x336a,0x00);
	sensor_write_register(0x336b,0x00);
	sensor_write_register(0x3367,0x08);
	sensor_write_register(0x330e,0x30);
	sensor_write_register(0x3366,0x7c);
	sensor_write_register(0x3635,0xc1);
	sensor_write_register(0x363b,0x09);
	sensor_write_register(0x363c,0x07);
	sensor_write_register(0x391e,0x00);
	sensor_write_register(0x3637,0x14);
	sensor_write_register(0x3306,0x54);
	sensor_write_register(0x330b,0xd8);
	sensor_write_register(0x366e,0x08);
	sensor_write_register(0x366f,0x2f);
	sensor_write_register(0x3631,0x84);
	sensor_write_register(0x3630,0x48);
	sensor_write_register(0x3622,0x06);
	sensor_write_register(0x3638,0x1f);
	sensor_write_register(0x3625,0x02);
	sensor_write_register(0x3636,0x24);
	sensor_write_register(0x3348,0x08);
    sensor_write_register(0x3e03,0x0b);
	sensor_write_register(0x3342,0x03);
	sensor_write_register(0x3343,0xa0);
	sensor_write_register(0x334a,0x03);
	sensor_write_register(0x334b,0xa0);
	sensor_write_register(0x3343,0xb0);
	sensor_write_register(0x334b,0xb0);
	sensor_write_register(0x3802,0x01);
	sensor_write_register(0x3235,0x04);
	sensor_write_register(0x3236,0x63);
	sensor_write_register(0x3343,0xd0);
	sensor_write_register(0x334b,0xd0);
	sensor_write_register(0x3348,0x07);
	sensor_write_register(0x3349,0x80);
	sensor_write_register(0x391b,0x4d);
	sensor_write_register(0x3342,0x04);
	sensor_write_register(0x3343,0x20);
	sensor_write_register(0x334a,0x04);
	sensor_write_register(0x334b,0x20);
	sensor_write_register(0x3222,0x29);
	sensor_write_register(0x3901,0x02);
	sensor_write_register(0x3f00,0x07);
	sensor_write_register(0x3f04,0x08);
	sensor_write_register(0x3f05,0x74);
	sensor_write_register(0x330b,0xc8);
	sensor_write_register(0x3306,0x4a);
	sensor_write_register(0x330b,0xca);
	sensor_write_register(0x3639,0x09);
	sensor_write_register(0x5780,0xff);
	sensor_write_register(0x5781,0x04);
	sensor_write_register(0x5785,0x18);
	sensor_write_register(0x3313,0x05);
	sensor_write_register(0x3678,0x42);
	sensor_write_register(0x3237,0x05);
	sensor_write_register(0x3238,0x00);
	sensor_write_register(0x330b,0xc8);
	sensor_write_register(0x3306,0x40);
	sensor_write_register(0x3802,0x00);
	//init
	sensor_write_register(0x3e01,0x00);
	sensor_write_register(0x3e02,0x10);
	sensor_write_register(0x3e03,0x0b);
	sensor_write_register(0x3e07,0x00);
	sensor_write_register(0x3e08,0x03);
	sensor_write_register(0x3e09,0x10);
	//Platform_Dependent
	sensor_write_register(0x3d08,0x01);
	sensor_write_register(0x3641,0x01);
	delay_ms(20);
	sensor_write_register(0x0100,0x01);
    printf("===SC2235_1080P_30FPS_12BIT_LINE_Init_OK!===\n");
    bSensorInit = HI_TRUE;
    return;
}

