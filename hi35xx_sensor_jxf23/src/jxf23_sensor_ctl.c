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

const unsigned char sensor_i2c_addr  = 0x80;/* I2C Address of jxf23 */
const unsigned int  sensor_addr_byte = 1;
const unsigned int  sensor_data_byte = 1;
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
void sensor_linear_720p30_init();
void sensor_linear_960p30_init();
void sensor_linear_1080p30_init();

void sensor_init()
{
    sensor_i2c_init();

	if (1 == gu8SensorImageMode)/* SENSOR_720P_30FPS_MODE */
	{
	    printf("i don't have sensor_linear_720p30_init\n");
		// sensor_linear_720p30_init();
	}
	else if(2 == gu8SensorImageMode)//960P
	{
        printf("i don't have sensor_linear_960p30_init\n");
		// sensor_linear_960p30_init();
	}
	else if (3 == gu8SensorImageMode)/* SENSOR_1080P_60FPS_MODE */
	{
		sensor_linear_1080p30_init();
	}
	else
	{
		printf("Not support this mode\n");
	}

    return ;
}

void sensor_exit()
{
    sensor_i2c_exit();

    return;
}

void sensor_linear_1080p30_init()
{  //F23_049_20180403
	sensor_write_register(0x0E,0x11 );
	sensor_write_register(0x0F,0x14 );
	sensor_write_register(0x10,0x40 );
	sensor_write_register(0x11,0x80 );
	sensor_write_register(0x48,0x05 );
	sensor_write_register(0x96,0xAA );
	sensor_write_register(0x94,0xC0 );
	sensor_write_register(0x97,0x8D );
	sensor_write_register(0x96,0x00 );
	sensor_write_register(0x12,0x40 );
	sensor_write_register(0x0E,0x11 );
	sensor_write_register(0x0F,0x14 );
	sensor_write_register(0x10,0x48 );
	sensor_write_register(0x11,0x80 );
	sensor_write_register(0x0D,0x50 );
	sensor_write_register(0x5F,0x41 );
	sensor_write_register(0x60,0x20 );
	sensor_write_register(0x58,0x12 );
	sensor_write_register(0x57,0x60 );
	sensor_write_register(0x9D,0x00 );
	sensor_write_register(0x20,0x00 );
	sensor_write_register(0x21,0x05 );
	sensor_write_register(0x22,0x65 );
	sensor_write_register(0x23,0x04 );
	sensor_write_register(0x24,0xC0 );
	sensor_write_register(0x25,0x38 );
	sensor_write_register(0x26,0x43 );
	sensor_write_register(0x27,0xC3 );
	sensor_write_register(0x28,0x19 );
	sensor_write_register(0x29,0x04 );
	sensor_write_register(0x2C,0x00 );
	sensor_write_register(0x2D,0x00 );
	sensor_write_register(0x2E,0x18 );
	sensor_write_register(0x2F,0x44 );
	sensor_write_register(0x41,0xC9 );
	sensor_write_register(0x42,0x13 );
	sensor_write_register(0x46,0x00 );
	sensor_write_register(0x76,0x60 );
	sensor_write_register(0x77,0x09 );
	sensor_write_register(0x1D,0xFF );
	sensor_write_register(0x1E,0x9F );
	sensor_write_register(0x6C,0xC0 );
	sensor_write_register(0x2A,0xB1 );
	sensor_write_register(0x2B,0x24 );
	sensor_write_register(0x31,0x08 );
	sensor_write_register(0x32,0x4F );
	sensor_write_register(0x33,0x20 );
	sensor_write_register(0x34,0x5E );
	sensor_write_register(0x35,0x5E );
	sensor_write_register(0x3A,0xAF );
	sensor_write_register(0x56,0x32 );
	sensor_write_register(0x59,0xBF );
	sensor_write_register(0x5A,0x04 );
	sensor_write_register(0x85,0x5A );
	sensor_write_register(0x8A,0x04 );
	sensor_write_register(0x8F,0x90 );
	sensor_write_register(0x91,0x13 );
	sensor_write_register(0x5B,0xA0 );
	sensor_write_register(0x5C,0xF0 );
	sensor_write_register(0x5D,0xF4 );
	sensor_write_register(0x5E,0x1F );
	sensor_write_register(0x62,0x04 );
	sensor_write_register(0x63,0x0F );
	sensor_write_register(0x64,0xC0 );
	sensor_write_register(0x66,0x44 );
	sensor_write_register(0x67,0x73 );
	sensor_write_register(0x69,0x7C );
	sensor_write_register(0x6A,0x28 );
	sensor_write_register(0x7A,0xC0 );
	sensor_write_register(0x4A,0x05 );
	sensor_write_register(0x7E,0xCD );
	sensor_write_register(0x49,0x10 );
	sensor_write_register(0x50,0x02 );
	sensor_write_register(0x7B,0x4A );
	sensor_write_register(0x7C,0x0C );
	sensor_write_register(0x7F,0x57 );
	sensor_write_register(0x90,0x00 );
	sensor_write_register(0x8E,0x00 );
	sensor_write_register(0x8C,0xFF );
	sensor_write_register(0x8D,0xC7 );
	sensor_write_register(0x8B,0x01 );
	sensor_write_register(0x0C,0x40 );
	sensor_write_register(0x65,0x02 );
	sensor_write_register(0x80,0x1A );
	sensor_write_register(0x81,0xC0 );
	sensor_write_register(0x19,0x20 );
	sensor_write_register(0x12,0x00 );
	sensor_write_register(0x48,0x85 );
	sensor_write_register(0x48,0x05 );

    printf("===soi_f23 sensor DVP 1080P30fps linear mode init success!=====\n");

    bSensorInit = HI_TRUE;

    return;
}
