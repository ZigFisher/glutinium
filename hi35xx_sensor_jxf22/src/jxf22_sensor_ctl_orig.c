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

const unsigned char sensor_i2c_addr     =    0x80;  // 0x8c       /* I2C Address of MN34220 */
const unsigned int  sensor_addr_byte    =    1;
const unsigned int  sensor_data_byte    =    1;
static int g_fd = -1;

extern WDR_MODE_E genSensorMode;
extern HI_U8 gu8SensorImageMode;
extern HI_BOOL bSensorInit;

#define SENSOR_1080P_30FPS_MODE (1)

void sensor_linear_1080p30_init(HI_VOID);

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
    printf("Init dev gpioi2c_ex!\n");
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
    printf("Init dev i2c_0!\n");
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

void sensor_init(HI_VOID)
{
    sensor_i2c_init();
    
    /* When sensor first init, config all registers */
    if (HI_FALSE == bSensorInit) 
    {
        
        if (WDR_MODE_NONE == genSensorMode)
        {
            if(SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)   /* SENSOR_1080P_30FPS_MODE */
            {
                sensor_linear_1080p30_init();
				bSensorInit = HI_TRUE;
            }
            else
            {
            	printf("Not Support Image Mode %d\n", gu8SensorImageMode);
            }
        }
        else
		{
			printf("Not Support Sensor Mode %d\n", genSensorMode);
		}
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else 
    {
        if (WDR_MODE_NONE == genSensorMode)
        {
            if(SENSOR_1080P_30FPS_MODE == gu8SensorImageMode)   /* SENSOR_1080P_30FPS_MODE */
            {
                sensor_linear_1080p30_init();
            }
            else
            {
            	printf("Not Support Image Mode %d\n", gu8SensorImageMode);
            }
        }
		else
		{
			printf("Not Support Sensor Mode %d\n", genSensorMode);
		}
    }

    return ;
}

void sensor_exit(HI_VOID)
{
    sensor_i2c_exit();
    return;
}

/* linear 1080P30*/
void sensor_linear_1080p30_init(HI_VOID)
{										// MCLK 24   2400*1133	30FPS
	sensor_write_register(0x12,0x40);
	sensor_write_register(0x0E,0x11);
	sensor_write_register(0x0F,0x0C);
	sensor_write_register(0x10,0x44);
	sensor_write_register(0x11,0x80);
	sensor_write_register(0x5F,0x01);
	sensor_write_register(0x60,0x0A);
	sensor_write_register(0x19,0x20);
	sensor_write_register(0x48,0x0A);
	sensor_write_register(0x20,0xB0);
	sensor_write_register(0x21,0x04);
	sensor_write_register(0x22,0x6E);//0x6D); 1133-1134 taylor
	sensor_write_register(0x23,0x04);
	sensor_write_register(0x24,0xC0);
	sensor_write_register(0x25,0x38);
	sensor_write_register(0x26,0x43);
	sensor_write_register(0x27,0xC9);
	sensor_write_register(0x28,0x18);
	sensor_write_register(0x29,0x01);
	sensor_write_register(0x2A,0xC0);
	sensor_write_register(0x2B,0x21);
	sensor_write_register(0x2C,0x02);
	sensor_write_register(0x2D,0x00);
	sensor_write_register(0x2E,0x16);
	sensor_write_register(0x2F,0x44);
	sensor_write_register(0x41,0xD0);
	sensor_write_register(0x42,0x03);
	sensor_write_register(0x39,0x90);
	sensor_write_register(0x1D,0x00);
	sensor_write_register(0x1E,0x04);
	sensor_write_register(0x6C,0x40);
	sensor_write_register(0x70,0x89);
	sensor_write_register(0x71,0x4A);
	sensor_write_register(0x72,0x68);
	sensor_write_register(0x73,0x43);
	sensor_write_register(0x74,0x52);
	sensor_write_register(0x75,0x2B);
	sensor_write_register(0x76,0x60);
	sensor_write_register(0x77,0x09);
	sensor_write_register(0x78,0x1B);
	sensor_write_register(0x30,0x8C);
	sensor_write_register(0x31,0x0C);
	sensor_write_register(0x32,0xF0);
	sensor_write_register(0x33,0x0C);
	sensor_write_register(0x34,0x1F);
	sensor_write_register(0x35,0xE3);
	sensor_write_register(0x36,0x0E);
	sensor_write_register(0x37,0x34);
	sensor_write_register(0x38,0x13);
	sensor_write_register(0x3A,0x08);
	sensor_write_register(0x3B,0x30);
	sensor_write_register(0x3C,0xC0);
	sensor_write_register(0x3D,0x00);
	sensor_write_register(0x3E,0x00);
	sensor_write_register(0x3F,0x00);
	sensor_write_register(0x40,0x00);
	sensor_write_register(0x6F,0x03);
	sensor_write_register(0x0D,0x64);
	sensor_write_register(0x56,0x32);
	sensor_write_register(0x5A,0x20);
	sensor_write_register(0x5B,0xB3);
	sensor_write_register(0x5C,0xF7);
	sensor_write_register(0x5D,0xF0);
	sensor_write_register(0x62,0x80);
	sensor_write_register(0x63,0x80);
	sensor_write_register(0x64,0x00);
	sensor_write_register(0x67,0x75);
	sensor_write_register(0x68,0x00);
	sensor_write_register(0x6A,0x4D);
	sensor_write_register(0x8F,0x18);
	sensor_write_register(0x91,0x04);
	sensor_write_register(0x0C,0x00);
	sensor_write_register(0x59,0x97);
	sensor_write_register(0x4A,0x05);
	sensor_write_register(0x49,0x10);
	sensor_write_register(0x50,0x02);
	sensor_write_register(0x47,0x22);
	sensor_write_register(0x7E,0xCD);
	sensor_write_register(0x7F,0x52);
	sensor_write_register(0x7B,0x57);
	sensor_write_register(0x7C,0x28);
	sensor_write_register(0x80,0x00);
	sensor_write_register(0x13,0x81);
	sensor_write_register(0x74,0x53);
	sensor_write_register(0x12,0x00);
	sensor_write_register(0x74,0x52);
	sensor_write_register(0x93,0x5C);
	sensor_write_register(0x45,0x89);
	delay_ms(500);
	sensor_write_register(0x45,0x09);
	sensor_write_register(0x1F,0x01);

    printf("=======sensor_linear_init OK!=======\n");

	return;
}
