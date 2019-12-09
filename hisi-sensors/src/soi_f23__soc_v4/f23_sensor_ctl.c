/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name		: soi_sensor_sensor_ctl.c
  Version		: Initial Draft
  Author		: Hisilicon multimedia software group
  Created		: 2019/02/12
  Description	:
  History		:
  1.Date		: 2019/02/15
	Author		:
******************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <hi_math.h>
#include "hi_comm_video.h"
#include "hi_sns_ctrl.h"									//hi3516ev200

#ifdef HI_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "hi_i2c.h"
#endif


//#define	_F23_NOISE_OPT									//min@20180302


const unsigned char	soi_sensor_i2c_addr	 = 0x80;			//Sensor I2C Address
const unsigned int	soi_sensor_addr_byte = 1;
const unsigned int	soi_sensor_data_byte = 1;
static int	g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};

//# extern WDR_MODE_E	genSensorMode;
//# extern HI_U8		gu8SensorImageMode;
//# extern HI_BOOL		bSensorInit;
extern ISP_SNS_STATE_S		*g_pastSoiSensor[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U	g_aunSoiSensorBusInfo[];
extern int   gu8F23_FlipValue;

extern HI_U8		Reg0D;				//min@20190109 add

int soi_sensor_i2c_init(VI_PIPE ViPipe)
{
	char	acDevFile[16] = {0};
	HI_U8	u8DevNum;
	
	if( g_fd[ViPipe] >= 0 ) {
		return HI_SUCCESS;
	}
	
#ifdef HI_GPIO_I2C
	int	ret;
	
	//# g_fd[ViPipe] = open( "/dev/gpioi2c_ex", 0 );
	g_fd[ViPipe] = open( "/dev/gpioi2c_ex", O_RDONLY, S_IRUSR );
	if( g_fd[ViPipe] < 0 ) {
		//# printf( "Open gpioi2c_ex error!\n" );
		ISP_TRACE( HI_DBG_ERR, "Open gpioi2c_ex error!\n" );
		return HI_FAILURE;
	}
#else
	int	ret;
	
	u8DevNum = g_aunSoiSensorBusInfo[ViPipe].s8I2cDev;
	snprintf( acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum );
	
	//# g_fd[ViPipe] = open( "/dev/i2c-0", O_RDWR );
	g_fd[ViPipe] = open( acDevFile, O_RDWR, S_IRUSR | S_IWUSR );
	if( g_fd[ViPipe] < 0 ) {
		//# printf( "Open /dev/i2c-0 error!\n" );
		ISP_TRACE( HI_DBG_ERR, "Open /dev/hi_i2c_drv-%u error!\n", u8DevNum );
		return HI_FAILURE;
	}
	
	ret = ioctl( g_fd[ViPipe], I2C_SLAVE_FORCE, (soi_sensor_i2c_addr>>1) );
	if( ret < 0 ) {
		//# printf( "CMD_SET_DEV error!\n" );
		ISP_TRACE( HI_DBG_ERR, "I2C_SLAVE_FORCE error!\n" );
		close( g_fd[ViPipe] );
		g_fd[ViPipe] = -1;
		return ret;
	}
#endif
	
	return HI_SUCCESS;
}

int soi_sensor_i2c_exit(VI_PIPE ViPipe)
{
	if( g_fd[ViPipe] >= 0 ) {
		close( g_fd[ViPipe] );
		g_fd[ViPipe] = -1;
		return HI_SUCCESS;
	}
	return HI_FAILURE;
}

#if 0 //# TODO
#define	CMD_I2C_WRITE		0x01
#define	CMD_I2C_READ		0x03

/* NOTE: Slave address is 7 or 10 bits, but 10-bit addresses
 * are NOT supported! (due to code brokenness)
 */
#define I2C_SLAVE			0x0703	/* Use this slave address */
#define I2C_SLAVE_FORCE		0x0706	/* Use this slave address, even if it is already in use by a driver! */
#define I2C_TENBIT			0x0704	/* 0 for 7 bit addrs, != 0 for 10 bit */
#define I2C_FUNCS			0x0705	/* Get the adapter functionality mask */
#define I2C_RDWR			0x0707	/* Combined R/W transfer (one STOP only) */
#define I2C_PEC				0x0708	/* != 0 to use PEC with SMBus */
#define I2C_SMBUS			0x0720	/* SMBus transfer */
#define I2C_16BIT_REG		0x0709	/* 16BIT REG WIDTH */
#define I2C_16BIT_DATA		0x070a	/* 16BIT DATA WIDTH */

//sdk\osdrv\opensource\kernel\linux-3.18.y\include\uapi\asm-generic\fcntl.h
//#define O_ACCMODE			00000003
//#define O_RDONLY			00000000
//#define O_WRONLY			00000001
//#define O_RDWR			00000002

struct i2c_msg {
	HI_U16	addr;	/* slave address			*/
	HI_U16	flags;
	#define I2C_M_TEN			0x0010	/* this is a ten bit chip address */
	#define I2C_M_RD			0x0001	/* read data, from slave to master */
	#define I2C_M_STOP			0x8000	/* if I2C_FUNC_PROTOCOL_MANGLING */
	#define I2C_M_NOSTART		0x4000	/* if I2C_FUNC_NOSTART */
	#define I2C_M_REV_DIR_ADDR	0x2000	/* if I2C_FUNC_PROTOCOL_MANGLING */
	#define I2C_M_IGNORE_NAK	0x1000	/* if I2C_FUNC_PROTOCOL_MANGLING */
	#define I2C_M_NO_RD_ACK		0x0800	/* if I2C_FUNC_PROTOCOL_MANGLING */
	#define I2C_M_RECV_LEN		0x0400	/* length will be first received byte */
	#define I2C_M_16BIT_REG		0x0002	/* indicate reg bit-width is 16bit */
	#define I2C_M_16BIT_DATA	0x0008	/* indicate data bit-width is 16bit */
	HI_U16	len;						/* msg length				*/
	HI_U8	*buf;						/* pointer to msg data			*/
};


/* This is the structure as used in the I2C_RDWR ioctl call */
//struct i2c_rdwr_ioctl_data {
//	struct i2c_msg	*msgs;		/* pointers to i2c_msgs */
//	HI_U32			nmsgs;		/* number of i2c_msgs */
//};

int i2c_read(
	unsigned int i2c_num, 
	unsigned int dev_addr, 
	unsigned int reg_addr, 
	unsigned int reg_addr_end, 
	unsigned int reg_width, 
	unsigned int data_width, 
	unsigned int reg_step)
{
	int				retval = 0;
	unsigned char	buf[4] = {0,0,0,0};
	int				cur_addr = 0;
	static struct i2c_rdwr_ioctl_data	rdwr;
	static struct i2c_msg				msg[2];
	unsigned int	data = 0;
	
	msg[0].addr	 = dev_addr >>1;	///dev_addr;
	msg[0].flags = 0;
	msg[0].len	 = reg_width;
	msg[0].buf	 = buf;
	
	msg[1].addr	 = dev_addr >>1;	///dev_addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len	 = data_width;
	msg[1].buf	 = buf;
	
	rdwr.msgs	 = &msg[0];
	rdwr.nmsgs	 = 2;
	for( cur_addr = reg_addr; cur_addr <= reg_addr_end; cur_addr += reg_step ) {
		if( reg_width == 2 ) {
			buf[0] = (cur_addr >> 8) & 0xff;
			buf[1] = (cur_addr     ) & 0xff;
		}
		else {
			buf[0] = cur_addr & 0xff;
		}
		
		retval = ioctl( g_fd[ViPipe], I2C_RDWR, &rdwr );
		
		if( data_width == 2 ) {
			data = (HI_U16)buf[1] | ((HI_U16)buf[0] << 8);
		}
		else {
			data = buf[0];
		}
	}
	
	return data;
}
#endif //#

int soi_sensor_read_register(VI_PIPE ViPipe, int addr)
{
	// TODO:
	
	return HI_SUCCESS;
	//# return i2c_read( 0, soi_sensor_i2c_addr, addr, addr, 1, 1, 1 );
}

int soi_sensor_write_register(VI_PIPE ViPipe, int addr, int data)
{
	if( 0 > g_fd[ViPipe] ) {
		return HI_SUCCESS;
	}
	
#ifdef HI_GPIO_I2C
	i2c_data.dev_addr		= soi_sensor_i2c_addr;
	i2c_data.reg_addr		= addr;
	i2c_data.addr_byte_num	= soi_sensor_addr_byte;
	i2c_data.data			= data;
	i2c_data.data_byte_num	= soi_sensor_data_byte;
	
	ret = ioctl( g_fd[ViPipe], GPIO_I2C_WRITE, &i2c_data );
	if( ret ) {
		//# printf( "GPIO-I2C write faild!\n" );
		ISP_TRACE( HI_DBG_ERR, "GPIO-I2C write faild!\n" );
		return ret;
	}
#else
	int		idx = 0;
	int		ret;
	char	buf[8] = {0};
	
	if( soi_sensor_addr_byte == 2 ) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}
	else {
		buf[idx] = addr & 0xff;
		idx++;
	}
	
	if( soi_sensor_data_byte == 2 ) {
		buf[idx] = (data >> 8) & 0xff;
		idx++;
		buf[idx] = data & 0xff;
		idx++;
	}
	else {
		buf[idx] = data & 0xff;
		idx++;
	}
	
	ret = write( g_fd[ViPipe], buf, soi_sensor_addr_byte + soi_sensor_data_byte );
	if( ret < 0 ) {
		//printf( "I2C_WRITE error!\n" );
		ISP_TRACE( HI_DBG_ERR, "I2C_WRITE error!\n" );
		return HI_FAILURE;
	}
#endif
	return HI_SUCCESS;
}


static void delay_ms(int ms)
{
	usleep( ms * 1000 );
}

void soi_sensor_prog(VI_PIPE ViPipe, int *rom)
{
	int i = 0;
	
	while( 1 ) {
		int lookup = rom[i++];
		int addr = (lookup >> 16) & 0xFFFF;
		int data = lookup & 0xFFFF;
		
		if( addr == 0xFFFE ) {
			delay_ms( data );
		}
		else if( addr == 0xFFFF ) {
			return;
		}
		else {
			soi_sensor_write_register( ViPipe, addr, data );
		}
	}
}

void soi_sensor_standby(VI_PIPE ViPipe)
{
	// TODO:
}

void soi_sensor_restart(VI_PIPE ViPipe)
{
	// TODO:
}

#define		SOI_SENSOR_1080P_30FPS_LINEAR_MODE	(1)
#define		SOI_SENSOR_1080P_30FPS_WDR_MODE		(2)

void soi_sensor_linear_1080p30_init(VI_PIPE ViPipe);
void soi_sensor_2wdr1_1080p30_init(VI_PIPE ViPipe);

void soi_sensor_init(VI_PIPE ViPipe)
{
	//HI_U32	i;
	//HI_BOOL	bInit;
	//HI_U8		u8ImgMode;
	
	//bInit		= g_pastSoiSensor[ViPipe]->bInit;
	//u8ImgMode	= g_pastSoiSensor[ViPipe]->u8ImgMode;
	
	printf( "[JXF23] Date: %s\n", __DATE__ );
	printf( "[JXF23] Time: %s\n", __TIME__ );
	printf( ">> soi_sensor_init()\n" );
	
	soi_sensor_i2c_init( ViPipe );
	
	/* When sensor first init, config all registers */
	//# if( HI_FALSE == bSensorInit )
//	if( HI_FALSE == g_pastSoiSensor[ViPipe]->bInit )
	{
//		if( WDR_MODE_2To1_LINE == pstSnsState->enWDRMode )
		{
//			soi_sensor_2wdr1_1080p30_init( ViPipe );
//			g_pastSoiSensor[ViPipe]->bInit = HI_TRUE;
			//# bSensorInit = HI_TRUE;
		}
//		else
//		{
			soi_sensor_linear_1080p30_init( ViPipe );
			g_pastSoiSensor[ViPipe]->bInit = HI_TRUE;
			//# bSensorInit = HI_TRUE;
//		}
	}
	/* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
//	else
//	{
//		if( WDR_MODE_2To1_LINE == pstSnsState->enWDRMode )
//		{
//			soi_sensor_2wdr1_1080p30_init( ViPipe );
//		}
//		else
//		{
//			soi_sensor_linear_1080p30_init( ViPipe );
//		}
//	}
	
	printf( "<< soi_sensor_init()\n" );
}

void soi_sensor_exit(VI_PIPE ViPipe)
{
	soi_sensor_i2c_exit( ViPipe );
}

void soi_sensor_linear_1080p30_init(VI_PIPE ViPipe)
{
	printf( ">> soi_sensor_linear_1080p30_init()\n" );
	
	delay_ms( 10 );
	
	#if 1 //F23AE_079_MIPI10_2L_1920x1080x30_M24_P43P2_MP216_2560x1125_FH_DccOn_Pub20180409
		soi_sensor_write_register( ViPipe, 0x0E, 0x11 );
		soi_sensor_write_register( ViPipe, 0x0F, 0x14 );
		soi_sensor_write_register( ViPipe, 0x10, 0x40 );
		soi_sensor_write_register( ViPipe, 0x11, 0x80 );
		soi_sensor_write_register( ViPipe, 0x48, 0x05 );
		soi_sensor_write_register( ViPipe, 0x96, 0xAA );
		soi_sensor_write_register( ViPipe, 0x94, 0xC0 );
		soi_sensor_write_register( ViPipe, 0x97, 0x8D );
		soi_sensor_write_register( ViPipe, 0x96, 0x00 );
		soi_sensor_write_register( ViPipe, 0x12, 0x40 );
		soi_sensor_write_register( ViPipe, 0x0E, 0x11 );
		soi_sensor_write_register( ViPipe, 0x0F, 0x14 );
		soi_sensor_write_register( ViPipe, 0x10, 0x24 );
		soi_sensor_write_register( ViPipe, 0x11, 0x80 );
		soi_sensor_write_register( ViPipe, 0x0D, 0xA0 );
		Reg0D = 0xA0;										//min@20180705
		soi_sensor_write_register( ViPipe, 0x5F, 0x41 );
		soi_sensor_write_register( ViPipe, 0x60, 0x20 );
		soi_sensor_write_register( ViPipe, 0x58, 0x12 );
		soi_sensor_write_register( ViPipe, 0x57, 0x60 );
		soi_sensor_write_register( ViPipe, 0x9D, 0x00 );
		soi_sensor_write_register( ViPipe, 0x20, 0x00 );
		soi_sensor_write_register( ViPipe, 0x21, 0x05 );
		soi_sensor_write_register( ViPipe, 0x22, 0x65 );
		soi_sensor_write_register( ViPipe, 0x23, 0x04 );
		soi_sensor_write_register( ViPipe, 0x24, 0xC0 );
		soi_sensor_write_register( ViPipe, 0x25, 0x38 );
		soi_sensor_write_register( ViPipe, 0x26, 0x43 );
		soi_sensor_write_register( ViPipe, 0x27, 0xC3 );
		soi_sensor_write_register( ViPipe, 0x28, 0x19 );
		soi_sensor_write_register( ViPipe, 0x29, 0x04 );
		soi_sensor_write_register( ViPipe, 0x2C, 0x00 );
		soi_sensor_write_register( ViPipe, 0x2D, 0x00 );
		soi_sensor_write_register( ViPipe, 0x2E, 0x18 );
		soi_sensor_write_register( ViPipe, 0x2F, 0x44 );
		soi_sensor_write_register( ViPipe, 0x41, 0xC9 );
		soi_sensor_write_register( ViPipe, 0x42, 0x13 );
		soi_sensor_write_register( ViPipe, 0x46, 0x00 );
		soi_sensor_write_register( ViPipe, 0x76, 0x60 );
		soi_sensor_write_register( ViPipe, 0x77, 0x09 );
		soi_sensor_write_register( ViPipe, 0x1D, 0x00 );
		soi_sensor_write_register( ViPipe, 0x1E, 0x04 );
		soi_sensor_write_register( ViPipe, 0x6C, 0x40 );
		soi_sensor_write_register( ViPipe, 0x68, 0x00 );
		soi_sensor_write_register( ViPipe, 0x6E, 0x2C );
		soi_sensor_write_register( ViPipe, 0x70, 0x6C );
		soi_sensor_write_register( ViPipe, 0x71, 0x6D );
		soi_sensor_write_register( ViPipe, 0x72, 0x6A );
		soi_sensor_write_register( ViPipe, 0x73, 0x36 );
		soi_sensor_write_register( ViPipe, 0x74, 0x02 );
		soi_sensor_write_register( ViPipe, 0x78, 0x9E );
		soi_sensor_write_register( ViPipe, 0x89, 0x01 );
		soi_sensor_write_register( ViPipe, 0x2A, 0xB1 );
		soi_sensor_write_register( ViPipe, 0x2B, 0x24 );
		soi_sensor_write_register( ViPipe, 0x31, 0x08 );
		soi_sensor_write_register( ViPipe, 0x32, 0x4F );
		soi_sensor_write_register( ViPipe, 0x33, 0x20 );
		soi_sensor_write_register( ViPipe, 0x34, 0x5E );
		soi_sensor_write_register( ViPipe, 0x35, 0x5E );
		soi_sensor_write_register( ViPipe, 0x3A, 0xAF );
		soi_sensor_write_register( ViPipe, 0x56, 0x32 );
		soi_sensor_write_register( ViPipe, 0x59, 0xBF );
		soi_sensor_write_register( ViPipe, 0x5A, 0x04 );
		soi_sensor_write_register( ViPipe, 0x85, 0x5A );
		soi_sensor_write_register( ViPipe, 0x8A, 0x04 );
		soi_sensor_write_register( ViPipe, 0x8F, 0x90 );
		soi_sensor_write_register( ViPipe, 0x91, 0x13 );
		soi_sensor_write_register( ViPipe, 0x5B, 0xA0 );
		soi_sensor_write_register( ViPipe, 0x5C, 0xF0 );
		soi_sensor_write_register( ViPipe, 0x5D, 0xF4 );
		soi_sensor_write_register( ViPipe, 0x5E, 0x1F );
		soi_sensor_write_register( ViPipe, 0x62, 0x04 );
		soi_sensor_write_register( ViPipe, 0x63, 0x0F );
		soi_sensor_write_register( ViPipe, 0x64, 0xC0 );
///		soi_sensor_write_register( ViPipe, 0x66, 0x44 );
		soi_sensor_write_register( ViPipe, 0x66, 0x04 );	//min@20190109
		#ifdef _F23_NOISE_OPT //min@20180302
		soi_sensor_write_register( ViPipe, 0x67, 0x76 );
		#else
		soi_sensor_write_register( ViPipe, 0x67, 0x73 );
		#endif
		soi_sensor_write_register( ViPipe, 0x69, 0x7C );
		soi_sensor_write_register( ViPipe, 0x6A, 0x28 );
		soi_sensor_write_register( ViPipe, 0x7A, 0xC0 );
		soi_sensor_write_register( ViPipe, 0x4A, 0x05 );
		soi_sensor_write_register( ViPipe, 0x7E, 0xCD );
		soi_sensor_write_register( ViPipe, 0x49, 0x10 );
//		soi_sensor_write_register( ViPipe, 0x50, 0x02 );
		soi_sensor_write_register( ViPipe, 0x50, 0x03 );	//min@20180514
		soi_sensor_write_register( ViPipe, 0x7B, 0x4A );
		soi_sensor_write_register( ViPipe, 0x7C, 0x0C );
		soi_sensor_write_register( ViPipe, 0x7F, 0x57 );
		soi_sensor_write_register( ViPipe, 0x90, 0x00 );
		soi_sensor_write_register( ViPipe, 0x8E, 0x00 );
		soi_sensor_write_register( ViPipe, 0x8C, 0xFF );
		soi_sensor_write_register( ViPipe, 0x8D, 0xC7 );
		soi_sensor_write_register( ViPipe, 0x8B, 0x01 );
		soi_sensor_write_register( ViPipe, 0x0C, 0x00 );
///		soi_sensor_write_register( ViPipe, 0x0C, 0x40 );
		soi_sensor_write_register( ViPipe, 0x65, 0x02 );
		soi_sensor_write_register( ViPipe, 0x80, 0x1A );
		soi_sensor_write_register( ViPipe, 0x81, 0xC0 );
		soi_sensor_write_register( ViPipe, 0x19, 0x28 );	//[3]: Enable AE change every frame.
		
        printf("gu8FlipValue=[0x%x]\r\n", gu8F23_FlipValue);
        soi_sensor_write_register( ViPipe, 0x12,gu8F23_FlipValue);
		//soi_sensor_write_register( ViPipe, 0x12, 0x00 );
		soi_sensor_write_register( ViPipe, 0x48, 0x8A );
		soi_sensor_write_register( ViPipe, 0x48, 0x0A );
	#endif
	
	printf( "<< soi_sensor_linear_1080p30_init()\n" );
}

void soi_sensor_2wdr1_1080p30_init(VI_PIPE ViPipe)
{
	printf( ">> soi_sensor_2wdr1_1080p30_init()\n" );
	
	delay_ms( 10 );
	
	#if 1
		soi_sensor_write_register( ViPipe, 0x12, 0x40 );
		soi_sensor_write_register( ViPipe, 0x0E, 0x11 );
		soi_sensor_write_register( ViPipe, 0x0F, 0x14 );
		soi_sensor_write_register( ViPipe, 0x10, 0x24 );
		soi_sensor_write_register( ViPipe, 0x11, 0x80 );
		soi_sensor_write_register( ViPipe, 0x0D, 0xA0 );
		Reg0D = 0xA0;										//min@20180705
		soi_sensor_write_register( ViPipe, 0x5F, 0x41 );
		soi_sensor_write_register( ViPipe, 0x60, 0x1E );
		soi_sensor_write_register( ViPipe, 0x58, 0x12 );
		soi_sensor_write_register( ViPipe, 0x57, 0x60 );
		soi_sensor_write_register( ViPipe, 0x9D, 0x00 );
		soi_sensor_write_register( ViPipe, 0x20, 0x00 );
		soi_sensor_write_register( ViPipe, 0x21, 0x05 );
		soi_sensor_write_register( ViPipe, 0x22, 0x65 );
		soi_sensor_write_register( ViPipe, 0x23, 0x04 );
		soi_sensor_write_register( ViPipe, 0x24, 0xC0 );
		soi_sensor_write_register( ViPipe, 0x25, 0x38 );
		soi_sensor_write_register( ViPipe, 0x26, 0x43 );
		soi_sensor_write_register( ViPipe, 0x27, 0x47 );
		soi_sensor_write_register( ViPipe, 0x28, 0x19 );
		soi_sensor_write_register( ViPipe, 0x29, 0x04 );
		soi_sensor_write_register( ViPipe, 0x2C, 0x00 );
		soi_sensor_write_register( ViPipe, 0x2D, 0x00 );
		soi_sensor_write_register( ViPipe, 0x2E, 0x18 );
		soi_sensor_write_register( ViPipe, 0x2F, 0x44 );
		soi_sensor_write_register( ViPipe, 0x41, 0xC8 );
		soi_sensor_write_register( ViPipe, 0x42, 0x13 );
		soi_sensor_write_register( ViPipe, 0x46, 0x00 );
		soi_sensor_write_register( ViPipe, 0x76, 0x60 );
		soi_sensor_write_register( ViPipe, 0x77, 0x09 );
		soi_sensor_write_register( ViPipe, 0x1D, 0x00 );
		soi_sensor_write_register( ViPipe, 0x1E, 0x04 );
		soi_sensor_write_register( ViPipe, 0x6C, 0x40 );
		soi_sensor_write_register( ViPipe, 0x68, 0x00 );
		soi_sensor_write_register( ViPipe, 0x6E, 0x2C );
		soi_sensor_write_register( ViPipe, 0x70, 0x6C );
		soi_sensor_write_register( ViPipe, 0x71, 0x6D );
		soi_sensor_write_register( ViPipe, 0x72, 0x6A );
		soi_sensor_write_register( ViPipe, 0x73, 0x36 );
		soi_sensor_write_register( ViPipe, 0x74, 0x02 );
		soi_sensor_write_register( ViPipe, 0x78, 0x9E );
		soi_sensor_write_register( ViPipe, 0x89, 0x01 );
		soi_sensor_write_register( ViPipe, 0x2A, 0x38 );
		soi_sensor_write_register( ViPipe, 0x2B, 0x24 );
		soi_sensor_write_register( ViPipe, 0x31, 0x08 );
		soi_sensor_write_register( ViPipe, 0x32, 0x4F );
		soi_sensor_write_register( ViPipe, 0x33, 0x20 );
		soi_sensor_write_register( ViPipe, 0x34, 0x5E );
		soi_sensor_write_register( ViPipe, 0x35, 0x5E );
		soi_sensor_write_register( ViPipe, 0x3A, 0xA0 );
		soi_sensor_write_register( ViPipe, 0x59, 0x87 );
		soi_sensor_write_register( ViPipe, 0x5A, 0x04 );
		soi_sensor_write_register( ViPipe, 0x8A, 0x04 );
		soi_sensor_write_register( ViPipe, 0x91, 0x13 );
		soi_sensor_write_register( ViPipe, 0x5B, 0xA0 );
		soi_sensor_write_register( ViPipe, 0x5C, 0xF0 );
		soi_sensor_write_register( ViPipe, 0x5D, 0xF4 );
		soi_sensor_write_register( ViPipe, 0x5E, 0x1F );
		soi_sensor_write_register( ViPipe, 0x62, 0x04 );
		soi_sensor_write_register( ViPipe, 0x63, 0x0F );
		soi_sensor_write_register( ViPipe, 0x64, 0xC0 );
///		soi_sensor_write_register( ViPipe, 0x66, 0x44 );
		soi_sensor_write_register( ViPipe, 0x66, 0x04 );	//min@20190109
		#ifdef _F23_NOISE_OPT //min@20180302
		soi_sensor_write_register( ViPipe, 0x67, 0x76 );
		#else
		soi_sensor_write_register( ViPipe, 0x67, 0x73 );
		#endif
		soi_sensor_write_register( ViPipe, 0x69, 0x7C );
		soi_sensor_write_register( ViPipe, 0x7A, 0xC0 );
		soi_sensor_write_register( ViPipe, 0x4A, 0x05 );
		soi_sensor_write_register( ViPipe, 0x7E, 0xCD );
		soi_sensor_write_register( ViPipe, 0x49, 0x10 );
///		soi_sensor_write_register( ViPipe, 0x50, 0x02 );
		soi_sensor_write_register( ViPipe, 0x50, 0x03 );
		soi_sensor_write_register( ViPipe, 0x7B, 0x4A );
		soi_sensor_write_register( ViPipe, 0x7C, 0x0C );
		soi_sensor_write_register( ViPipe, 0x7F, 0x57 );
		soi_sensor_write_register( ViPipe, 0x8F, 0x80 );
		soi_sensor_write_register( ViPipe, 0x90, 0x00 );
		soi_sensor_write_register( ViPipe, 0x8E, 0x00 );
		soi_sensor_write_register( ViPipe, 0x8C, 0xFF );
		soi_sensor_write_register( ViPipe, 0x8D, 0xC7 );
		soi_sensor_write_register( ViPipe, 0x8B, 0x01 );
		soi_sensor_write_register( ViPipe, 0x0C, 0x00 );
///		soi_sensor_write_register( ViPipe, 0x0C, 0x40 );
		soi_sensor_write_register( ViPipe, 0x6A, 0x4D );
		soi_sensor_write_register( ViPipe, 0x65, 0x07 );
		soi_sensor_write_register( ViPipe, 0x80, 0x02 );
		soi_sensor_write_register( ViPipe, 0x81, 0xC0 );
		soi_sensor_write_register( ViPipe, 0x19, 0x20 );
		soi_sensor_write_register( ViPipe, 0x12, 0x00 );
		soi_sensor_write_register( ViPipe, 0x48, 0x8A );
		soi_sensor_write_register( ViPipe, 0x48, 0x0A );
	#endif
	
	printf( "<< soi_sensor_2wdr1_1080p30_init()\n" );
}

