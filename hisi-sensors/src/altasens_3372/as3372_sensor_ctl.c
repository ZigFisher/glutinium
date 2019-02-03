#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "hi_ssp.h"
#include "sensor_config_lin.h"

#define EXPOSURE_LONG_INDEX             (0)
#define EXPOSURE_SHORT_INDEX            (1)
#define VBLANK_FRONT_PORCH_OFFSET_INDEX (2)
#define COL_BUF_GAIN_EVEN_EAST_INDEX    (3)
#define COL_BUF_GAIN_EVEN_WEST_INDEX    (4)
#define COL_BUF_GAIN_ODD_EAST_INDEX     (5)
#define COL_BUF_GAIN_ODD_WEST_INDEX     (6)
#define DP_COARSE_INDEX                 (7)
#define DP_FINE_GAIN_FACTOR_E_INDEX     (8)
#define DP_FINE_GAIN_FACTOR_O_INDEX     (9)
#define DP_IN_OFFSET_E_INDEX            (10)
#define DP_IN_OFFSET_O_INDEX            (11)
#define DP_OUT_OFFSET_E_INDEX           (12)
#define DP_OUT_OFFSET_O_INDEX           (13)
#define DP_CORR_SMALL_PUSH_OFFSET_O_INDEX (14)

#define EXPOSURE_LONG_ADDR             (0x0802)
#define EXPOSURE_SHORT_ADDR            (0x0803)
#define VBLANK_FRONT_PORCH_OFFSET_ADDR (0x0815)
#define COL_BUF_GAIN_EVEN_EAST_ADDR    (0x0822)
#define COL_BUF_GAIN_EVEN_WEST_ADDR    (0x0823)
#define COL_BUF_GAIN_ODD_EAST_ADDR     (0x0824)
#define COL_BUF_GAIN_ODD_WEST_ADDR     (0x0825)
#define DP_COARSE_ADDR                 (0x0067)
#define DP_FINE_GAIN_FACTOR_E_ADDR     (0x0068)
#define DP_FINE_GAIN_FACTOR_O_ADDR     (0x0069)
#define DP_IN_OFFSET_E_ADDR            (0x0071)
#define DP_IN_OFFSET_O_ADDR            (0x0072)
#define DP_OUT_OFFSET_E_ADDR           (0x0071)
#define DP_OUT_OFFSET_O_ADDR           (0x0072)
#define DP_CORR_SMALL_PUSH_OFFSET_O_ADDR (0x006D)


int altasens_sensor_write_packet(unsigned int data)
{
	int fd = -1;
	int ret;
	unsigned int value;

	fd = open("/dev/ssp", 0);
	if(fd<0)
	{
		printf("Open /dev/ssp error!\n");
		return -1;
	}

	value = data;

	ret = ioctl(fd, SSP_WRITE_ALT, &value);

	//printf("ssp_write %#x, %#x\n", addr, value);

	close(fd);

	return 0;
}

int sensor_write_register(int addr, int data)
{
    return altasens_sensor_write_packet((unsigned int)((addr<<16) | (data & 0xFFFF)));
}

static void delay_ms(int ms) { }

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

void sensor_init()
{
    // program the sensor table
    unsigned int *ptr;
    for(ptr = (unsigned int *)sensor_rom_lin  ;  *ptr != 0xffffffff  ; ++ptr)
    {
        altasens_sensor_write_packet(*ptr);
    }
}

void setup_sensor(int isp_mode)
{
	if(0 == isp_mode) /* setup for ISP 'normal mode' */
	{
	}
	else if(1 == isp_mode) /* setup for ISP pixel calibration mode */
	{
		sensor_write_register(VBLANK_FRONT_PORCH_OFFSET_ADDR, 17908/2-1125+5);

		sensor_write_register(EXPOSURE_SHORT_ADDR, 2);

		sensor_write_register(EXPOSURE_LONG_ADDR, 2);

		sensor_write_register(COL_BUF_GAIN_EVEN_EAST_ADDR, 255);
		sensor_write_register(COL_BUF_GAIN_EVEN_WEST_ADDR, 255);
		sensor_write_register(COL_BUF_GAIN_ODD_EAST_ADDR, 255);
		sensor_write_register(COL_BUF_GAIN_ODD_WEST_ADDR, 255);

		sensor_write_register(0x062, 0x8000+92);
		sensor_write_register(0x071, 0x8000+92);
		sensor_write_register(0x072, 0x8000+92);
		sensor_write_register(0x075, 0x8000+92);

		sensor_write_register(0x076, 0x8000+64);
		sensor_write_register(0x077, 0x8000+64);

		sensor_write_register(0x06d, 2);
		sensor_write_register(0x066, 0x80b0);

		{
			unsigned short data16 = 0;
			// digital_gain_fine data - set bit15
			data16 = data16 | 0x8000;
			sensor_write_register(DP_FINE_GAIN_FACTOR_O_ADDR, data16);

			data16 = 0;
			// digital_gain_fine data - set bit15
			data16 = data16 | 0x8000;
			sensor_write_register(DP_FINE_GAIN_FACTOR_E_ADDR, data16);
		}

		{
			unsigned char data_lut = 4;

			unsigned short data16 = (data_lut << 4) | data_lut;
			// digital_gain_coarse data - set bit15
			// digital_gain_coarse data - set bit8
			data16 = data16 | 0x8100;
			sensor_write_register(DP_COARSE_ADDR, data16);
		}
	}
}
