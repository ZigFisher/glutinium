#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <memory.h>
#include <sys/ioctl.h>

#include "hi_spi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define READ_MIN_CNT 5
#define WRITE_MIN_CNT 6

static int atoul(char *str, int * pulValue)
{
	int ulResult=0;

	while (*str)
	{
		if (isdigit((int)*str))
		{
			if ((ulResult<429496729) || ((ulResult==429496729) && (*str<'6')))
			{
				ulResult = ulResult*10 + (*str)-48;
			}
			else
			{
				*pulValue = ulResult;
				return -1;
			}
		}
		else
		{
			*pulValue=ulResult;
			return -1;
		}
		str++;
	}
	*pulValue=ulResult;
	return 0;
}

#define ASC2NUM(ch) (ch - '0')
#define HEXASC2NUM(ch) (ch - 'A' + 10)

static int  atoulx(char *str, int * pulValue)
{
	int   ulResult=0;
	char ch;

	while (*str)
	{
		ch=toupper(*str);
		if (isdigit(ch) || ((ch >= 'A') && (ch <= 'F' )))
		{
			if (ulResult < 0x10000000)
			{
				ulResult = (ulResult << 4) + ((ch<='9')?(ASC2NUM(ch)):(HEXASC2NUM(ch)));
			}
			else
			{
				*pulValue=ulResult;
				return -1;
			}
		}
		else
		{
			*pulValue=ulResult;
			return -1;
		}
		str++;
	}

	*pulValue=ulResult;
	return 0;
}

static int  StrToNumber(char *str , int * pulValue)
{
	if ( *str == '0' && (*(str+1) == 'x' || *(str+1) == 'X') )
	{
		if (*(str+2) == '\0')
		{
			return -1;
		}
		else
		{
			return atoulx(str+2,pulValue);
		}
	}
	else
	{
		return atoul(str,pulValue);
	}
}

static void print_r_usage(void)
{
	printf("Usage: ssp_read <spi_num> <csn> <dev_addr> <reg_addr> [num_reg] [dev_width] [reg_width] [data_width] [reg_order] [data_order] .\n");
	printf("\tnum_reg and dev_width and reg_width and data_width and reg_order and data_order can be omitted, the default is 0x1.\n");
	printf("eg:\n");
	printf("\tssp_read 0x0 0x0 0x2 0x0 0x10 0x1 0x1 0x1 0x1 0x1.\n");
	printf("\tssp_read 0x0 0x0 0x2 0x0. default num_reg and dev_width and reg_width and data_width and reg_order and data_order is 0x1.\n");
	return;
}

static void print_w_usage(void)
{
	printf("Usage: ssp_write <spi_num> <csn> <dev_addr> <reg_addr> <data> [dev_width] [reg_width] [data_width] [reg_order] [data_order].\n");
	printf("\tdev_width and reg_width and data_width and reg_order and data_order can be omitted, the default is 0x1.\n");
	printf("eg:\n");
	printf("\tssp_write 0x0 0x0 0x2 0x0 0x65 0x1 0x1 0x1 0x1 0x1.\n");
	printf("\tssp_write 0x0 0x0 0x2 0x0 0x65. default dev_width and reg_width and data_width and reg_order and data_orderis 0x1.\n");
	return;
}

void reverse8(unsigned char *buf, unsigned int len)
{
	unsigned int i;

	for (i = 0; i < len; i++) {
		buf[i] = (buf[i] & 0x55) << 1 | (buf[i] & 0xAA) >> 1;
		buf[i] = (buf[i] & 0x33) << 2 | (buf[i] & 0xCC) >> 2;
		buf[i] = (buf[i] & 0x0F) << 4 | (buf[i] & 0xF0) >> 4;
	}
}

int ssp_read(int argc, char* argv[])
{
	int retval = 0;
	int i = 0, index = 0;
	int tmp = 0;
	int fd = -1;
	char file_name[0x20];
	unsigned char  buf[0x10];
	struct spi_ioc_transfer mesg[1];
	unsigned int spi_num, csn, dev_addr, reg_addr, cur_reg_addr;
	unsigned int num_reg = 1, dev_width = 1, reg_width = 1, data_width = 1, reg_order = 1, data_order = 1;

	if (argc < READ_MIN_CNT) {
		print_r_usage();
		retval = -1;
		goto end0;
	}

	for (i = 1; i < argc; i++) {
		if (StrToNumber(argv[i], &tmp) != 0) {
			print_r_usage();
			retval = -1;
			goto end0;
		}
		
		switch (i) {
			case 1:
				spi_num = tmp;
				break;
			case 2:
				csn = tmp;
				break;
			case 3:
				dev_addr = tmp;
				break;
			case 4:
				reg_addr = tmp;
				break;
			case 5:
				num_reg = tmp;
				break;
			case 6:
				dev_width = tmp;
				if ((dev_width != 1) && (dev_width != 2)) {
					printf("dev_width must be 1 or 2\n");
					print_r_usage();
					retval = -1;
					goto end0;
				}
				break;
			case 7:
				reg_width = tmp;
				if ((reg_width != 1) && (reg_width != 2)) {
					printf("reg_width must be 1 or 2\n");
					print_r_usage();
					retval = -1;
					goto end0;
				}
				break;
			case 8:
				data_width = tmp;
				if ((data_width != 1) && (data_width != 2)) {
					printf("data_width must be 1 or 2\n");
					print_r_usage();
					retval = -1;
					goto end0;
				}
				break;
			case 9:
				reg_order = tmp;
				if ((reg_order != 1) && (reg_order != 0))
				{
					printf("reg_order must be 1 or 0\n");
					print_w_usage();
					retval = -1;
					goto end0;
				}
				break;
			case 10:
				data_order = tmp;
				if ((data_order != 1) && (data_order != 0))
				{
					printf("data_order must be 1 or 0\n");
					print_w_usage();
					retval = -1;
					goto end0;
				}
				break;
			default:
				break;
		}
	}
	
	printf("spi_num:%u, csn:%u\n"
			"dev_addr:0x%04x, reg_addr:0x%04x, num_reg:%d, "
			"dev_width:%d, reg_width:%d, data_width:%d, reg_order: %d, data_order: %d\n",
			spi_num, csn, dev_addr, reg_addr, num_reg,
			dev_width, reg_width, data_width, reg_order, data_order);

	sprintf(file_name, "/dev/spidev%u.%u", spi_num, csn);

	fd = open(file_name, 0);
	if (fd < 0) {
		printf("Open %s error!\n",file_name);
		retval = -1;
		goto end0;
	}

	tmp = SPI_MODE_3;
	retval = ioctl(fd, SPI_IOC_WR_MODE, &tmp);
	if (retval) {
		printf("set spi mode fail!\n");
		retval = -1;
		goto end1;
	}

	memset(mesg, 0, sizeof mesg);

	mesg[0].tx_buf = (__u32)buf;
	mesg[0].rx_buf = (__u32)buf;
	mesg[0].len = dev_width + reg_width + data_width;
	mesg[0].speed_hz = 2000000;
	mesg[0].bits_per_word = 8;
	mesg[0].cs_change = 1;

	memset(buf, 0, sizeof buf);

	printf("====reg_addr:0x%04x====", reg_addr);

	for (cur_reg_addr = reg_addr, i = 0; cur_reg_addr < reg_addr + num_reg; cur_reg_addr++, i++) {
		index = 0;

		if(dev_width == 1) {
			*(__u8 *)(&buf[index]) = dev_addr & 0xff;
			index++;
		} else {
			*(__u16 *)(&buf[index]) = dev_addr & 0xffff;
			index += 2;
		}

		if(reg_width == 1) {
			*(__u8 *)(&buf[index]) = cur_reg_addr & 0xff;
			index++;
		} else {
			if (reg_order)
				*(__u16 *)(&buf[index]) = cur_reg_addr & 0xffff;
			else {
				*(__u8*)(&buf[index]) = (cur_reg_addr >> 8)  & 0xff;
				*(__u8*)(&buf[index + 1]) = cur_reg_addr & 0xff;
			}			
			index += 2;
		}

		if(data_width == 1) {
			*(__u8 *)(&buf[index]) = 0x00;
		} else {
			*(__u16 *)(&buf[index]) = 0x0000;
		}

		reverse8(buf, mesg[0].len);

		retval = ioctl(fd, SPI_IOC_MESSAGE(1), mesg);
		if (retval  != mesg[0].len) {  
			printf("SPI_IOC_MESSAGE error \n");  
			retval = -1;
			goto end1;
		}

		retval = 0;
		if(data_width == 1)
			tmp = *(__u8 *)(&buf[index]);
		else {
			if (data_order)
				tmp = *(__u16 *)(&buf[index]);
			else {
				tmp = *(__u8*)(&buf[index]) << 8;
				tmp += (*(__u8*)(&buf[index + 1]));
			}
		}

		reverse8(&tmp, 1);

		if ((i % 0x10) == 0) {
			printf("\n0x%04x:  ", i);
		}
		printf("0x%04x ", tmp);
	}

	printf("\n[END]\n");

end1:
	close(fd);

end0:
	return retval;
}

int ssp_write(int argc, char* argv[])
{
	int retval = 0;
	int i = 0, index = 0;
	int tmp = 0;
	int fd = -1;
	char file_name[0x20];
	unsigned char  buf[0x10];
	struct spi_ioc_transfer mesg[1];
	unsigned int spi_num, csn, dev_addr, reg_addr, data;
	unsigned int dev_width = 1, reg_width = 1, data_width = 1, reg_order = 1, data_order = 1;

	if (argc < WRITE_MIN_CNT) {
		print_w_usage();
		retval = -1;
		goto end0;
	}

	for (i = 1; i < argc; i++) {
		if (StrToNumber(argv[i], &tmp) != 0) {
			print_w_usage();
			retval = -1;
			goto end0;
		}
		
		switch (i) {
			case 1:
				spi_num = tmp;
				break;
			case 2:
				csn = tmp;
				break;
			case 3:
				dev_addr = tmp;
				break;
			case 4:
				reg_addr = tmp;
				break;
			case 5:
				data = tmp;
				break;
			case 6:
				dev_width = tmp;
				if ((dev_width != 1) && (dev_width != 2)) {
					printf("dev_width must be 1 or 2\n");
					print_w_usage();
					retval = -1;
					goto end0;
				}
				break;
			case 7:
				reg_width = tmp;
				if ((reg_width != 1) && (reg_width != 2)) {
					printf("reg_width must be 1 or 2\n");
					print_w_usage();
					retval = -1;
					goto end0;
				}
				break;
			case 8:
				data_width = tmp;
				if ((data_width != 1) && (data_width != 2)) {
					printf("data_width must be 1 or 2\n");
					print_w_usage();
					retval = -1;
					goto end0;
				}
				break;
			case 9:
				reg_order = tmp;
				if ((reg_order != 1) && (reg_order != 0))
				{
					printf("reg_order must be 1 or 0\n");
					print_w_usage();
					retval = -1;
					goto end0;
				}
				break;
			case 10:
				data_order = tmp;
				if ((data_order != 1) && (data_order != 0))
				{
					printf("data_order must be 1 or 0\n");
					print_w_usage();
					retval = -1;
					goto end0;
				}
				break;
			default:
				break;
		}
	}
	
	printf("spi_num:%u, csn:%u\n"
			"dev_addr:0x%04x, reg_addr:0x%04x, data:0x%04x, "
			"dev_width:%d, reg_width:%d, data_width:%d, reg_order: %d, data_order: %d\n",
			spi_num, csn, dev_addr, reg_addr, data,
			dev_width, reg_width, data_width, reg_order, data_order);

	sprintf(file_name, "/dev/spidev%u.%u", spi_num, csn);

	fd = open(file_name, 0);
	if (fd < 0) {
		printf("Open %s error!\n",file_name);
		retval = -1;
		goto end0;
	}

	tmp = SPI_MODE_3;
	retval = ioctl(fd, SPI_IOC_WR_MODE, &tmp);
	if (retval) {
		printf("set spi mode fail!\n");
		retval = -1;
		goto end1;
	}

	memset(mesg, 0, sizeof mesg);

	mesg[0].tx_buf = (__u32)buf;
	mesg[0].rx_buf = (__u32)buf;
	mesg[0].len = dev_width + reg_width + data_width;
	mesg[0].speed_hz = 2000000;
	mesg[0].bits_per_word = 8;
	mesg[0].cs_change = 1;

	memset(buf, 0, sizeof buf);

	if (dev_width == 1) {
		*(__u8*)(&buf[index]) = dev_addr & 0xff;
		index++;
	} else {
		*(__u16*)(&buf[index]) = dev_addr & 0xffff;
		index += 2;
	}

	if (reg_width == 1) {
		*(__u8 *)(&buf[index]) = reg_addr & 0xff;
		index++;
	} else {
		if (reg_order)
			*(__u16 *)(&buf[index]) = reg_addr & 0xffff;
		else {
			*(__u8*)(&buf[index]) = (reg_addr >> 8)  & 0xff;
			*(__u8*)(&buf[index + 1]) = reg_addr & 0xff;
		}
		index += 2;
	}

	if (data_width == 1)
		*(__u8 *)(&buf[index]) = data & 0xff;
	else {
		if (data_order)
			*(__u16 *)(&buf[index]) = data & 0xffff;

		else {
			*(__u8*)(&buf[index]) = (data >> 8)  & 0xff;
			*(__u8*)(&buf[index + 1]) = data   & 0xff;
		}
	}

	reverse8(buf, mesg[0].len);

	retval = ioctl(fd, SPI_IOC_MESSAGE(1), mesg);
	if (retval != mesg[0].len) {  
		printf("SPI_IOC_MESSAGE error \n");  
		retval = -1;
		goto end1;
	}
	retval = 0;

	printf("\n[END]\n");

end1:
	close(fd);

end0:
	return retval;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

