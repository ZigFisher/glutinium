#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

/* /dev/i2c-X ioctl commands.  The ioctl's parameter is always an
 * unsigned long, except for:
 *      - I2C_FUNCS, takes pointer to an unsigned long
 *      - I2C_RDWR, takes pointer to struct i2c_rdwr_ioctl_data
 *      - I2C_SMBUS, takes pointer to struct i2c_smbus_ioctl_data
 */
#define I2C_RETRIES     0x0701  /* number of times a device address should
				   be polled when not acknowledging */
#define I2C_TIMEOUT     0x0702  /* set timeout in units of 10 ms */
/* NOTE: Slave address is 7 or 10 bits, but 10-bit addresses
 * are NOT supported! (due to code brokenness)
 */
#define I2C_SLAVE       0x0703  /* Use this slave address */
#define I2C_SLAVE_FORCE 0x0706  /* Use this slave address, even if it
				   is already in use by a driver! */
#define I2C_TENBIT      0x0704  /* 0 for 7 bit addrs, != 0 for 10 bit */

#define I2C_FUNCS       0x0705  /* Get the adapter functionality mask */

#define I2C_RDWR        0x0707  /* Combined R/W transfer (one STOP only) */

#define I2C_PEC         0x0708  /* != 0 to use PEC with SMBus */
#define I2C_SMBUS       0x0720  /* SMBus transfer */
#define I2C_16BIT_REG   0x0709  /* 16BIT REG WIDTH */
#define I2C_16BIT_DATA  0x070a  /* 16BIT DATA WIDTH */

#define READ_MIN_CNT 4
#define WRITE_MIN_CNT 5

void print_w_usage(void)
{
	printf("Usage: \n");
	printf("i2c_write <i2c_num> <device_addr> <reg_addr> <value> \\\n"
		"\t<reg_width> <data_width>.\n");
	printf("\nFor example:\n");
	printf("\ti2c_write 0x1 0x56 0x0 0x28 2 2. \n");
	printf("\ti2c_write 0x1 0x56 0x0 0x28. default reg_width and data_width is 1. \n");
}

int main(int argc , char* argv[])
{
	int fd = -1;
	char file_name[0x20];
	int ret =0;
	unsigned int tmp, i, index = 0;
	unsigned int i2c_num, device_addr, reg_addr, reg_value;
	unsigned int reg_width  = 1;
	unsigned int data_width = 1;
	char buf[4];

	if (argc < WRITE_MIN_CNT) {
		print_w_usage();
		return -1;
	}

	for (i = 1; i < argc; i++) {
		char *end;
		tmp = strtol(argv[i], &end, 0);
		if (*end) {
			print_w_usage();
			return -1;
		}

		switch (i) {
		case 1: i2c_num = tmp;
			break;
		case 2: device_addr = tmp;
			break;
		case 3: reg_addr = tmp;
			break;
		case 4: reg_value = tmp;
			break;
		case 5: reg_width = tmp;
			break;
		case 6: data_width = tmp;
			break;
		default: break;
		}
	}

	sprintf(file_name, "/dev/i2c-%u", i2c_num);
	fd = open(file_name, O_RDWR);
	if (fd<0) {
		fprintf(stderr, "Open %s error!\n", file_name);
		return -1;
	}

	ret = ioctl(fd, I2C_SLAVE_FORCE, device_addr);
	if (ret < 0) {
		fprintf(stderr, "CMD_SET_DEV error!\n");
		close(fd);
		return -1;
	}

	if (reg_width == 2)
		ret = ioctl(fd, I2C_16BIT_REG, 1);
	else
		ret = ioctl(fd, I2C_16BIT_REG, 0);

	if (ret < 0) {
		fprintf(stderr, "CMD_SET_REG_WIDTH error!\n");
		close(fd);
		return -1;
	}

	if (data_width == 2)
		ret = ioctl(fd, I2C_16BIT_DATA, 1);
	else
		ret = ioctl(fd, I2C_16BIT_DATA, 0);

	if (ret < 0) {
		fprintf(stderr, "CMD_SET_DATA_WIDTH error!\n");
		close(fd);
		return -1;
	}

	if (reg_width == 2) {
		buf[index] = reg_addr & 0xff;
		index++;
		buf[index] = (reg_addr >> 8) & 0xff;
		index++;
	} else {
		buf[index] = reg_addr & 0xff;
		index++;
	}

	if (data_width == 2) {
		buf[index] = reg_value & 0xff;
		index++;
		buf[index] = (reg_value >> 8) & 0xff;
		index++;
	} else {
		buf[index] = reg_value & 0xff;
		index++;
	}

	write(fd, buf, (reg_width + data_width));
	if(ret < 0)
	{
		fprintf(stderr, "I2C_WRITE error!\n");
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}

