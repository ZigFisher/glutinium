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

void sensor_wdr_1080p30_2to1_init();
void sensor_wdr_1080p60_2to1_init();
void sensor_wdr_1080p120_2to1_init();
void sensor_wdr_720p60_2to1_init();
void sensor_wdr_1080p30_3to1_init();
void sensor_wdr_1080p120_3to1_init();
void sensor_wdr_720p60_3to1_init();
void sensor_linear_1080p30_init();

void sensor_init()
{
    sensor_i2c_init();
    
    /* When sensor first init, config all registers */
    if (HI_FALSE == bSensorInit) 
    {
        if (WDR_MODE_2To1_LINE == genSensorMode)
        {
            sensor_wdr_1080p60_2to1_init();
            //sensor_wdr_1080p30_2to1_init();
            bSensorInit = HI_TRUE;
        }

        else if (WDR_MODE_3To1_LINE == genSensorMode)
        {
            sensor_wdr_1080p120_3to1_init();
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
             sensor_wdr_1080p60_2to1_init();
            //sensor_wdr_1080p30_2to1_init();
        }

        else if (WDR_MODE_3To1_LINE == genSensorMode)
        {
            sensor_wdr_1080p120_3to1_init();
        }

        else
        {
            sensor_linear_1080p30_init();
        }      
    }

    return ;
}

void sensor_exit()
{
    sensor_i2c_exit();

    return;
}


/* 1080P30 and 1080P25 */
void sensor_linear_1080p30_init()
{
    sensor_write_register (0x3000, 0x01); /* standby */
    sensor_write_register (0x3002, 0x01); /* XTMSTA */
    
    sensor_write_register (0x3005, 0x01); //ADBIT
    sensor_write_register (0x3129, 0x00); //ADBIT1
    sensor_write_register (0x317c, 0x00); //ADBIT2
    sensor_write_register (0x31ec, 0x0e); //ADBIT3
    sensor_write_register (0x3441, 0x0c); //CSI_DT_FMT
    sensor_write_register (0x3442, 0x0c); //CSI_DT_FMT
    
    sensor_write_register (0x3007, 0x00);  
    sensor_write_register (0x300c, 0x00);
    sensor_write_register (0x300f, 0x00);
    sensor_write_register (0x3010, 0x21);
    sensor_write_register (0x3012, 0x64);
    sensor_write_register (0x3016, 0x09);
    sensor_write_register (0x3017, 0x00);
#if 1
    sensor_write_register (0x3009, 0x12);  /* Gain */
    sensor_write_register (0x3014, 0x0b);
    sensor_write_register (0x3018, 0x47);  /* Vmax */ 
    sensor_write_register (0x3019, 0x05);
    sensor_write_register (0x3020, 0x01);  /* SHS1 */
    sensor_write_register (0x3021, 0x00);
    sensor_write_register (0x3024, 0x00);  /* SHS2 */ 
    sensor_write_register (0x3025, 0x00);
    sensor_write_register (0x3028, 0x00);  /* SHS3 */
    sensor_write_register (0x3029, 0x00);
    sensor_write_register (0x3030, 0x00);  /* RHS1 */
    sensor_write_register (0x3031, 0x00);
    sensor_write_register (0x3034, 0x00);  /* RHS2 */ 
    sensor_write_register (0x3035, 0x00);
#else
    sensor_write_register (0x3020, 0x02);
#endif
    sensor_write_register (0x305c, 0x18);
    sensor_write_register (0x305d, 0x03); 
    sensor_write_register (0x305e, 0x20); 
    sensor_write_register (0x305f, 0x01);
    sensor_write_register (0x3070, 0x02);
    sensor_write_register (0x3071, 0x11);
    sensor_write_register (0x309b, 0x10);
    sensor_write_register (0x309c, 0x22);
    sensor_write_register (0x30a2, 0x02);
    sensor_write_register (0x30a6, 0x20);
    sensor_write_register (0x30a8, 0x20);
    sensor_write_register (0x30aa, 0x20);
    sensor_write_register (0x30ac, 0x20);
    
    sensor_write_register (0x30b0, 0x43);
    sensor_write_register (0x3119, 0x9e);
    sensor_write_register (0x311c, 0x1e);
    sensor_write_register (0x311e, 0x08);
    sensor_write_register (0x3128, 0x05);
    sensor_write_register (0x313d, 0x83);
    sensor_write_register (0x3150, 0x03);

    sensor_write_register (0x317e, 0x00);
    sensor_write_register (0x315e, 0x1a);
    sensor_write_register (0x3164, 0x1a);
    sensor_write_register (0x32b8, 0x50);

    sensor_write_register (0x32b9, 0x10);
    sensor_write_register (0x32ba, 0x00);
    sensor_write_register (0x32bb, 0x04);
    sensor_write_register (0x32c8, 0x50);
    sensor_write_register (0x32c9, 0x10);
    sensor_write_register (0x32ca, 0x00);
    sensor_write_register (0x32cb, 0x04);
    sensor_write_register (0x332c, 0xd3);
    sensor_write_register (0x332d, 0x10);
    sensor_write_register (0x332e, 0x0d);
    sensor_write_register (0x3358, 0x06);
    sensor_write_register (0x3359, 0xe1);
    sensor_write_register (0x335a, 0x11);
    sensor_write_register (0x3360, 0x1e);
    
    sensor_write_register (0x3361, 0x61);
    sensor_write_register (0x3362, 0x10);
    sensor_write_register (0x33b0, 0x50);
    sensor_write_register (0x33b2, 0x1a);
    sensor_write_register (0x33b3, 0x04);
    sensor_write_register (0x3414, 0x0a);
    sensor_write_register (0x3418, 0x49);
    sensor_write_register (0x3419, 0x04);
    sensor_write_register (0x3444, 0x20);
    sensor_write_register (0x3445, 0x25);

    sensor_write_register (0x3446, 0x47);
    sensor_write_register (0x3447, 0x0);
    sensor_write_register (0x3448, 0x1f);
    sensor_write_register (0x3449, 0x0);
    sensor_write_register (0x344a, 0x17);
    sensor_write_register (0x344b, 0x0);
    sensor_write_register (0x344c, 0x0f);
    sensor_write_register (0x344d, 0x0);
    sensor_write_register (0x344e, 0x17);
    sensor_write_register (0x344f, 0x0);
    sensor_write_register (0x3450, 0x47);
    sensor_write_register (0x3451, 0x0);
    sensor_write_register (0x3452, 0x0f);
    sensor_write_register (0x3453, 0x0);
    sensor_write_register (0x3454, 0x0f);
    sensor_write_register (0x3455, 0x0);

    sensor_write_register (0x3480, 0x49);

    sensor_write_register (0x3000, 0x00); /* standby */
    delay_ms(20);
    sensor_write_register (0x3002, 0x00); /* master mode start */
    sensor_write_register (0x304b, 0x0a); /* XVSOUTSEL XHSOUTSEL */
    
    printf("===IMX290 1080P 30fps 12bit LINE Init OK!===\n");    
    bSensorInit = HI_TRUE;

    return;
}


void sensor_wdr_1080p30_2to1_init()
{
    sensor_write_register (0x3000, 0x01); /* standby */
    sensor_write_register (0x3002, 0x01); /* XTMSTA */

    //10bit
    sensor_write_register (0x3005, 0x00);
    sensor_write_register (0x3007, 0x00);
    sensor_write_register (0x300a, 0x3c);
    sensor_write_register (0x300c, 0x11);
    sensor_write_register (0x300f, 0x00);
    sensor_write_register (0x3010, 0x21);
    sensor_write_register (0x3012, 0x64);
    sensor_write_register (0x3016, 0x09);

    sensor_write_register (0x3018, 0xA6);   /**** VMAX ****/
    sensor_write_register (0x3019, 0x04);
    sensor_write_register (0x301A, 0x00);

    sensor_write_register (0x301C, 0xD8);  /***** HMAX ****/
    sensor_write_register (0x301D, 0x0F);
    
    sensor_write_register (0x3020, 0x02);
    sensor_write_register (0x3024, 0xc9);
    sensor_write_register (0x3030, 0x0b);
    sensor_write_register (0x3045, 0x05);
    sensor_write_register (0x3046, 0x00);
    sensor_write_register (0x304b, 0x0a);
    sensor_write_register (0x305c, 0x18);
    sensor_write_register (0x305d, 0x03);
    sensor_write_register (0x305e, 0x20);
    sensor_write_register (0x305f, 0x01);
    sensor_write_register (0x3070, 0x02);
    sensor_write_register (0x3071, 0x11);
    sensor_write_register (0x309b, 0x10);
    sensor_write_register (0x309c, 0x22);
    sensor_write_register (0x30a2, 0x02);
    sensor_write_register (0x30a6, 0x20);
    sensor_write_register (0x30a8, 0x20);
    sensor_write_register (0x30aa, 0x20);
    sensor_write_register (0x30ac, 0x20);
    sensor_write_register (0x30b0, 0x43);
    sensor_write_register (0x3106, 0x11);
    sensor_write_register (0x3119, 0x9e);
    sensor_write_register (0x311c, 0x1e);
    sensor_write_register (0x311e, 0x08);
    sensor_write_register (0x3128, 0x05);
    sensor_write_register (0x3129, 0x1d);
    sensor_write_register (0x313d, 0x83);
    sensor_write_register (0x3150, 0x03);
    sensor_write_register (0x315e, 0x1a);
    sensor_write_register (0x3164, 0x1a);
    sensor_write_register (0x317c, 0x12);
    sensor_write_register (0x317e, 0x00);
    sensor_write_register (0x31ec, 0x37);
    sensor_write_register (0x32b8, 0x50);
    sensor_write_register (0x32b9, 0x10);
    sensor_write_register (0x32ba, 0x00);
    sensor_write_register (0x32bb, 0x04);
    sensor_write_register (0x32c8, 0x50);
    sensor_write_register (0x32c9, 0x10);
    sensor_write_register (0x32ca, 0x00);
    sensor_write_register (0x32cb, 0x04);
    sensor_write_register (0x332c, 0xd3);
    sensor_write_register (0x332d, 0x10);
    sensor_write_register (0x332e, 0x0d);
    sensor_write_register (0x3358, 0x06);
    sensor_write_register (0x3359, 0xe1);
    sensor_write_register (0x335a, 0x11);
    sensor_write_register (0x3360, 0x1e);
    sensor_write_register (0x3361, 0x61);
    sensor_write_register (0x3362, 0x10);
    sensor_write_register (0x33b0, 0x50);
    sensor_write_register (0x33b2, 0x1a);
    sensor_write_register (0x33b3, 0x04);

    sensor_write_register (0x3418, 0xb2); /**** Y_OUT_SIZE *****/ 
    sensor_write_register (0x3419, 0x08);
   
    sensor_write_register (0x3441, 0x0a);
    sensor_write_register (0x3442, 0x0a);
    sensor_write_register (0x3444, 0x20);
    sensor_write_register (0x3445, 0x25);
    sensor_write_register (0x3480, 0x49);
 
    sensor_write_register (0x3000, 0x00); /* standby */
    delay_ms(20); 
    sensor_write_register (0x3002, 0x00); /* master mode start */
 
    printf("===Imx290 sensor 1080P15fps 10bit 2to1 WDR(30fps->15fps) init success!=====\n");

    bSensorInit = HI_TRUE;

    return;
    
}

void sensor_wdr_1080p30_3to1_init()
{
    sensor_write_register (0x3000, 0x01); /* standby */
    sensor_write_register (0x3002, 0x01); /* XTMSTA */

    //12bit
    sensor_write_register (0x3007, 0x00);
    sensor_write_register (0x300c, 0x21);
    sensor_write_register (0x300f, 0x00);
    sensor_write_register (0x3010, 0x21);
    sensor_write_register (0x3012, 0x64);
    sensor_write_register (0x3016, 0x09);
    sensor_write_register (0x3020, 0x04);
    sensor_write_register (0x3024, 0x89);
    sensor_write_register (0x3028, 0x93);
    sensor_write_register (0x3029, 0x01);
    sensor_write_register (0x3030, 0x85);
    sensor_write_register (0x3034, 0x92);
    sensor_write_register (0x3045, 0x05);
    sensor_write_register (0x304b, 0x0a);
    sensor_write_register (0x305c, 0x18);
    sensor_write_register (0x305d, 0x03);
    sensor_write_register (0x305e, 0x20);
    sensor_write_register (0x305f, 0x01);
    sensor_write_register (0x3070, 0x02);
    sensor_write_register (0x3071, 0x11);
    sensor_write_register (0x309b, 0x10);
    sensor_write_register (0x309c, 0x22);
    sensor_write_register (0x30a2, 0x02);
    sensor_write_register (0x30a6, 0x20);
    sensor_write_register (0x30a8, 0x20);
    sensor_write_register (0x30aa, 0x20);
    sensor_write_register (0x30ac, 0x20);
    sensor_write_register (0x30b0, 0x43);
    sensor_write_register (0x3106, 0x33);
    sensor_write_register (0x3119, 0x9e);
    sensor_write_register (0x311c, 0x1e);
    sensor_write_register (0x311e, 0x08);
    sensor_write_register (0x3128, 0x05);
    sensor_write_register (0x313d, 0x83);
    sensor_write_register (0x3150, 0x03);
    sensor_write_register (0x315e, 0x1a);
    sensor_write_register (0x3164, 0x1a);
    sensor_write_register (0x317e, 0x00);
    sensor_write_register (0x32b8, 0x50);
    sensor_write_register (0x32b9, 0x10);
    sensor_write_register (0x32ba, 0x00);
    sensor_write_register (0x32bb, 0x04);
    sensor_write_register (0x32c8, 0x50);
    sensor_write_register (0x32c9, 0x10);
    sensor_write_register (0x32ca, 0x00);
    sensor_write_register (0x32cb, 0x04);
    sensor_write_register (0x332c, 0xd3);
    sensor_write_register (0x332d, 0x10);
    sensor_write_register (0x332e, 0x0d);
    sensor_write_register (0x3358, 0x06);
    sensor_write_register (0x3359, 0xe1);
    sensor_write_register (0x335a, 0x11);
    sensor_write_register (0x3360, 0x1e);
    sensor_write_register (0x3361, 0x61);
    sensor_write_register (0x3362, 0x10);
    sensor_write_register (0x33b0, 0x50);
    sensor_write_register (0x33b2, 0x1a);
    sensor_write_register (0x33b3, 0x04);

    sensor_write_register (0x3418, 0xa0); /**** Y_OUT_SIZE *****/ 
    sensor_write_register (0x3419, 0x0d);
    
    sensor_write_register (0x3444, 0x20);
    sensor_write_register (0x3445, 0x25);
    sensor_write_register (0x3480, 0x49);

    sensor_write_register (0x3000, 0x00); /* standby */
    delay_ms(20); 
    sensor_write_register (0x3002, 0x00); /* master mode start */
 
    printf("===Imx290 sensor 1080P15fps 12bit 3to1 WDR(30fps->7p5fps) init success!=====\n");

    bSensorInit = HI_TRUE;

    return;
    
}

void sensor_wdr_720p60_2to1_init()
{
    sensor_write_register (0x3000, 0x01); /* standby */
    sensor_write_register (0x3002, 0x01); /* XTMSTA */

    //12bit
    sensor_write_register (0x3005, 0x01);
    sensor_write_register (0x3007, 0x10);
    sensor_write_register (0x3009, 0x01);
    sensor_write_register (0x300a, 0xf0);
    sensor_write_register (0x300c, 0x11);
    sensor_write_register (0x300f, 0x00);
    sensor_write_register (0x3010, 0x21);
    sensor_write_register (0x3012, 0x64);
    sensor_write_register (0x3016, 0x09);
    sensor_write_register (0x3018, 0xee);
    sensor_write_register (0x3019, 0x02);
    sensor_write_register (0x301c, 0xe4);
    sensor_write_register (0x301d, 0x0c);
    sensor_write_register (0x3045, 0x05);
    sensor_write_register (0x3046, 0x01);
    sensor_write_register (0x304b, 0x0a);

    sensor_write_register (0x305c, 0x20); //INCKSEL1
    sensor_write_register (0x305d, 0x03); //INCKSEL2
    sensor_write_register (0x305e, 0x20); //INCKSEL3
    sensor_write_register (0x305f, 0x01); //INCKSEL4

    sensor_write_register (0x3070, 0x02);
    sensor_write_register (0x3071, 0x11);
    sensor_write_register (0x309b, 0x10);
    sensor_write_register (0x309c, 0x22);
    sensor_write_register (0x30a2, 0x02);
    sensor_write_register (0x30a6, 0x20);
    sensor_write_register (0x30a8, 0x20);
    sensor_write_register (0x30aa, 0x20);
    sensor_write_register (0x30ac, 0x20);
    sensor_write_register (0x30b0, 0x43);

    //Add 
    sensor_write_register (0x3106, 0x11);
    
    sensor_write_register (0x3119, 0x9e);
    sensor_write_register (0x311c, 0x1e);
    sensor_write_register (0x311e, 0x08);
    sensor_write_register (0x3128, 0x05);
    sensor_write_register (0x3129, 0x00);
    sensor_write_register (0x313d, 0x83);
    sensor_write_register (0x3150, 0x03);
    sensor_write_register (0x315e, 0x1a);
    sensor_write_register (0x3164, 0x1a);
    sensor_write_register (0x317c, 0x00);
    sensor_write_register (0x317e, 0x00);
    sensor_write_register (0x31ec, 0x00);
    
    sensor_write_register (0x32b8, 0x50);
    sensor_write_register (0x32b9, 0x10);
    sensor_write_register (0x32ba, 0x00);
    sensor_write_register (0x32bb, 0x04);
    sensor_write_register (0x32c8, 0x50);
    sensor_write_register (0x32c9, 0x10);
    sensor_write_register (0x32ca, 0x00);
    sensor_write_register (0x32cb, 0x04);
    
    sensor_write_register (0x332c, 0xd3);
    sensor_write_register (0x332d, 0x10);
    sensor_write_register (0x332e, 0x0d);
    sensor_write_register (0x3358, 0x06);
    sensor_write_register (0x3359, 0xe1);
    sensor_write_register (0x335a, 0x11);
    sensor_write_register (0x3360, 0x1e);
    sensor_write_register (0x3361, 0x61);
    sensor_write_register (0x3362, 0x10);
    sensor_write_register (0x33b0, 0x50);
    sensor_write_register (0x33b2, 0x1a);
    sensor_write_register (0x33b3, 0x04);
    
    sensor_write_register (0x3405, 0x10);
    sensor_write_register (0x3407, 0x03);
    sensor_write_register (0x3414, 0x04);
    sensor_write_register (0x3418, 0xc6);
    sensor_write_register (0x3419, 0x05);
    sensor_write_register (0x3441, 0x0c);
    sensor_write_register (0x3442, 0x0c);
    sensor_write_register (0x3443, 0x03);
    sensor_write_register (0x3444, 0x20);
    sensor_write_register (0x3445, 0x25);
    sensor_write_register (0x3446, 0x4f);
    sensor_write_register (0x3447, 0x00);
    sensor_write_register (0x3448, 0x2f);
    sensor_write_register (0x3449, 0x00);
    sensor_write_register (0x344a, 0x17);
    sensor_write_register (0x344b, 0x00);
    sensor_write_register (0x344c, 0x17);
    sensor_write_register (0x344d, 0x00);
    sensor_write_register (0x344e, 0x17);
    sensor_write_register (0x344f, 0x00);
    sensor_write_register (0x3450, 0x57);
    sensor_write_register (0x3451, 0x00);
    sensor_write_register (0x3452, 0x17);
    sensor_write_register (0x3453, 0x00);
    sensor_write_register (0x3454, 0x17);
    sensor_write_register (0x3455, 0x00);
    sensor_write_register (0x3472, 0x1c);
    sensor_write_register (0x3473, 0x05);
    sensor_write_register (0x3480, 0x49);

    sensor_write_register (0x3000, 0x00); /* standby */
    delay_ms(20); 
    sensor_write_register (0x3002, 0x00); /* master mode start */
 
    printf("===Imx290 sensor 720P30fps 12bit 2to1 WDR(60fps->30fps) init success!=====\n");

    bSensorInit = HI_TRUE;

    return;
}

void sensor_wdr_720p60_3to1_init()
{
    sensor_write_register (0x3000, 0x01); /* standby */
    sensor_write_register (0x3002, 0x01); /* XTMSTA */
    
    //12bit
    sensor_write_register (0x3005, 0x01);
    sensor_write_register (0x3007, 0x10);
    sensor_write_register (0x3009, 0x01);
    sensor_write_register (0x300a, 0xf0);
    sensor_write_register (0x300c, 0x31);
    sensor_write_register (0x300f, 0x00);
    sensor_write_register (0x3010, 0x21);
    sensor_write_register (0x3012, 0x64);
    sensor_write_register (0x3016, 0x09);
    sensor_write_register (0x3018, 0xee);
    sensor_write_register (0x3019, 0x02);
    sensor_write_register (0x301c, 0xe4);
    sensor_write_register (0x301d, 0x0c);
    sensor_write_register (0x3045, 0x05);
    sensor_write_register (0x3046, 0x01);
    sensor_write_register (0x304b, 0x0a);

    sensor_write_register (0x305c, 0x20); //INCKSEL1
    sensor_write_register (0x305d, 0x03); //INCKSEL2
    sensor_write_register (0x305e, 0x20); //INCKSEL3
    sensor_write_register (0x305f, 0x01); //INCKSEL4
    
    sensor_write_register (0x3070, 0x02);
    sensor_write_register (0x3071, 0x11);
    sensor_write_register (0x309b, 0x10);
    sensor_write_register (0x309c, 0x22);
    sensor_write_register (0x30a2, 0x02);
    sensor_write_register (0x30a6, 0x20);
    sensor_write_register (0x30a8, 0x20);
    sensor_write_register (0x30aa, 0x20);
    sensor_write_register (0x30ac, 0x20);
    sensor_write_register (0x30b0, 0x43);

    //Add 
    sensor_write_register (0x3106, 0x33);
    
    sensor_write_register (0x3119, 0x9e);
    sensor_write_register (0x311c, 0x1e);
    sensor_write_register (0x311e, 0x08);
    sensor_write_register (0x3128, 0x05);
    sensor_write_register (0x3129, 0x00);
    sensor_write_register (0x313d, 0x83);
    sensor_write_register (0x3150, 0x03);
    sensor_write_register (0x315e, 0x1a);
    sensor_write_register (0x3164, 0x1a);
    sensor_write_register (0x317c, 0x00);
    sensor_write_register (0x317e, 0x00);
    sensor_write_register (0x31ec, 0x00);
    
    sensor_write_register (0x32b8, 0x50);
    sensor_write_register (0x32b9, 0x10);
    sensor_write_register (0x32ba, 0x00);
    sensor_write_register (0x32bb, 0x04);
    sensor_write_register (0x32c8, 0x50);
    sensor_write_register (0x32c9, 0x10);
    sensor_write_register (0x32ca, 0x00);
    sensor_write_register (0x32cb, 0x04);
    
    sensor_write_register (0x332c, 0xd3);
    sensor_write_register (0x332d, 0x10);
    sensor_write_register (0x332e, 0x0d);
    sensor_write_register (0x3358, 0x06);
    sensor_write_register (0x3359, 0xe1);
    sensor_write_register (0x335a, 0x11);
    sensor_write_register (0x3360, 0x1e);
    sensor_write_register (0x3361, 0x61);
    sensor_write_register (0x3362, 0x10);
    sensor_write_register (0x33b0, 0x50);
    sensor_write_register (0x33b2, 0x1a);
    sensor_write_register (0x33b3, 0x04);
    
    sensor_write_register (0x3405, 0x10);
    sensor_write_register (0x3407, 0x03);
    sensor_write_register (0x3414, 0x04);
    sensor_write_register (0x3418, 0xb5);
    sensor_write_register (0x3419, 0x08);
    sensor_write_register (0x3441, 0x0c);
    sensor_write_register (0x3442, 0x0c);
    sensor_write_register (0x3443, 0x03);
    sensor_write_register (0x3444, 0x20);
    sensor_write_register (0x3445, 0x25);
    sensor_write_register (0x3446, 0x4f);
    sensor_write_register (0x3447, 0x00);
    sensor_write_register (0x3448, 0x2f);
    sensor_write_register (0x3449, 0x00);
    sensor_write_register (0x344a, 0x17);
    sensor_write_register (0x344b, 0x00);
    sensor_write_register (0x344c, 0x17);
    sensor_write_register (0x344d, 0x00);
    sensor_write_register (0x344e, 0x17);
    sensor_write_register (0x344f, 0x00);
    sensor_write_register (0x3450, 0x57);
    sensor_write_register (0x3451, 0x00);
    sensor_write_register (0x3452, 0x17);
    sensor_write_register (0x3453, 0x00);
    sensor_write_register (0x3454, 0x17);
    sensor_write_register (0x3455, 0x00);
    sensor_write_register (0x3472, 0x1c);
    sensor_write_register (0x3473, 0x05);
    sensor_write_register (0x3480, 0x49);

    sensor_write_register (0x3000, 0x00); /* standby */
    delay_ms(20); 
    sensor_write_register (0x3002, 0x00); /* master mode start */
 
    printf("===Imx290 sensor 720P15fps 3to1 WDR(60fps->15fps) init success!=====\n");

    bSensorInit = HI_TRUE;

    return;
}

void sensor_wdr_1080p60_2to1_init()
{
 #if 0   
    sensor_write_register (0x3000, 0x01); /* standby */
    sensor_write_register (0x3002, 0x00); /* XTMSTA */

    sensor_write_register (0x3005, 0x01);
    sensor_write_register (0x3007, 0x00);
    sensor_write_register (0x3009, 0x01);
    sensor_write_register (0x300a, 0xf0);
    sensor_write_register (0x300c, 0x11);
    sensor_write_register (0x300f, 0x00);
    sensor_write_register (0x3010, 0x21);
    sensor_write_register (0x3012, 0x64);
    sensor_write_register (0x3016, 0x09);
    sensor_write_register (0x3018, 0x65);
    sensor_write_register (0x3019, 0x04);

    sensor_write_register (0x301c, 0x98); /* HMAX */
    sensor_write_register (0x301d, 0x08); /* HMAX */

    sensor_write_register (0x3045, 0x05);
    sensor_write_register (0x3046, 0x01);
    sensor_write_register (0x304b, 0x0a);
    
    sensor_write_register (0x305c, 0x18);
    sensor_write_register (0x305d, 0x03);
    sensor_write_register (0x305e, 0x20);
    sensor_write_register (0x305f, 0x01);
    
    sensor_write_register (0x3070, 0x02);
    sensor_write_register (0x3071, 0x11);
    
    sensor_write_register (0x309b, 0x10);
    sensor_write_register (0x309c, 0x22);
    
    sensor_write_register (0x30a2, 0x02);
    sensor_write_register (0x30a6, 0x20);
    sensor_write_register (0x30a8, 0x20);
    
    sensor_write_register (0x30aa, 0x20);
    sensor_write_register (0x30ac, 0x20);
    sensor_write_register (0x30b0, 0x43);

    sensor_write_register (0x3106, 0x11);
    sensor_write_register (0x3119, 0x9e);
    sensor_write_register (0x311c, 0x1e);
    sensor_write_register (0x311e, 0x08);
    
    sensor_write_register (0x3128, 0x05);
    sensor_write_register (0x3129, 0x00);
    sensor_write_register (0x313d, 0x83);
    sensor_write_register (0x3150, 0x03);
    
    sensor_write_register (0x315e, 0x1a);
    sensor_write_register (0x3164, 0x1a);
    sensor_write_register (0x317c, 0x00);
    sensor_write_register (0x317e, 0x00);
    sensor_write_register (0x31ec, 0x00);
                                         
    sensor_write_register (0x32b8, 0x50);
    sensor_write_register (0x32b9, 0x10);
    sensor_write_register (0x32ba, 0x00);
    sensor_write_register (0x32bb, 0x04);
    
    sensor_write_register (0x32c8, 0x50);
    sensor_write_register (0x32c9, 0x10);
    sensor_write_register (0x32ca, 0x00);
    sensor_write_register (0x32cb, 0x04);
                                        
    sensor_write_register (0x332c, 0xd3);
    sensor_write_register (0x332d, 0x10);
    sensor_write_register (0x332e, 0x0d);
    
    sensor_write_register (0x3358, 0x06);
    sensor_write_register (0x3359, 0xe1);
    sensor_write_register (0x335a, 0x11);
    
    sensor_write_register (0x3360, 0x1e);
    sensor_write_register (0x3361, 0x61);
    sensor_write_register (0x3362, 0x10);
    
    sensor_write_register (0x33b0, 0x50);
    sensor_write_register (0x33b2, 0x1a);
    sensor_write_register (0x33b3, 0x04);
                                         
    sensor_write_register (0x3405, 0x10);
    sensor_write_register (0x3407, 0x03);
    sensor_write_register (0x3414, 0x0a);
    sensor_write_register (0x3415, 0x00);
    sensor_write_register (0x3418, 0xb2);
    sensor_write_register (0x3419, 0x08);
    
    sensor_write_register (0x3441, 0x0c);
    sensor_write_register (0x3442, 0x0c);
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
    sensor_write_register (0x3472, 0x9c);
    sensor_write_register (0x3473, 0x07);
    sensor_write_register (0x3480, 0x49);

    sensor_write_register (0x3000, 0x00); /* standby */
    delay_ms(20); 
    sensor_write_register (0x3002, 0x00); /* master mode start */
 
    printf("===Imx290 sensor 1080P30fps 12bit 2to1 WDR(60fps->30fps) init success!=====\n");

    bSensorInit = HI_TRUE;

    return;
 #endif
    
	sensor_write_register (0x3000, 0x01); /* standby */
	sensor_write_register (0x3002, 0x01); /* XTMSTA */
	                                               
	sensor_write_register (0x3005, 0x00);             
	sensor_write_register (0x3007, 0x00);
#if 1    
	sensor_write_register (0x3009, 0x11); /* HCG */
#else
	sensor_write_register (0x3009, 0x01);             
#endif

	sensor_write_register (0x300a, 0x3c);             
	sensor_write_register (0x300c, 0x11);             
	sensor_write_register (0x300f, 0x00);             
	sensor_write_register (0x3010, 0x21);             
	sensor_write_register (0x3012, 0x64); 
#if 1
	sensor_write_register (0x3014, 0x34); /* Gain */
#endif
	sensor_write_register (0x3016, 0x09);
#if 0
	sensor_write_register (0x3018, 0xb9);  /* Vmax */ 
 	sensor_write_register (0x3019, 0x05);
#else
	sensor_write_register (0x3018, 0xC4); /* VMAX */  
 	sensor_write_register (0x3019, 0x04); /* VMAX */  
#endif
	sensor_write_register (0x301c, 0xEC); /* HMAX */  
	sensor_write_register (0x301d, 0x07); /* HMAX */  

#if 1
    sensor_write_register (0x3020, 0x03);  /* SHS1 */
    sensor_write_register (0x3021, 0x00);
    //sensor_write_register (0x3022, 0x00);
	sensor_write_register (0x3024, 0xfa);  /* SHS2 */     
	sensor_write_register (0x3025, 0x08); 
    //sensor_write_register (0x3025, 0x00);
    sensor_write_register (0x3028, 0x00);  /* SHS3 */
    sensor_write_register (0x3029, 0x00);
    //sensor_write_register (0x302A, 0x00);
    sensor_write_register (0x3030, 0x93);  /* RHS1 */
    sensor_write_register (0x3031, 0x00);
    sensor_write_register (0x3034, 0x00);  /* RHS2 */ 
    sensor_write_register (0x3035, 0x00);
#endif
    
    
    sensor_write_register (0x3045, 0x05);
    sensor_write_register (0x3046, 0x00);
    sensor_write_register (0x304b, 0x0a);
    sensor_write_register (0x305c, 0x18);
    sensor_write_register (0x305d, 0x03);
    sensor_write_register (0x305e, 0x20);
    sensor_write_register (0x305f, 0x01);
    sensor_write_register (0x3070, 0x02);
    sensor_write_register (0x3071, 0x11);
    sensor_write_register (0x309b, 0x10);
    sensor_write_register (0x309c, 0x22);
    sensor_write_register (0x30a2, 0x02);
    sensor_write_register (0x30a6, 0x20);
    sensor_write_register (0x30a8, 0x20);
    sensor_write_register (0x30aa, 0x20);
    sensor_write_register (0x30ac, 0x20);
    sensor_write_register (0x30b0, 0x43);

    sensor_write_register (0x3106, 0x11); 
    sensor_write_register (0x3119, 0x9e);
    sensor_write_register (0x311c, 0x1e);
    sensor_write_register (0x311e, 0x08);
    sensor_write_register (0x3128, 0x05);
    sensor_write_register (0x3129, 0x1d);
    sensor_write_register (0x313d, 0x83);
    sensor_write_register (0x3150, 0x03);
    sensor_write_register (0x315e, 0x1a);
    sensor_write_register (0x3164, 0x1a);
    sensor_write_register (0x317c, 0x12);
    sensor_write_register (0x317e, 0x00);
    sensor_write_register (0x31ec, 0x37);
                                         
    sensor_write_register (0x32b8, 0x50);
    sensor_write_register (0x32b9, 0x10);
    sensor_write_register (0x32ba, 0x00);
    sensor_write_register (0x32bb, 0x04);
    sensor_write_register (0x32c8, 0x50);
    sensor_write_register (0x32c9, 0x10);
    sensor_write_register (0x32ca, 0x00);
    sensor_write_register (0x32cb, 0x04);
                                        
    sensor_write_register (0x332c, 0xd3);                      
    sensor_write_register (0x332d, 0x10);                                 
    sensor_write_register (0x332e, 0x0d);                         
    sensor_write_register (0x3358, 0x06);                         
    sensor_write_register (0x3359, 0xe1);                         
    sensor_write_register (0x335a, 0x11);                         
    sensor_write_register (0x3360, 0x1e);                         
    sensor_write_register (0x3361, 0x61);                         
    sensor_write_register (0x3362, 0x10);                         
    sensor_write_register (0x33b0, 0x50);                         
    sensor_write_register (0x33b2, 0x1a);                         
    sensor_write_register (0x33b3, 0x04);                         
                                                                  
    sensor_write_register (0x3405, 0x10);                         
    sensor_write_register (0x3407, 0x03);                         
    sensor_write_register (0x3414, 0x0a);                         
    sensor_write_register (0x3415, 0x00);                         
    sensor_write_register (0x3418, 0x32);  /* Y_OUT_SIZE */       
    sensor_write_register (0x3419, 0x09);  /* Y_OUT_SIZE */       
    sensor_write_register (0x3441, 0x0a);                         
    sensor_write_register (0x3442, 0x0a);                         
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
    sensor_write_register (0x3472, 0x9c);                  
    sensor_write_register (0x3473, 0x07);                  
    sensor_write_register (0x3480, 0x49);  

    sensor_write_register (0x3000, 0x00); /* standby */
    delay_ms(20); 
    sensor_write_register (0x3002, 0x00); /* master mode start */
 
    printf("===Imx290 sensor 1080P30fps 10bit 2to1 WDR(60fps->30fps) init success!=====\n"); 
                                           
    return;

}

void sensor_wdr_1080p120_2to1_init()
{
    sensor_write_register (0x3000, 0x01); /* standby */
    sensor_write_register (0x3002, 0x01); /* XTMSTA */

    sensor_write_register (0x3005, 0x00);
    sensor_write_register (0x3007, 0x00);
    sensor_write_register (0x3009, 0x00);
    sensor_write_register (0x300a, 0x3c);
    sensor_write_register (0x300c, 0x11);
    sensor_write_register (0x300f, 0x00);
    sensor_write_register (0x3010, 0x21);
    sensor_write_register (0x3012, 0x64);
    sensor_write_register (0x3016, 0x09);
    sensor_write_register (0x3018, 0x65);
    sensor_write_register (0x3019, 0x04);

    sensor_write_register (0x301c, 0xF6); /* HMAX */
    sensor_write_register (0x301d, 0x03); /* HMAX */

    sensor_write_register (0x3020, 0x02);
    sensor_write_register (0x3024, 0xc9);
    sensor_write_register (0x3030, 0x0b);
    sensor_write_register (0x3045, 0x05);
    sensor_write_register (0x3046, 0x00);
    sensor_write_register (0x304b, 0x0a);
    sensor_write_register (0x305c, 0x18);
    sensor_write_register (0x305d, 0x03);
    sensor_write_register (0x305e, 0x20);
    sensor_write_register (0x305f, 0x01);
    sensor_write_register (0x3070, 0x02);
    sensor_write_register (0x3071, 0x11);
    sensor_write_register (0x309b, 0x10);
    sensor_write_register (0x309c, 0x22);
    sensor_write_register (0x30a2, 0x02);
    sensor_write_register (0x30a6, 0x20);
    sensor_write_register (0x30a8, 0x20);
    sensor_write_register (0x30aa, 0x20);
    sensor_write_register (0x30ac, 0x20);
    sensor_write_register (0x30b0, 0x43);
    sensor_write_register (0x3106, 0x11);
    sensor_write_register (0x3119, 0x9e);
    sensor_write_register (0x311c, 0x1e);
    sensor_write_register (0x311e, 0x08);
    sensor_write_register (0x3128, 0x05);
    sensor_write_register (0x3129, 0x1d);
    sensor_write_register (0x313d, 0x83);
    sensor_write_register (0x3150, 0x03);
    sensor_write_register (0x315e, 0x1a);
    sensor_write_register (0x3164, 0x1a);
    sensor_write_register (0x317c, 0x12);
    sensor_write_register (0x317e, 0x00);
    sensor_write_register (0x31ec, 0x37);
                                         
    sensor_write_register (0x32b8, 0x50);
    sensor_write_register (0x32b9, 0x10);
    sensor_write_register (0x32ba, 0x00);
    sensor_write_register (0x32bb, 0x04);
    sensor_write_register (0x32c8, 0x50);
    sensor_write_register (0x32c9, 0x10);
    sensor_write_register (0x32ca, 0x00);
    sensor_write_register (0x32cb, 0x04);
                                        
    sensor_write_register (0x332c, 0xd3);
    sensor_write_register (0x332d, 0x10);
    sensor_write_register (0x332e, 0x0d);
    sensor_write_register (0x3358, 0x06);
    sensor_write_register (0x3359, 0xe1);
    sensor_write_register (0x335a, 0x11);
    sensor_write_register (0x3360, 0x1e);
    sensor_write_register (0x3361, 0x61);
    sensor_write_register (0x3362, 0x10);
    sensor_write_register (0x33b0, 0x50);
    sensor_write_register (0x33b2, 0x1a);
    sensor_write_register (0x33b3, 0x04);
                                         
    sensor_write_register (0x3405, 0x00);
    sensor_write_register (0x3407, 0x03);
    sensor_write_register (0x3414, 0x0a);
    sensor_write_register (0x3418, 0xb2);
    sensor_write_register (0x3419, 0x08);
    sensor_write_register (0x3441, 0x0a);
    sensor_write_register (0x3442, 0x0a);
    sensor_write_register (0x3443, 0x03);
    sensor_write_register (0x3444, 0x20);
    sensor_write_register (0x3445, 0x25);
    sensor_write_register (0x3446, 0x77);
    sensor_write_register (0x3447, 0x00);
    sensor_write_register (0x3448, 0x67);
    sensor_write_register (0x3449, 0x00);
    sensor_write_register (0x344a, 0x47);
    sensor_write_register (0x344b, 0x00);
    sensor_write_register (0x344c, 0x37);
    sensor_write_register (0x344d, 0x00);
    sensor_write_register (0x344e, 0x3f);
    sensor_write_register (0x344f, 0x00);
    sensor_write_register (0x3450, 0xff);
    sensor_write_register (0x3451, 0x00);
    sensor_write_register (0x3452, 0x3f);
    sensor_write_register (0x3453, 0x00);
    sensor_write_register (0x3454, 0x37);
    sensor_write_register (0x3455, 0x00);
    sensor_write_register (0x3472, 0x9c);
    sensor_write_register (0x3473, 0x07);
    sensor_write_register (0x3480, 0x49);

    sensor_write_register (0x3000, 0x00); /* standby */
    delay_ms(20); 
    sensor_write_register (0x3002, 0x00); /* master mode start */
 
    printf("===Imx290 sensor 1080P60fps 10bit 2to1 WDR(120fps->60fps) init success!=====\n");

    bSensorInit = HI_TRUE;

    return;
}

void sensor_wdr_1080p120_3to1_init()
{
    sensor_write_register (0x3000, 0x01); /* standby */
    sensor_write_register (0x3002, 0x01); /* XTMSTA */

    sensor_write_register (0x3005, 0x00);
    sensor_write_register (0x3007, 0x00);
    sensor_write_register (0x3009, 0x00);
    sensor_write_register (0x300a, 0x3c);
    sensor_write_register (0x300c, 0x21);
    sensor_write_register (0x300f, 0x00);
    sensor_write_register (0x3010, 0x21);
    sensor_write_register (0x3012, 0x64);
    sensor_write_register (0x3016, 0x09);
    sensor_write_register (0x3018, 0x65);
    sensor_write_register (0x3019, 0x04);
    sensor_write_register (0x301c, 0x4c);
    sensor_write_register (0x301d, 0x04);
    sensor_write_register (0x3020, 0x04);
    sensor_write_register (0x3024, 0x89);
    sensor_write_register (0x3028, 0x93);
    sensor_write_register (0x3029, 0x01);
    sensor_write_register (0x3030, 0x85);
    sensor_write_register (0x3034, 0x92);
    sensor_write_register (0x3045, 0x05);
    sensor_write_register (0x3046, 0x00);
    sensor_write_register (0x304b, 0x0a);
    sensor_write_register (0x305c, 0x18);
    sensor_write_register (0x305d, 0x03);
    sensor_write_register (0x305e, 0x20);
    sensor_write_register (0x305f, 0x01);
    sensor_write_register (0x3070, 0x02);
    sensor_write_register (0x3071, 0x11);
    sensor_write_register (0x309b, 0x10);
    sensor_write_register (0x309c, 0x22);
    sensor_write_register (0x30a2, 0x02);
    sensor_write_register (0x30a6, 0x20);
    sensor_write_register (0x30a8, 0x20);
    sensor_write_register (0x30aa, 0x20);
    sensor_write_register (0x30ac, 0x20);
    sensor_write_register (0x30b0, 0x43);
    sensor_write_register (0x3106, 0x33);
    sensor_write_register (0x3119, 0x9e);
    sensor_write_register (0x311c, 0x1e);
    sensor_write_register (0x311e, 0x08);
    sensor_write_register (0x3128, 0x05);
    sensor_write_register (0x3129, 0x1d);
    sensor_write_register (0x313d, 0x83);
    sensor_write_register (0x3150, 0x03);
    sensor_write_register (0x315e, 0x1a);
    sensor_write_register (0x3164, 0x1a);
    sensor_write_register (0x317c, 0x12);
    sensor_write_register (0x317e, 0x00);
    sensor_write_register (0x31ec, 0x37);
                                         
    sensor_write_register (0x32b8, 0x50);
    sensor_write_register (0x32b9, 0x10);
    sensor_write_register (0x32ba, 0x00);
    sensor_write_register (0x32bb, 0x04);
    sensor_write_register (0x32c8, 0x50);
    sensor_write_register (0x32c9, 0x10);
    sensor_write_register (0x32ca, 0x00);
    sensor_write_register (0x32cb, 0x04);
                                        
    sensor_write_register (0x332c, 0xd3);
    sensor_write_register (0x332d, 0x10);
    sensor_write_register (0x332e, 0x0d);
    sensor_write_register (0x3358, 0x06);
    sensor_write_register (0x3359, 0xe1);
    sensor_write_register (0x335a, 0x11);
    sensor_write_register (0x3360, 0x1e);
    sensor_write_register (0x3361, 0x61);
    sensor_write_register (0x3362, 0x10);
    sensor_write_register (0x33b0, 0x50);
    sensor_write_register (0x33b2, 0x1a);
    sensor_write_register (0x33b3, 0x04);
                                         
    sensor_write_register (0x3405, 0x00);
    sensor_write_register (0x3407, 0x03);
    sensor_write_register (0x3414, 0x0a);
    sensor_write_register (0x3418, 0x55);
    sensor_write_register (0x3419, 0x11);
    sensor_write_register (0x3441, 0x0a);
    sensor_write_register (0x3442, 0x0a);
    sensor_write_register (0x3443, 0x03);
    sensor_write_register (0x3444, 0x20);
    sensor_write_register (0x3445, 0x25);
    sensor_write_register (0x3446, 0x77);
    sensor_write_register (0x3447, 0x00);
    sensor_write_register (0x3448, 0x67);
    sensor_write_register (0x3449, 0x00);
    sensor_write_register (0x344a, 0x47);
    sensor_write_register (0x344b, 0x00);
    sensor_write_register (0x344c, 0x37);
    sensor_write_register (0x344d, 0x00);
    sensor_write_register (0x344e, 0x3f);
    sensor_write_register (0x344f, 0x00);
    sensor_write_register (0x3450, 0xff);
    sensor_write_register (0x3451, 0x00);
    sensor_write_register (0x3452, 0x3f);
    sensor_write_register (0x3453, 0x00);
    sensor_write_register (0x3454, 0x37);
    sensor_write_register (0x3455, 0x00);
    sensor_write_register (0x3472, 0x9c);
    sensor_write_register (0x3473, 0x07);
    sensor_write_register (0x3480, 0x49);

    sensor_write_register (0x3000, 0x00); /* standby */
    delay_ms(20); 
    sensor_write_register (0x3002, 0x00); /* master mode start */
 
    printf("===Imx290 sensor 1080P30fps 10bit 3to1 WDR(120fps->30fps) init success!=====\n");

    bSensorInit = HI_TRUE;

    return;
}


