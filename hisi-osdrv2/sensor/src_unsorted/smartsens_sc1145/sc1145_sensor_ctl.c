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

const unsigned char sensor_i2c_addr	=	0x60;		/* I2C Address of SC1145 */
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
	if(ret < 0)
	{
		printf("I2C_READ error!\n");
		return -1;
	}
	//else
	//    printf("...............addr.%#x...buf.%#x....\n",addr,buf[0]);

	return buf[0];
}


int sensor_write_register(int addr, int data)
{
	int i = 0;

	for (i = 0; i < 2; i ++)
	{
		if(flag_init == 0)
		{

			sensor_i2c_init();
			flag_init = 1;
		}

		int idx = 0;
		int ret;
		char buf[8] = {0};


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

	}
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
	sensor_write_register(0x3003,0x01);//soft reset
	sensor_write_register(0x3000,0x00);//pause for reg writing
	sensor_write_register(0x3010,0x01);//output format 43.2M 1920X750 30fps
	sensor_write_register(0x3011,0xc6);
	sensor_write_register(0x3004,0x45);
	sensor_write_register(0x3e03,0x03);//exp and gain
	sensor_write_register(0x3600,0x94);//0607 reduce power
	sensor_write_register(0x3610,0x03);
	sensor_write_register(0x3634,0x00);// reduce power
	sensor_write_register(0x3620,0x84);
	sensor_write_register(0x3631,0x85);// txvdd 0910
	sensor_write_register(0x3907,0x03);
	sensor_write_register(0x3622,0x0e);
	sensor_write_register(0x3633,0x2c);//0825
	sensor_write_register(0x3630,0x88);
	sensor_write_register(0x3635,0x80);
	sensor_write_register(0x3310,0X83);//prechg tx auto ctrl [5] 0825
	sensor_write_register(0x3336,0x00);
	sensor_write_register(0x3337,0x00);
	sensor_write_register(0x3338,0x02);
	sensor_write_register(0x3339,0xee);
	sensor_write_register(0x331E,0xa0);//
	sensor_write_register(0x3335,0x10);
	sensor_write_register(0x3315,0X44);
	sensor_write_register(0x3308,0X40);
	sensor_write_register(0x3330,0x0d);// sal_en timing,cancel the fpn in low light
	sensor_write_register(0x3320,0x05);//0825
	sensor_write_register(0x3321,0x60);
	sensor_write_register(0x3322,0x02);
	sensor_write_register(0x3323,0xc0);
	sensor_write_register(0x3303,0xa0);
	sensor_write_register(0x3304,0x60);
	sensor_write_register(0x3d04,0x04);//0806    vsync gen mode
	sensor_write_register(0x3d08,0x03);
	sensor_write_register(0x320e,0x02);//barlow
	sensor_write_register(0x320f,0xee);//barlow

	sensor_write_register(0x3781,0x10);
	sensor_write_register(0x3211,0x60);
	sensor_write_register(0x321c,0x00);
	sensor_write_register(0x321d,0x00);
	// io strength
	sensor_write_register(0x3640,0x03);

	sensor_write_register(0x3000,0x01);//recover

	bSensorInit = HI_TRUE;
	printf("=========================================================\n");
	printf("==SC1145 sensor 720P30fps(Parallel port) init success! ==\n");
	printf("=========================================================\n");

	return;
}


