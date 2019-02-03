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

const unsigned char sensor_i2c_addr	    =	0x60;		/* I2C Address of SC2135 */
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

	//printf("%#x, %#x, \n",data,sensor_read_register(addr));
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

void sensor_linear_1080p30_init();

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
	sensor_linear_1080p30_init();

	return ;
}

void sensor_exit()
{
	sensor_i2c_exit();
	flag_init = 0;
	return;
}

void sensor_linear_1080p30_init()
{
	//SC2135_27Minput_67.5MPclk_1080p_30fps_20160613
	sensor_write_register(0x0103,0x01);  // reset all regist
	sensor_write_register(0x0100,0x00);  // stream output ho

	sensor_write_register(0x3e03,0x03);  //AE AG setting
	sensor_write_register(0x3e01,0x46);
	sensor_write_register(0x3e08,0x00);
	sensor_write_register(0x3e09,0x10);
	sensor_write_register(0x3416,0x11);

	sensor_write_register(0x3300,0x20);  //sampling timing
	sensor_write_register(0x3301,0x08);
	sensor_write_register(0x3303,0x30);
	sensor_write_register(0x3306,0x78); //0524
	sensor_write_register(0x330b,0xd0);
	sensor_write_register(0x3309,0x30);
	sensor_write_register(0x3308,0x0a);
	sensor_write_register(0x331e,0x26);
	sensor_write_register(0x331f,0x26);
	sensor_write_register(0x3320,0x2c);
	sensor_write_register(0x3321,0x2c);
	sensor_write_register(0x3322,0x2c);
	sensor_write_register(0x3323,0x2c);
	sensor_write_register(0x330e,0x20);
	sensor_write_register(0x3f05,0xdf);  // mem write
	sensor_write_register(0x3f01,0x04);  //sram write inv
	sensor_write_register(0x3626,0x04);

	sensor_write_register(0x3312,0x06);  //sa1 timing
	sensor_write_register(0x3340,0x03);
	sensor_write_register(0x3341,0x68);
	sensor_write_register(0x3342,0x02);
	sensor_write_register(0x3343,0x20);

	sensor_write_register(0x3333,0x10);
	sensor_write_register(0x3334,0x20);

	sensor_write_register(0x3621,0x18); //mem timing setting
	sensor_write_register(0x3626,0x04); //reduce non-overlap

	sensor_write_register(0x3635,0x34); //analog config
	sensor_write_register(0x3038,0xa4);
	sensor_write_register(0x3630,0x84); //blksun  0xcc
	sensor_write_register(0x3622,0x0e);
	sensor_write_register(0x3620,0x62);
	sensor_write_register(0x3627,0x08);

	sensor_write_register(0x3637,0x87); //ramp config
	sensor_write_register(0x3638,0x86);
	sensor_write_register(0x3034,0xd2); //decrease counter c

	sensor_write_register(0x5780,0xff); //DPC manual mode
	sensor_write_register(0x5781,0x0c);
	sensor_write_register(0x5785,0x10);

	sensor_write_register(0x3d08,0x00); //PCLK inversion
	sensor_write_register(0x3640,0x00); //pad driving abilit

	sensor_write_register(0x320c,0x03); //30fps config
	sensor_write_register(0x320d,0xe8);

	sensor_write_register(0x3340,0x03); //barlow denoise
	sensor_write_register(0x3341,0x68);

	sensor_write_register(0x3662,0x82); //close temperature
	sensor_write_register(0x335d,0x00); //close precharge au
	sensor_write_register(0x4501,0xa4); //close bitwidth aut

	sensor_write_register(0x3333,0x00);
	sensor_write_register(0x3627,0x02);
	sensor_write_register(0x3620,0x62); //0x62 gain<2
	sensor_write_register(0x5781,0x04);

	sensor_write_register(0x3333,0x10);
	sensor_write_register(0x3306,0x69);
	sensor_write_register(0x3635,0x52);
	sensor_write_register(0x3636,0x7c);
	sensor_write_register(0x3631,0x84);
	sensor_write_register(0x330b,0xe0);

	sensor_write_register(0x3637,0x88);
	sensor_write_register(0x3306,0x6b);
	sensor_write_register(0x330b,0xd0);
	sensor_write_register(0x3630,0x84);

	sensor_write_register(0x0100,0x01); //stream output on

	bSensorInit = HI_TRUE;
	printf("=========================================================\n");
	printf("==sc2135 sensor 1080P30fps(Parallel port) init success!==\n");
	printf("=========================================================\n");

	return;
}

