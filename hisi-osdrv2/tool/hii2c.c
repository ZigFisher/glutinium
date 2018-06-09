#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "memmap.h"
#include "hi.h"
#include "strfunc.h"


#ifdef BVT_I2C 

#ifdef HI_GPIO_I2C
#include "gpio_i2c.h"
#else
#include "hi_i2c.h"
#endif

#else

//#include "hi_unf_ecs.h"

#endif
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#ifndef BVT_I2C 
extern int HI_UNF_I2C_Open (void);
extern int HI_UNF_I2C_Close(void);
extern int HI_UNF_I2C_Read(U32 u32I2cNum, U8 u8DevAddress, U32 u32RegAddr,
		U32 u32RegAddrCount, U8 *pu8Buf, U32 u32Length);
extern int HI_UNF_I2C_Write(U32 u32I2cNum, U8 u8DevAddress, U32 u32RegAddr,
		U32  u32RegAddrCount, U8 * pu8Buf, U32 u32Length);
#endif

HI_RET hier(int argc , char* argv[])
{
	U32 I2C_port;
	U32 device_addr;
	U32 reg_addr;
	U8* data;
	U32 len = 0x40;
	U32 count=0;
	int result;

	if (argc < 4)
	{
		printf("usage: %s <device address> <register address> <len>. sample: %s 0xA0 0x10 0x40\n", argv[0]);
		return -1;
	}

	if(StrToNumber(argv[1], &I2C_port) != HI_SUCCESS ) {
		printf("Please input i2c port like 0x100 or 256.\r\n");
		return -1;
	}

	if(StrToNumber(argv[2], &device_addr) != HI_SUCCESS ) {
		printf("Please input dev addr like 0x100 or 256.\r\n");
		return -1;
	}

	if(StrToNumber(argv[3], &reg_addr) != HI_SUCCESS ) {
		printf("Please input reg addr like 0x100 0r 256.\r\n");
		return -1;
	}

	if (argc >= 5)
	{
		if(StrToNumber(argv[4], &len) != HI_SUCCESS ) {
			printf("Please input len like 0x100\n");
			return -1;
		}
	}
	data = (char*)malloc(len);
	if (data == NULL)
    {
		/*EXIT("I2C open error.", result);*/
		printf("[error]not enough memory.\r\n");
    	return -1;
    }
#ifndef BVT_I2C 
result = HI_UNF_I2C_Open();
if (result != HI_SUCCESS)
{
	EXIT("I2C open error.", result);
} 
#else
    int fd;
#ifdef HI_GPIO_I2C
    fd = open("/dev/gpioi2c", 0);  
#else
    fd = open("/dev/hi_i2c", 0);  
#endif
#endif


printf("====I2C read:<%#x> <%#x> <%#x>====\r\n", device_addr, reg_addr, len);

while (count < len)
{
	//U8 buff;
#ifndef BVT_I2C
	result = HI_UNF_I2C_Read(I2C_port,device_addr, reg_addr+count, 1, &data[count], 1);
	if (result != HI_SUCCESS)
	{
		EXIT("I2C read error.", result);

	}

#else

#ifdef HI_GPIO_I2C
	int value;
	value = ((device_addr&0xff)<<24) | (((reg_addr+count)&0xff)<<16);        
	ioctl(fd, GPIO_I2C_READ, &value);        
	data[count] = value&0xff;
#else
	I2C_DATA_S  i2c_data ;
	i2c_data.dev_addr = device_addr ;
	i2c_data.reg_addr = reg_addr+count    ;
	i2c_data.addr_byte_num   = 1  ;
	i2c_data.data_byte_num   = 1 ;
	ioctl(fd, CMD_I2C_READ, &i2c_data);
	data[count] =  i2c_data.data ;
#endif

#endif        
	count++;
}
if (count >0)
	hi_hexdump(STDOUT, (void*)data, count, 16);

#ifndef BVT_I2C
	result = HI_UNF_I2C_Close();
if (result != HI_SUCCESS)
{
	EXIT("I2C close error.", -1);
}
#else
close(fd);
#endif
return 0;
}

HI_RET hiew(int argc , char* argv[])
{
	U32 I2C_port;
	U32 device_addr;
	U32 reg_addr;
	U32 new_data;
	U8 value;
	int result;


	if (argc < 5)
	{
	  printf("usage: %s <device address> <register address> <value>. sample: %s 0xA0 0x10 0x40\n", argv[0]);
		return -1;

	}

	if(StrToNumber(argv[1], &I2C_port) != HI_SUCCESS ) {
		printf("Please input i2c port like 0x100 or 256.\r\n");
		return -1;
	}

	if(StrToNumber(argv[2], &device_addr) != HI_SUCCESS ) {
		printf("Please input dev addr like 0x100 or 256.\r\n");
		return -1;
	}

	if(StrToNumber(argv[3], &reg_addr) != HI_SUCCESS ) {
		printf("Please input reg addr like 0x100 0r 256.\r\n");
		return -1;
	}

	if(StrToNumber(argv[4], &new_data) != HI_SUCCESS ) {
		printf("Please input len like 0x100\n");
		return -1;
	}
	value = (U8)new_data;

	printf("====I2C write:<%#x> <%#x> <%#x>====\n", device_addr, reg_addr, new_data);

#ifndef BVT_I2C    
	result = HI_UNF_I2C_Open();
	if (result != HI_SUCCESS)
	{
		EXIT("I2C open error.", -1);

	}
	result = HI_UNF_I2C_Write(I2C_port, device_addr, reg_addr, 1, &value, 1);
	if (result != HI_SUCCESS)
	{
		EXIT("I2C write error.", result);
	}

	result = HI_UNF_I2C_Close();
	if (result != HI_SUCCESS)
	{
		EXIT("I2C close error.", -1);
	}
#else

	int fd;

#ifdef HI_GPIO_I2C
	int tmp;
	fd = open("/dev/gpioi2c", 0);  
	tmp = ((device_addr&0xff)<<24) | ((reg_addr&0xff)<<16) | (value&0xffff);
	ioctl(fd, GPIO_I2C_WRITE, &tmp);
#else
	I2C_DATA_S  i2c_data ;
	fd = open("/dev/hi_i2c", 0);  
	i2c_data.dev_addr = device_addr ; 
	i2c_data.reg_addr = reg_addr    ; 
	i2c_data.addr_byte_num = 1  ; 
	i2c_data.data     = value ; 
	i2c_data.data_byte_num = 1 ;
	ioctl(fd, CMD_I2C_WRITE, &i2c_data);
#endif
	close(fd);
#endif
	return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
