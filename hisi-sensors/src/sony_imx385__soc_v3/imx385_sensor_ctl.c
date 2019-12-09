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
    
    sensor_write_register (0x3000, 0x01); /* standby */
    delay_ms(20);
    sensor_write_register (0x3009, 0x02);
    sensor_write_register (0x3012, 0x2c);
    sensor_write_register (0x3013, 0x01);
    sensor_write_register (0x3014, 0x07); //Gain
    sensor_write_register (0x3018, 0x47);
    sensor_write_register (0x3019, 0x05);
    sensor_write_register (0x301b, 0x30);
    sensor_write_register (0x301c, 0x11);
    sensor_write_register (0x3020, 0x02); //SHS1
    sensor_write_register (0x3021, 0x00); //SHS1
    sensor_write_register (0x3049, 0x0a);
    sensor_write_register (0x3054, 0x66);
    sensor_write_register (0x305d, 0x00);
    sensor_write_register (0x305f, 0x00);
    sensor_write_register (0x310b, 0x07);
    sensor_write_register (0x3110, 0x12);
    sensor_write_register (0x31ed, 0x38);
    sensor_write_register (0x3338, 0xd4);
    sensor_write_register (0x3339, 0x40);
    sensor_write_register (0x333a, 0x10);
    sensor_write_register (0x333b, 0x00);
    sensor_write_register (0x333c, 0xd4);
    sensor_write_register (0x333d, 0x40);
    sensor_write_register (0x333e, 0x10);
    sensor_write_register (0x333f, 0x00);
    sensor_write_register (0x3344, 0x10);
    sensor_write_register (0x336b, 0x2f);
    sensor_write_register (0x3380, 0x20);
    sensor_write_register (0x3381, 0x25);
    sensor_write_register (0x3382, 0x5f);
    sensor_write_register (0x3383, 0x17);
    sensor_write_register (0x3384, 0x2f);
    sensor_write_register (0x3385, 0x17);
    sensor_write_register (0x3386, 0x17);
    sensor_write_register (0x3387, 0x0f);
    sensor_write_register (0x3388, 0x4f);
    sensor_write_register (0x338d, 0xb4);
    sensor_write_register (0x338e, 0x01);
    delay_ms(20);
    sensor_write_register (0x3000, 0x00); /* standby cancel */
    
    sensor_write_register (0x3002, 0x00); 

    printf("===IMX385 1080P 30fps 12bit LINE Init OK!===\n");    
    bSensorInit = HI_TRUE;

    return;
}


void sensor_wdr_1080p30_2to1_init(HI_VOID)
{
    sensor_write_register (0x3000, 0x01); /* standby */
    //sensor_write_register (0x3002, 0x01); /* XTMSTA */
    delay_ms(20);

    //10bit
    sensor_write_register (0x3007, 0x10);
    
    sensor_write_register (0x3009, 0x11);
    
    sensor_write_register (0x3109, 0x01);
    sensor_write_register (0x310b, 0x07);
    sensor_write_register (0x300c, 0x11);
    sensor_write_register (0x3110, 0x12);
    
    sensor_write_register (0x3012, 0x2c);
    sensor_write_register (0x3013, 0x01);
    sensor_write_register (0x3014, 0x06); //Gain
    
    sensor_write_register (0x3018, 0x47); //VMAX
    sensor_write_register (0x3019, 0x05);
    
    sensor_write_register (0x3020, 0x03); //SHS1

    sensor_write_register (0x3023, 0xb9); //SHS2
    sensor_write_register (0x3024, 0x08);
    
    sensor_write_register (0x302c, 0xd7); //RHS1
    sensor_write_register (0x302d, 0x01); //RHS1

    sensor_write_register (0x3338, 0xD4);  
    sensor_write_register (0x3339, 0x40);
    
    sensor_write_register (0x333a, 0x10);
    sensor_write_register (0x333b, 0x00);
    sensor_write_register (0x333c, 0xd4);
    sensor_write_register (0x333d, 0x40);
    sensor_write_register (0x333e, 0x10);
    sensor_write_register (0x333f, 0x00);
    
    sensor_write_register (0x3043, 0x05);
    sensor_write_register (0x3049, 0x0a);
    
    sensor_write_register (0x3054, 0x66);
    sensor_write_register (0x3354, 0x00);
    //sensor_write_register (0x3357, 0xb2); //Y_OUT_SIZE
    //sensor_write_register (0x3358, 0x08);
    sensor_write_register (0x3357, 0x76); //Y_OUT_SIZE
    sensor_write_register (0x3358, 0x0a);
    sensor_write_register (0x305d, 0x00);
    sensor_write_register (0x305f, 0x00);
    sensor_write_register (0x3380, 0x20);
    sensor_write_register (0x3381, 0x25);
    sensor_write_register (0x338d, 0xb4);
    sensor_write_register (0x338e, 0x01);
    sensor_write_register (0x31ed, 0x38);
    
    delay_ms(20);
    sensor_write_register (0x3000, 0x00); /* standby cancel */
    
    sensor_write_register (0x3002, 0x00); 


    printf("===Imx385 sensor 1080P15fps 12bit 2to1 WDR init success!=====\n");

    bSensorInit = HI_TRUE;

    return;
    
}







