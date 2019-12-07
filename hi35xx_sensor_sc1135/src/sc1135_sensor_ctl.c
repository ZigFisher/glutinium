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

const unsigned char sensor_i2c_addr	    =	0x60;		/* I2C Address of SC1135 */
const unsigned int  sensor_addr_byte	=	2;
const unsigned int  sensor_data_byte	=	1;
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
	int idx = 0;
	int ret = 0, data = 0;
	char buf[8] = {0};
	buf[idx++] = addr & 0xFF;

	if (sensor_addr_byte == 2)
	{
		ret = ioctl(g_fd, I2C_16BIT_REG, 1);
		buf[idx++] = (addr >> 8);
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
	ret = read(g_fd, buf, idx);

	return buf[0];
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

#define SENSOR_1080P_30FPS_MODE  (1)

void sensor_init()
{
	sensor_i2c_init();
#if 0
	/* When sensor first init, config all registers */
	if (HI_FALSE == bSensorInit)
	{
		if(SENSOR_720P_25FPS_MODE == gu8SensorImageMode)
		{
			sensor_linear_720p25_init();
		}
	}
	/* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
	else
	{
		if(SENSOR_720P_25FPS_MODE == gu8SensorImageMode)
		{
			sensor_linear_720p25_init();
		}
	}
#endif
	sensor_linear_720p30_init();

	return ;
}

void sensor_exit()
{
	sensor_i2c_exit();
	flag_init = 0;
	return;
}

void sensor_linear_720p30_init()
{
	sensor_write_register(0x3000,0x01);//manualstreamenbale
	sensor_write_register(0x3003,0x01);//softreset
	sensor_write_register(0x3400,0x53);
	sensor_write_register(0x3416,0xc0);
	sensor_write_register(0x3d08,0x00);
	sensor_write_register(0x3e03,0x03);
	sensor_write_register(0x3928,0x00);
	sensor_write_register(0x3630,0x58);
	sensor_write_register(0x3612,0x00);
	sensor_write_register(0x3632,0x41);
	sensor_write_register(0x3635,0x00); //20160328
	sensor_write_register(0x3620,0x44);
	sensor_write_register(0x3633,0x7f); //20160422
	sensor_write_register(0x3780,0x0b);
	sensor_write_register(0x3300,0x33);
	sensor_write_register(0x3301,0x38);
	sensor_write_register(0x3302,0x30);
	sensor_write_register(0x3303,0x80); //20160307B  20160412
	sensor_write_register(0x3304,0x18);
	sensor_write_register(0x3305,0x72);
	sensor_write_register(0x331e,0x50); //20160512
	sensor_write_register(0x321e,0x00);
	sensor_write_register(0x321f,0x0a);
	sensor_write_register(0x3216,0x0a);
	sensor_write_register(0x3332,0x38);
	sensor_write_register(0x5054,0x82);
	sensor_write_register(0x3622,0x26);
	sensor_write_register(0x3907,0x02);
	sensor_write_register(0x3908,0x00);
	sensor_write_register(0x3601,0x1a); //20160422
	sensor_write_register(0x3315,0x44);
	sensor_write_register(0x3308,0x40);
	sensor_write_register(0x3223,0x22);//vysncmode[5]
	sensor_write_register(0x3e0e,0x50);
	/*DPC*/
	sensor_write_register(0x3211,0x60);
	sensor_write_register(0x5780,0xff);
	sensor_write_register(0x5781,0x04); //20160328
	sensor_write_register(0x5785,0x0c); //20160328
	sensor_write_register(0x5000,0x66);

	sensor_write_register(0x3e0f,0x90);
	sensor_write_register(0x3631,0x80);
	sensor_write_register(0x3310,0x83);
	sensor_write_register(0x3336,0x01);
	sensor_write_register(0x3337,0x00);
	sensor_write_register(0x3338,0x03);
	sensor_write_register(0x3339,0xe8);
	sensor_write_register(0x3335,0x06); //20160418
	sensor_write_register(0x3880,0x00);

	//power reduction 20160606
	sensor_write_register(0x3620,0x42);
	sensor_write_register(0x3610,0x03);
	sensor_write_register(0x3600,0x64);
	sensor_write_register(0x3636,0x0d);
	sensor_write_register(0x3323,0x80);
	//io strength
	sensor_write_register(0x3640,0x03);

#if 1
	//960 p
	/*27Minput54Moutputpixelclockfrequency*/
	sensor_write_register(0x3010,0x07);
	sensor_write_register(0x3011,0x46);
	sensor_write_register(0x3004,0x04);
	//  sensor_write_register(0x3610,0x2b);  //20160606

	/*configFramelengthandwidth*/
	sensor_write_register(0x320c,0x07); //1800
	sensor_write_register(0x320d,0x08); //1000
	sensor_write_register(0x320e,0x03);
	sensor_write_register(0x320f,0xe8);

	/*configOutputwindowposition*/
	sensor_write_register(0x3210,0x00);
	sensor_write_register(0x3211,0x60);
	sensor_write_register(0x3212,0x00);
	sensor_write_register(0x3213,0x04); //for BGGR out format 20160412

	/*configOutputimagesize*/
	sensor_write_register(0x3208,0x05);
	sensor_write_register(0x3209,0x00);
	sensor_write_register(0x320a,0x03);
	sensor_write_register(0x320b,0xc0);

	/*configFramestartphysicalposition*/
	sensor_write_register(0x3202,0x00);
	sensor_write_register(0x3203,0x08);
	sensor_write_register(0x3206,0x03);
	sensor_write_register(0x3207,0xcf);

	/*powerconsumptionreduction*/
	sensor_write_register(0x3330,0x0d);
	sensor_write_register(0x3320,0x06);
	sensor_write_register(0x3321,0xd8);
	sensor_write_register(0x3322,0x01);
	//  sensor_write_register(0x3323,0x80); //20160606
	//  sensor_write_register(0x3600,0x54); //20160606

	printf("SC1135 960p 30fps sensor init OK!\n");

#else

	//720p
	/*27Minput54Moutputpixelclockfrequency*/
	sensor_write_register(0x3010,0x07);
	sensor_write_register(0x3011,0x46);
	sensor_write_register(0x3004,0x04);
	//  sensor_write_register(0x3610,0x2b); //201600606

	/*configFramelengthandwidth*/
	sensor_write_register(0x320c,0x07); //1800
	sensor_write_register(0x320d,0x08); //1000
	sensor_write_register(0x320e,0x03);
	sensor_write_register(0x320f,0xe8);

	/*configOutputwindowposition*/
	sensor_write_register(0x3210,0x00);
	sensor_write_register(0x3211,0x60);
	sensor_write_register(0x3212,0x00);
	sensor_write_register(0x3213,0x80);  //for BGGR out format 20160412

	/*configOutputimagesize*/
	sensor_write_register(0x3208,0x05);
	sensor_write_register(0x3209,0x00);
	sensor_write_register(0x320a,0x02);
	sensor_write_register(0x320b,0xd0);

	/*configFramestartphysicalposition*/
	sensor_write_register(0x3202,0x00);
	sensor_write_register(0x3203,0x08);
	sensor_write_register(0x3206,0x03);
	sensor_write_register(0x3207,0xcf);

	/*powerconsumptionreduction*/
	sensor_write_register(0x3330,0x0d);
	sensor_write_register(0x3320,0x06);
	sensor_write_register(0x3321,0xd8);
	sensor_write_register(0x3322,0x01);
	// sensor_write_register(0x3323,0x80); //20160606
	// sensor_write_register(0x3600,0x54); //20160606

	printf("SC1135 720p 30fps sensor init OK!~~~~~~~~~~~~~~~~\n");
#endif

	bSensorInit = HI_TRUE;
	printf("=========================================================\n");
	printf("============== SC1135 sensor init success! ==============\n");
	printf("=========================================================\n");

	return;
}

