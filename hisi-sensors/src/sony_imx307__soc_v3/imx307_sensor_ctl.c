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

const unsigned char sensor_i2c_addr     =    0x34;        /* I2C Address of IMX290 */
const unsigned int  sensor_addr_byte    =    2;
const unsigned int  sensor_data_byte    =    1;
static int g_fd = -1;

extern WDR_MODE_E genSensorMode;
extern HI_U8 gu8SensorImageMode;
extern HI_BOOL bSensorInit;

//sensor fps mode
#define IMX290_SENSOR_1080P_30FPS_LINEAR_MODE  (1)
#define IMX290_SENSOR_1080P_30FPS_WDR_MODE     (2)
#define IMX290_SENSOR_1080P_60FPS_WDR_MODE     (3)

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

    ret = ioctl(g_fd, I2C_SLAVE_FORCE, (sensor_i2c_addr>>1));
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
    hi_usleep(ms*1000);
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

void sensor_wdr_1080p30_2to1_init(HI_VOID);


void sensor_linear_1080p30_init(HI_VOID);

void sensor_init(HI_VOID)
{
    sensor_i2c_init();
    
    /* When sensor first init, config all registers */
    if (HI_FALSE == bSensorInit) 
    {
        if (WDR_MODE_2To1_LINE == genSensorMode)
        {
          
            sensor_wdr_1080p30_2to1_init();
            bSensorInit = HI_TRUE;
        }

   

        else
        {
            sensor_linear_1080p30_init();
            bSensorInit = HI_TRUE;
        }
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else 
    {
        if (WDR_MODE_2To1_LINE == genSensorMode)
        {
             
            sensor_wdr_1080p30_2to1_init();
        }

        else
        {
            sensor_linear_1080p30_init();
        }      
    }

    return ;
}

void sensor_exit(HI_VOID)
{
    sensor_i2c_exit();

    return;
}


/* 1080P30 and 1080P25 */
void sensor_linear_1080p30_init(HI_VOID)
{

	// Enter Standby
	sensor_write_register(0x3000, 0x01); 	// Standby mode
	sensor_write_register(0x3002, 0x01);	// Master mode stop

	// Mode register setting
	sensor_write_register(0x3005, 0x01);
	sensor_write_register(0x3007, 0x00);
	sensor_write_register(0x3009, 0x02);	// 60fps;0x00->120fps
	sensor_write_register(0x300c, 0x00);
	sensor_write_register(0x3010, 0x21);
	sensor_write_register(0x3011, 0x0a);
	sensor_write_register(0x3014, 0x00);	// gain
	sensor_write_register(0x3018, 0x65);	// VMAX
	sensor_write_register(0x3019, 0x04);
	sensor_write_register(0x301c, 0x30);	// HMAX;
	sensor_write_register(0x301d, 0x11); 	// HMAX;
	sensor_write_register(0x3020, 0x01);	// SHS1
	sensor_write_register(0x3021, 0x00);	// SHS1
	sensor_write_register(0x3022, 0x00);	// SHS1
	sensor_write_register(0x3030, 0x0B);	// RHS1
	sensor_write_register(0x3031, 0x00);	// RHS1
	sensor_write_register(0x3032, 0x00);	// RHS1
	sensor_write_register(0x3024, 0x00);	// SHS2
	sensor_write_register(0x3025, 0x00);	// SHS2
	sensor_write_register(0x3026, 0x00);	// SHS2
	sensor_write_register(0x3045, 0x01);
	sensor_write_register(0x3046, 0x01);	// MIPI
	sensor_write_register(0x305c, 0x18);	//37.125MHz INCK Setting
	sensor_write_register(0x305d, 0x03);
	sensor_write_register(0x305e, 0x20);
	sensor_write_register(0x305f, 0x01);
	sensor_write_register(0x309e, 0x4a);
	sensor_write_register(0x309f, 0x4a);
	sensor_write_register(0x3106, 0x00);	
	sensor_write_register(0x311c, 0x0e);
	sensor_write_register(0x3128, 0x04);
	sensor_write_register(0x3129, 0x00);
	sensor_write_register(0x313b, 0x41);
	sensor_write_register(0x315e, 0x1a);	//37.125MHz INCK5 Setting
	sensor_write_register(0x3164, 0x1a);
	sensor_write_register(0x3480, 0x49);	//37.125MHz INCK7 Setting
	sensor_write_register(0x3129, 0x00);	// ADBIT1,12Bit;
	sensor_write_register(0x317c, 0x00);	// ADBIT2,12Bit;
	sensor_write_register(0x31ec, 0x0e);	// ADBIT3,12Bit;

	// Standby Cancel
	sensor_write_register(0x3000, 0x00); 	// standby
	usleep(20000);	// DELAY20mS
	sensor_write_register(0x3002, 0x00); 	// master mode start
	sensor_write_register(0x304B, 0x0a); 	// XVSOUTSEL XHSOUTSEL enable output
	usleep(20000);

	bSensorInit = HI_TRUE;
	printf("==============================================================\n");
	printf("===Sony imx307 sensor 1080P30fps(MIPI) init success!=====\n");
	printf("==============================================================\n");
    return;
}


void sensor_wdr_1080p30_2to1_init(HI_VOID)
{
#if 0
    sensor_write_register (0x3000, 0x01); /* standby */
    sensor_write_register (0x3002, 0x01); /* XTMSTA */
    delay_ms(200);

    //12bit
    sensor_write_register (0x3005, 0x01);
    sensor_write_register (0x3007, 0x00);
    sensor_write_register (0x3009, 0x01);
    sensor_write_register (0x300a, 0xF0);
    sensor_write_register (0x300c, 0x11);
    sensor_write_register (0x3011, 0x0A);
    sensor_write_register (0x3018, 0x65); //VMAX
    sensor_write_register (0x3019, 0x04); //VMAX
    sensor_write_register (0x301c, 0x98); //HMAX
    sensor_write_register (0x301d, 0x08); //HMAX
    sensor_write_register (0x3020, 0x02);	//SHS1
	sensor_write_register (0x3021, 0x00);	//SHS1
	sensor_write_register (0x3024, 0xC9); //SHS2
    sensor_write_register (0x3025, 0x06); //SHS2
    sensor_write_register (0x3030, 0x0B); //RHS1
    sensor_write_register (0x3031, 0x00); //RHS1
    sensor_write_register (0x3045, 0x05);
    sensor_write_register (0x3046, 0x01);
    sensor_write_register (0x304b, 0x0a);
    sensor_write_register (0x305c, 0x18);
    sensor_write_register (0x305d, 0x03);
    sensor_write_register (0x305e, 0x20);
    sensor_write_register (0x305f, 0x01);
    sensor_write_register (0x309e, 0x4a);
    sensor_write_register (0x309f, 0x4a);
    
    sensor_write_register (0x3106, 0x11);
    sensor_write_register (0x311c, 0x0e);
    sensor_write_register (0x3128, 0x04); //SHS3
    sensor_write_register (0x3129, 0x00); //SHS3
    sensor_write_register (0x313b, 0x41);
    sensor_write_register (0x315e, 0x1a);
    sensor_write_register (0x3164, 0x1a);
    sensor_write_register (0x317c, 0x00);
    sensor_write_register (0x31ec, 0x0e);
    
    sensor_write_register (0x3405, 0x10);
    sensor_write_register (0x3407, 0x03);
    sensor_write_register (0x3414, 0x0a);
    sensor_write_register (0x3415, 0x00);
    sensor_write_register (0x3418, 0x9c);
	sensor_write_register (0x3419, 0x08);
    sensor_write_register (0x3441, 0x0C);
    sensor_write_register (0x3442, 0x0C);
    sensor_write_register (0x3443, 0x03);
    sensor_write_register (0x3444, 0x20);
    sensor_write_register (0x3445, 0x25);
    sensor_write_register (0x3446, 0x57);
    sensor_write_register (0x3447, 0x00);
    sensor_write_register (0x3448, 0x37);
    sensor_write_register (0x3449, 0x00);
    sensor_write_register (0x344a, 0x1f);
    sensor_write_register (0x344b, 0x00);
    sensor_write_register (0x344c, 0x1f);
    sensor_write_register (0x344d, 0x00);
    sensor_write_register (0x344e, 0x1f);
    sensor_write_register (0x344f, 0x00);
    sensor_write_register (0x3450, 0x77);
    sensor_write_register (0x3451, 0x00);
    sensor_write_register (0x3452, 0x1f);
    sensor_write_register (0x3453, 0x00);
    sensor_write_register (0x3454, 0x17);
    sensor_write_register (0x3455, 0x00);
    sensor_write_register (0x3472, 0xa0);
    sensor_write_register (0x3473, 0x07);
    sensor_write_register (0x347b, 0x23);
    sensor_write_register (0x3480, 0x49);
    
    delay_ms(200);
    sensor_write_register (0x3000, 0x00); /* standby cancel */
    sensor_write_register (0x3002, 0x00);
    sensor_write_register (0x304b, 0x0a);
#endif
    sensor_write_register(0x3000, 0x01); //# standby 
    delay_ms(200);

    sensor_write_register(0x3005, 0x01); //# 12Bit, 0x00,10Bit;
    sensor_write_register(0x3007, 0x00); //#
    sensor_write_register(0x3009, 0x11); //#
    sensor_write_register(0x300c, 0x11); //#
    sensor_write_register(0x3010, 0x21); //#
    sensor_write_register(0x3011, 0x0a); //# Change after reset;
    sensor_write_register(0x3014, 0x02); //# Gain
    sensor_write_register(0x3020, 0x02); //# SHS1?
    sensor_write_register(0x3021, 0x00); //# 
    sensor_write_register(0x3022, 0x00); //# 
    sensor_write_register(0x3024, 0xC1); //# SHS2?
    sensor_write_register(0x3025, 0x08); //# 
    sensor_write_register(0x3026, 0x00); //# 
    //sensor_write_register(0x3030, 0x0B); //# RHS1?
    //sensor_write_register(0x3031, 0x00); //#
    sensor_write_register(0x3030, 0xCF); //# RHS1?
    sensor_write_register(0x3031, 0x01); //#
    sensor_write_register(0x3032, 0x00); //#
    //sensor_write_register(0x3018, 0x65); //# VMAX[7:0] 
    //sensor_write_register(0x3019, 0x04); //# VMAX[15:8] 
    sensor_write_register(0x3018, 0x46); //# VMAX[7:0] 
    sensor_write_register(0x3019, 0x05); //# VMAX[15:8] 
    sensor_write_register(0x301a, 0x00); //# VMAX[16] 
    sensor_write_register(0x301c, 0x98); //# HMAX[7:0]      0x14a0->25fps;
    sensor_write_register(0x301d, 0x08); //# HMAX[15:8]     0x1130->30fps;
    sensor_write_register(0x3045, 0x05); //#
    sensor_write_register(0x3046, 0x01); //# OPORTSE&ODBIT  
    sensor_write_register(0x305C, 0x18); //# INCKSEL1,1080P,CSI-2,37.125MHz;74.25MHz->0x0C
    sensor_write_register(0x305D, 0x03); //# INCKSEL2,1080P,CSI-2,37.125MHz;74.25MHz->0x03
    sensor_write_register(0x305E, 0x20); //# INCKSEL3,1080P,CSI-2,37.125MHz;74.25MHz->0x10
    sensor_write_register(0x305F, 0x01); //# INCKSEL4,1080P,CSI-2,37.125MHz;74.25MHz->0x01
    sensor_write_register(0x309e, 0x4a); //#
    sensor_write_register(0x309f, 0x4a); //#
    sensor_write_register(0x3106, 0x10); //#
    sensor_write_register(0x311c, 0x0e); //#
    sensor_write_register(0x3128, 0x04); //#
    sensor_write_register(0x3129, 0x00); //# ADBIT1,12Bit;0x1D->10Bit; 
    sensor_write_register(0x313b, 0x41); //#
    sensor_write_register(0x315E, 0x1A); //# INCKSEL5,1080P,CSI-2,37.125MHz;74.25MHz->0x1B
    sensor_write_register(0x3164, 0x1A); //# INCKSEL6,1080P,CSI-2,37.125MHz;74.25MHz->0x1B
    sensor_write_register(0x317C, 0x00); //# ADBIT2,12Bit;0x12->10Bit;
    sensor_write_register(0x31EC, 0x0E); //# ADBIT3,12Bit;0x37->10Bit;
    sensor_write_register(0x3480, 0x49); //# INCKSEL7,1080P,CSI-2,37.125MHz;74.25MHz->0x92
    									 //
    //##MIPI setting						 //
    sensor_write_register(0x3405, 0x10); //#
    sensor_write_register(0x3407, 0x03); //#
    sensor_write_register(0x3414, 0x0a); //#
    sensor_write_register(0x3415, 0x00); //#
    //sensor_write_register(0x3418, 0x9c); //# Y_OUT_SIZE
    //sensor_write_register(0x3419, 0x08); //#
    sensor_write_register(0x3418, 0x6e); //# Y_OUT_SIZE
    sensor_write_register(0x3419, 0x0a); //#
    sensor_write_register(0x3441, 0x0c); //# CSI_DT_FMT 12Bit
    sensor_write_register(0x3442, 0x0c); //#
    sensor_write_register(0x3443, 0x03); //# MIPI 4CH
    sensor_write_register(0x3444, 0x20); //#
    sensor_write_register(0x3445, 0x25); //#
    sensor_write_register(0x3446, 0x57); //#
    sensor_write_register(0x3447, 0x00); //#
    sensor_write_register(0x3448, 0x37); //#
    sensor_write_register(0x3449, 0x00); //#
    sensor_write_register(0x344a, 0x1f); //#
    sensor_write_register(0x344b, 0x00); //#
    sensor_write_register(0x344c, 0x1f); //#
    sensor_write_register(0x344d, 0x00); //#
    sensor_write_register(0x344e, 0x1f); //#
    sensor_write_register(0x344f, 0x00); //#
    sensor_write_register(0x3450, 0x77); //#
    sensor_write_register(0x3451, 0x00); //#
    sensor_write_register(0x3452, 0x1e); //#
    sensor_write_register(0x3453, 0x00); //#
    sensor_write_register(0x3454, 0x17); //#
    sensor_write_register(0x3455, 0x00); //#
    sensor_write_register(0x3472, 0xa0); //#
    sensor_write_register(0x3473, 0x07); //#
    sensor_write_register(0x347b, 0x23); //#
    									 //
    delay_ms(200);						 //
    sensor_write_register(0x3000, 0x00); //# Standby Cancel
    sensor_write_register(0x3002, 0x00); //#
    sensor_write_register(0x304b, 0x0a); //#


    printf("===Imx307 sensor 1080P30fps 12bit 2to1 WDR init success!=====\n");

    bSensorInit = HI_TRUE;

    return;
    
}







