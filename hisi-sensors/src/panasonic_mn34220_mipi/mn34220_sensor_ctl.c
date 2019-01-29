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

const unsigned char sensor_i2c_addr        =    0x6c;        /* I2C Address of MN34220 */
const unsigned int  sensor_addr_byte    =    2;
const unsigned int  sensor_data_byte    =    1;
static int g_fd = -1;

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

void sensor_wdr_init();
void sensor_linear_1080p30_init();
void sensor_linear_1080p60_init();
void sensor_linear_720p120_init();
void sensor_linear_VGA160_init();

void sensor_init()
{
    sensor_i2c_init();
    
    /* When sensor first init, config all registers */
    if (HI_FALSE == bSensorInit) 
    {
        if(WDR_MODE_2To1_LINE == genSensorMode)
        {
            sensor_wdr_init();
        }
        else
        {
            if (1 == gu8SensorImageMode)    /* SENSOR_1080P_60FPS_MODE */
            {
                sensor_linear_1080p60_init(); 
            }
            else if(2 == gu8SensorImageMode)                          /* SENSOR_1080P_30FPS_MODE */
            {
                sensor_linear_1080p30_init();
            }
            else if(3 ==gu8SensorImageMode)
            {
                sensor_linear_720p120_init();
            }
            else if(4 ==gu8SensorImageMode)
            {
                sensor_linear_VGA160_init();
            }
            else
            {
            }
        }
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else 
    {
        if(WDR_MODE_2To1_LINE == genSensorMode)
        {
            sensor_wdr_init();
        }
        else
        {
            if (1 == gu8SensorImageMode)    /* SENSOR_1080P_60FPS_MODE */
            {
                sensor_linear_1080p60_init(); 
            }
            else if(2 == gu8SensorImageMode)/* SENSOR_1080P_30FPS_MODE */
            {
                sensor_linear_1080p30_init();
            }
            else if(3 ==gu8SensorImageMode)
            {
                sensor_linear_720p120_init();
            }
            else if(4 ==gu8SensorImageMode)
            {
                sensor_linear_VGA160_init();
            }
            else
            {
            }
        }       
    }

    return ;
}

void sensor_exit()
{
    sensor_i2c_exit();

    return;
}

/* 1080P60 and 1080P50 */
void sensor_linear_1080p60_init()
{
    /* # N112M_S12_P4_FHD_V1125_12b_445MHz_60fps_vM17e_140904_MCLK37_Mst_I2C_d.txt# VCYCLE:1125 HCYCLE:550 (@MCLK) */
    sensor_write_register (0x3022, 0x00);
    sensor_write_register (0x3023, 0x33);
    sensor_write_register (0x300E, 0x01);
    sensor_write_register (0x300F, 0x00);
    sensor_write_register (0x0304, 0x00);
    sensor_write_register (0x0305, 0x02);
    sensor_write_register (0x0306, 0x00);
    sensor_write_register (0x0307, 0x24);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0x03);
    sensor_write_register (0x0112, 0x0C);
    sensor_write_register (0x0113, 0x0C);
    sensor_write_register (0x3005, 0x64);
    sensor_write_register (0x3007, 0x10);
    sensor_write_register (0x3009, 0x0E);
    sensor_write_register (0x300A, 0xFF);
    sensor_write_register (0x300B, 0x00);
    sensor_write_register (0x3018, 0x43);
    sensor_write_register (0x3019, 0x10);
    sensor_write_register (0x301A, 0xB9);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0x53);
    sensor_write_register (0x300E, 0x00);
    sensor_write_register (0x300F, 0x00);
    #if 0
    sensor_write_register (0x0202, 0x04);
    sensor_write_register (0x0203, 0x63);
    
    sensor_write_register (0x0204, 0x01);
    sensor_write_register (0x0205, 0xC0);
    #else
    sensor_write_register (0x0202, 0x00);
    sensor_write_register (0x0203, 0x02);

    sensor_write_register (0x0204, 0x01);
    sensor_write_register (0x0205, 0x00);
    #endif
    sensor_write_register (0x3036, 0x00);
    sensor_write_register (0x3039, 0x2E);
    sensor_write_register (0x0340, 0x04);
    sensor_write_register (0x0341, 0x65);
    sensor_write_register (0x0342, 0x08);
    sensor_write_register (0x0343, 0x98);
    sensor_write_register (0x0346, 0x00);
    sensor_write_register (0x0347, 0x3C);
    sensor_write_register (0x034A, 0x04);
    sensor_write_register (0x034B, 0x7F);
    sensor_write_register (0x034E, 0x04);
    sensor_write_register (0x034F, 0x44);
    sensor_write_register (0x3031, 0x00);
    sensor_write_register (0x3032, 0x00);
    sensor_write_register (0x3036, 0x00);
    sensor_write_register (0x3039, 0x2E);
    sensor_write_register (0x3041, 0x2C);
    sensor_write_register (0x3058, 0x0F);
    sensor_write_register (0x3059, 0xFF);
    sensor_write_register (0x305B, 0x00);
    sensor_write_register (0x3062, 0x10);
    sensor_write_register (0x3063, 0x24);
    sensor_write_register (0x306E, 0x0C);
    sensor_write_register (0x306F, 0x00);
    sensor_write_register (0x3074, 0x01);
    sensor_write_register (0x3076, 0x40);
    sensor_write_register (0x307C, 0x2C);
    sensor_write_register (0x3085, 0x33);
    sensor_write_register (0x3087, 0x03);
    sensor_write_register (0x3088, 0x0E);
    sensor_write_register (0x3089, 0x00);
    sensor_write_register (0x308B, 0x00);
    sensor_write_register (0x308C, 0x06);
    sensor_write_register (0x308D, 0x03);
    sensor_write_register (0x308E, 0x03);
    sensor_write_register (0x308F, 0x06);
    sensor_write_register (0x3090, 0x04);
    sensor_write_register (0x3094, 0x0B);
    sensor_write_register (0x3095, 0x76);
    sensor_write_register (0x3098, 0x00);
    sensor_write_register (0x3099, 0x00);
    sensor_write_register (0x309A, 0x01);
    sensor_write_register (0x3101, 0x00);
    sensor_write_register (0x3104, 0x04);
    sensor_write_register (0x3106, 0x00);
    sensor_write_register (0x3107, 0xC0);
    sensor_write_register (0x312B, 0x00);
    sensor_write_register (0x312D, 0x00);
    sensor_write_register (0x312F, 0x00);
    sensor_write_register (0x3141, 0x40);
    sensor_write_register (0x3143, 0x02);
    sensor_write_register (0x3144, 0x02);
    sensor_write_register (0x3145, 0x02);
    sensor_write_register (0x3146, 0x00);
    sensor_write_register (0x3147, 0x02);
    sensor_write_register (0x3148, 0x02);
    sensor_write_register (0x3149, 0x02);
    sensor_write_register (0x314A, 0x01);
    sensor_write_register (0x314B, 0x02);
    sensor_write_register (0x314C, 0x02);
    sensor_write_register (0x314D, 0x02);
    sensor_write_register (0x314E, 0x01);
    sensor_write_register (0x314F, 0x02);
    sensor_write_register (0x3150, 0x02);
    sensor_write_register (0x3152, 0x04);
    sensor_write_register (0x3153, 0xE3);
    sensor_write_register (0x3155, 0xCA);
    sensor_write_register (0x3157, 0xCA);
    sensor_write_register (0x3159, 0xCA);
    sensor_write_register (0x315B, 0xCA);
    sensor_write_register (0x315D, 0xCA);
    sensor_write_register (0x315F, 0xCA);
    sensor_write_register (0x3161, 0xCA);
    sensor_write_register (0x3163, 0xCA);
    sensor_write_register (0x3165, 0xCA);
    sensor_write_register (0x3167, 0xCA);
    sensor_write_register (0x3169, 0xCA);
    sensor_write_register (0x316B, 0xCA);
    sensor_write_register (0x316D, 0xCA);
    sensor_write_register (0x316F, 0xC6);
    sensor_write_register (0x3171, 0xCA);
    sensor_write_register (0x3173, 0xCA);
    sensor_write_register (0x3175, 0x80);
    sensor_write_register (0x318E, 0x20);
    sensor_write_register (0x318F, 0x70);
    sensor_write_register (0x3196, 0x08);
    sensor_write_register (0x31FC, 0x02);
    sensor_write_register (0x31FE, 0x07);
    sensor_write_register (0x3211, 0x14);
    sensor_write_register (0x323A, 0x80);
    sensor_write_register (0x323B, 0x91);
    sensor_write_register (0x323C, 0x71);
    sensor_write_register (0x323D, 0x90);
    sensor_write_register (0x323E, 0x01);
    sensor_write_register (0x3243, 0xD7);
    sensor_write_register (0x3247, 0x79);
    sensor_write_register (0x3248, 0x00);
    sensor_write_register (0x3249, 0x00);
    sensor_write_register (0x324A, 0x30);
    sensor_write_register (0x324B, 0x18);
    sensor_write_register (0x324C, 0x02);
    sensor_write_register (0x3253, 0xDE);
    sensor_write_register (0x3256, 0x11);
    sensor_write_register (0x3259, 0x49);
    sensor_write_register (0x325A, 0x39);
    sensor_write_register (0x325E, 0x84);
    sensor_write_register (0x3272, 0x46);
    sensor_write_register (0x3273, 0xAB);
    sensor_write_register (0x3280, 0x30);
    sensor_write_register (0x3285, 0x1B);
    sensor_write_register (0x3288, 0x01);
    sensor_write_register (0x328A, 0xE8);
    sensor_write_register (0x330E, 0x05);
    sensor_write_register (0x3310, 0x02);
    sensor_write_register (0x3315, 0x1F);
    sensor_write_register (0x331A, 0x02);
    sensor_write_register (0x331B, 0x02);
    sensor_write_register (0x332C, 0x02);
    sensor_write_register (0x3339, 0x02);
    sensor_write_register (0x336B, 0x03);
    sensor_write_register (0x339F, 0x03);
    sensor_write_register (0x33A2, 0x03);
    sensor_write_register (0x33A3, 0x03);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0xD3);
    sensor_write_register (0x0100, 0x01);
    sensor_write_register (0x0101, 0x00);
    
    printf("===panasonic mn34220 sensor 1080P60fps linear mode(MIPI port) init success!=====\n");
    
    bSensorInit = HI_TRUE;

    return;
}


/* 1080P30 and 1080P25 */
void sensor_linear_1080p30_init()
{
    /* # N111M_S12_P4_FHD_V1125_12b_222MHz_30fps_vM17e_140904_MCLK37_Mst_I2C_d.txt# VCYCLE:1125 HCYCLE:1100 (@MCLK) */
    sensor_write_register (0x3022, 0x00);
    sensor_write_register (0x3023, 0x33);
    sensor_write_register (0x300E, 0x01);
    sensor_write_register (0x300F, 0x00);
    sensor_write_register (0x0304, 0x00);
    sensor_write_register (0x0305, 0x02);
    sensor_write_register (0x0306, 0x00);
    sensor_write_register (0x0307, 0x24);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0x03);
    sensor_write_register (0x0112, 0x0C);
    sensor_write_register (0x0113, 0x0C);
    sensor_write_register (0x3005, 0x64);
    sensor_write_register (0x3007, 0x14);
    sensor_write_register (0x3009, 0x0E);
    sensor_write_register (0x300A, 0xFF);
    sensor_write_register (0x300B, 0x00);
    sensor_write_register (0x3018, 0x43);
    sensor_write_register (0x3019, 0x10);
    sensor_write_register (0x301A, 0xB9);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0x53);
    sensor_write_register (0x300E, 0x00);
    sensor_write_register (0x300F, 0x00);

    #if 0
    sensor_write_register (0x0202, 0x04);
    sensor_write_register (0x0203, 0x63);
    
    sensor_write_register (0x0204, 0x01);
    sensor_write_register (0x0205, 0xC0);
    #else
    sensor_write_register (0x0202, 0x00);
    sensor_write_register (0x0203, 0x02);

    sensor_write_register (0x0204, 0x01);
    sensor_write_register (0x0205, 0x00);
    #endif
    
    sensor_write_register (0x3036, 0x00);
    sensor_write_register (0x3039, 0x2E);
    sensor_write_register (0x0340, 0x04);
    sensor_write_register (0x0341, 0x65);
    sensor_write_register (0x0342, 0x08);
    sensor_write_register (0x0343, 0x98);
    sensor_write_register (0x0346, 0x00);
    sensor_write_register (0x0347, 0x3C);
    sensor_write_register (0x034A, 0x04);
    sensor_write_register (0x034B, 0x7F);
    sensor_write_register (0x034E, 0x04);
    sensor_write_register (0x034F, 0x44);
    sensor_write_register (0x3031, 0x00);
    sensor_write_register (0x3032, 0x00);
    sensor_write_register (0x3036, 0x00);
    sensor_write_register (0x3039, 0x2E);
    sensor_write_register (0x3041, 0x2C);
    sensor_write_register (0x3058, 0x0F);
    sensor_write_register (0x3059, 0xFF);
    sensor_write_register (0x305B, 0x00);
    sensor_write_register (0x3062, 0x10);
    sensor_write_register (0x3063, 0x24);
    sensor_write_register (0x306E, 0x0C);
    sensor_write_register (0x306F, 0x00);
    sensor_write_register (0x3074, 0x01);
    sensor_write_register (0x3076, 0x40);
    sensor_write_register (0x307C, 0x2C);
    sensor_write_register (0x3085, 0x33);
    sensor_write_register (0x3087, 0x01);
    sensor_write_register (0x3088, 0x07);
    sensor_write_register (0x3089, 0x02);
    sensor_write_register (0x308B, 0x09);
    sensor_write_register (0x308C, 0x03);
    sensor_write_register (0x308D, 0x02);
    sensor_write_register (0x308E, 0x02);
    sensor_write_register (0x308F, 0x02);
    sensor_write_register (0x3090, 0x02);
    sensor_write_register (0x3094, 0x0B);
    sensor_write_register (0x3095, 0x76);
    sensor_write_register (0x3098, 0x00);
    sensor_write_register (0x3099, 0x00);
    sensor_write_register (0x309A, 0x01);
    sensor_write_register (0x3101, 0x00);
    sensor_write_register (0x3104, 0x04);
    sensor_write_register (0x3106, 0x00);
    sensor_write_register (0x3107, 0xC0);
    sensor_write_register (0x312B, 0x00);
    sensor_write_register (0x312D, 0x00);
    sensor_write_register (0x312F, 0x00);
    sensor_write_register (0x3141, 0x40);
    sensor_write_register (0x3143, 0x02);
    sensor_write_register (0x3144, 0x02);
    sensor_write_register (0x3145, 0x02);
    sensor_write_register (0x3146, 0x00);
    sensor_write_register (0x3147, 0x02);
    sensor_write_register (0x3148, 0x02);
    sensor_write_register (0x3149, 0x02);
    sensor_write_register (0x314A, 0x01);
    sensor_write_register (0x314B, 0x02);
    sensor_write_register (0x314C, 0x02);
    sensor_write_register (0x314D, 0x02);
    sensor_write_register (0x314E, 0x01);
    sensor_write_register (0x314F, 0x02);
    sensor_write_register (0x3150, 0x02);
    sensor_write_register (0x3152, 0x04);
    sensor_write_register (0x3153, 0xE3);
    sensor_write_register (0x3155, 0xCA);
    sensor_write_register (0x3157, 0xCA);
    sensor_write_register (0x3159, 0xCA);
    sensor_write_register (0x315B, 0xCA);
    sensor_write_register (0x315D, 0xCA);
    sensor_write_register (0x315F, 0xCA);
    sensor_write_register (0x3161, 0xCA);
    sensor_write_register (0x3163, 0xCA);
    sensor_write_register (0x3165, 0xCA);
    sensor_write_register (0x3167, 0xCA);
    sensor_write_register (0x3169, 0xCA);
    sensor_write_register (0x316B, 0xCA);
    sensor_write_register (0x316D, 0xCA);
    sensor_write_register (0x316F, 0xC6);
    sensor_write_register (0x3171, 0xCA);
    sensor_write_register (0x3173, 0xCA);
    sensor_write_register (0x3175, 0x80);
    sensor_write_register (0x318E, 0x20);
    sensor_write_register (0x318F, 0x70);
    sensor_write_register (0x3196, 0x08);
    sensor_write_register (0x31FC, 0x02);
    sensor_write_register (0x31FE, 0x07);
    sensor_write_register (0x3211, 0x14);
    sensor_write_register (0x323A, 0x80);
    sensor_write_register (0x323B, 0x91);
    sensor_write_register (0x323C, 0x71);
    sensor_write_register (0x323D, 0x90);
    sensor_write_register (0x323E, 0x01);
    sensor_write_register (0x3243, 0xD7);
    sensor_write_register (0x3247, 0x38);
    sensor_write_register (0x3248, 0x03);
    sensor_write_register (0x3249, 0xE2);
    sensor_write_register (0x324A, 0x30);
    sensor_write_register (0x324B, 0x18);
    sensor_write_register (0x324C, 0x02);
    sensor_write_register (0x3253, 0xDE);
    sensor_write_register (0x3256, 0x11);
    sensor_write_register (0x3259, 0x68);
    sensor_write_register (0x325A, 0x39);
    sensor_write_register (0x325E, 0x84);
    sensor_write_register (0x3272, 0x46);
    sensor_write_register (0x3273, 0xAB);
    sensor_write_register (0x3280, 0x30);
    sensor_write_register (0x3285, 0x1B);
    sensor_write_register (0x3288, 0x01);
    sensor_write_register (0x328A, 0xE8);
    sensor_write_register (0x330E, 0x05);
    sensor_write_register (0x3310, 0x02);
    sensor_write_register (0x3315, 0x1F);
    sensor_write_register (0x331A, 0x02);
    sensor_write_register (0x331B, 0x02);
    sensor_write_register (0x332C, 0x02);
    sensor_write_register (0x3339, 0x02);
    sensor_write_register (0x336B, 0x03);
    sensor_write_register (0x339F, 0x03);
    sensor_write_register (0x33A2, 0x03);
    sensor_write_register (0x33A3, 0x03);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0xD3);
    sensor_write_register (0x0100, 0x01);
    sensor_write_register (0x0101, 0x00);
    
    bSensorInit = HI_TRUE;
    printf("=========================================================\n");
    printf("===panasonic mn34220 sensor 1080P30fps(MIPI port) init success!=====\n");
    printf("=========================================================\n");
    
    return;
}

void sensor_linear_720p120_init()
{
    /*
    # N531M_S12_P4_HD_V750_12b_594MHz_MCLK37_119fps_vM17e_141127_Mst_I2C_d.txt
    # VCYCLE:750 HCYCLE:414 (@MCLK)
    */
    
    sensor_write_register (0x3022, 0x00);
    sensor_write_register (0x3023, 0x33);
    sensor_write_register (0x300E, 0x01);
    sensor_write_register (0x300F, 0x00);
    sensor_write_register (0x0304, 0x00);
    sensor_write_register (0x0305, 0x02);
    sensor_write_register (0x0306, 0x00);
    sensor_write_register (0x0307, 0x30);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0x03);
    sensor_write_register (0x0112, 0x0C);
    sensor_write_register (0x0113, 0x0C);
    sensor_write_register (0x3005, 0x67);
    sensor_write_register (0x3007, 0x10);
    sensor_write_register (0x3009, 0x0E);
    sensor_write_register (0x300A, 0xFF);
    sensor_write_register (0x300B, 0x00);
    sensor_write_register (0x3018, 0x43);
    sensor_write_register (0x3019, 0x10);
    sensor_write_register (0x301A, 0xB9);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0x53);
    sensor_write_register (0x300E, 0x00);
    sensor_write_register (0x300F, 0x00);
#if 0
    sensor_write_register (0x0202, 0x02);
    sensor_write_register (0x0203, 0xEC);
    
    sensor_write_register (0x0204, 0x01);
    sensor_write_register (0x0205, 0xC0);
#else
    sensor_write_register (0x0202, 0x00);
    sensor_write_register (0x0203, 0x02);

    sensor_write_register (0x0204, 0x01);
    sensor_write_register (0x0205, 0x00);
#endif
    
    sensor_write_register (0x3036, 0x00);
    sensor_write_register (0x3039, 0x2E);
    sensor_write_register (0x0340, 0x02);
    sensor_write_register (0x0341, 0xEE);
    sensor_write_register (0x0342, 0x08);
    sensor_write_register (0x0343, 0xA0);
    sensor_write_register (0x0346, 0x00);
    sensor_write_register (0x0347, 0xF2);
    sensor_write_register (0x034A, 0x03);
    sensor_write_register (0x034B, 0xC9);
    sensor_write_register (0x034E, 0x02);
    sensor_write_register (0x034F, 0xD8);
    sensor_write_register (0x3031, 0x02);
    sensor_write_register (0x3032, 0x02);
    sensor_write_register (0x3036, 0x00);
    sensor_write_register (0x3039, 0x2E);
    sensor_write_register (0x3041, 0x12);
    sensor_write_register (0x3058, 0x0F);
    sensor_write_register (0x3059, 0xFF);
    sensor_write_register (0x305B, 0x00);
    sensor_write_register (0x3062, 0x10);
    sensor_write_register (0x3063, 0x24);
    sensor_write_register (0x306E, 0x0C);
    sensor_write_register (0x306F, 0x00);
    sensor_write_register (0x3074, 0x01);
    sensor_write_register (0x3076, 0x40);
    sensor_write_register (0x307C, 0x2C);
    sensor_write_register (0x3085, 0x33);
    sensor_write_register (0x3087, 0x03);
    sensor_write_register (0x3088, 0x14);
    sensor_write_register (0x3089, 0x04);
    sensor_write_register (0x308B, 0x0B);
    sensor_write_register (0x308C, 0x08);
    sensor_write_register (0x308D, 0x04);
    sensor_write_register (0x308E, 0x04);
    sensor_write_register (0x308F, 0x09);
    sensor_write_register (0x3090, 0x05);
    sensor_write_register (0x3094, 0x09);
    sensor_write_register (0x3095, 0x90);
    sensor_write_register (0x3098, 0x00);
    sensor_write_register (0x3099, 0x40);
    sensor_write_register (0x309A, 0x10);
    sensor_write_register (0x3101, 0x00);
    sensor_write_register (0x3104, 0x04);
    sensor_write_register (0x3106, 0x00);
    sensor_write_register (0x3107, 0xC0);
    sensor_write_register (0x312B, 0x00);
    sensor_write_register (0x312D, 0x00);
    sensor_write_register (0x312F, 0x00);
    sensor_write_register (0x3141, 0x70);
    sensor_write_register (0x3143, 0x01);
    sensor_write_register (0x3144, 0x03);
    sensor_write_register (0x3145, 0x02);
    sensor_write_register (0x3146, 0x05);
    sensor_write_register (0x3147, 0x00);
    sensor_write_register (0x3148, 0x00);
    sensor_write_register (0x3149, 0x00);
    sensor_write_register (0x314A, 0x03);
    sensor_write_register (0x314B, 0x01);
    sensor_write_register (0x314C, 0x01);
    sensor_write_register (0x314D, 0x01);
    sensor_write_register (0x314E, 0x02);
    sensor_write_register (0x314F, 0x02);
    sensor_write_register (0x3150, 0x02);
    sensor_write_register (0x3152, 0x01);
    sensor_write_register (0x3153, 0xE3);
    sensor_write_register (0x3155, 0x11);
    sensor_write_register (0x3157, 0x30);
    sensor_write_register (0x3159, 0x33);
    sensor_write_register (0x315B, 0x36);
    sensor_write_register (0x315D, 0x35);
    sensor_write_register (0x315F, 0x3C);
    sensor_write_register (0x3161, 0x3F);
    sensor_write_register (0x3163, 0x3A);
    sensor_write_register (0x3165, 0x39);
    sensor_write_register (0x3167, 0x28);
    sensor_write_register (0x3169, 0x2B);
    sensor_write_register (0x316B, 0x2E);
    sensor_write_register (0x316D, 0x2D);
    sensor_write_register (0x316F, 0x22);
    sensor_write_register (0x3171, 0x22);
    sensor_write_register (0x3173, 0x61);
    sensor_write_register (0x3175, 0x80);
    sensor_write_register (0x318E, 0x20);
    sensor_write_register (0x318F, 0x70);
    sensor_write_register (0x3196, 0x08);
    sensor_write_register (0x31FC, 0x03);
    sensor_write_register (0x31FE, 0x06);
    sensor_write_register (0x3211, 0x0C);
    sensor_write_register (0x323A, 0x80);
    sensor_write_register (0x323B, 0x91);
    sensor_write_register (0x323C, 0x71);
    sensor_write_register (0x323D, 0x90);
    sensor_write_register (0x323E, 0x01);
    sensor_write_register (0x3243, 0x75);
    sensor_write_register (0x3247, 0xA5);
    sensor_write_register (0x3248, 0x00);
    sensor_write_register (0x3249, 0x00);
    sensor_write_register (0x324A, 0x30);
    sensor_write_register (0x324B, 0x1B);
    sensor_write_register (0x324C, 0x02);
    sensor_write_register (0x3253, 0x7B);
    sensor_write_register (0x3256, 0x32);
    sensor_write_register (0x3259, 0x9A);
    sensor_write_register (0x325A, 0x14);
    sensor_write_register (0x325E, 0xB8);
    sensor_write_register (0x3272, 0x0C);
    sensor_write_register (0x3273, 0xB8);
    sensor_write_register (0x3280, 0x30);
    sensor_write_register (0x3285, 0x19);
    sensor_write_register (0x3288, 0x01);
    sensor_write_register (0x328A, 0x62);
    sensor_write_register (0x330E, 0x05);
    sensor_write_register (0x3310, 0x02);
    sensor_write_register (0x3315, 0x1F);
    sensor_write_register (0x331A, 0x03);
    sensor_write_register (0x331B, 0x03);
    sensor_write_register (0x332C, 0x00);
    sensor_write_register (0x3339, 0x03);
    sensor_write_register (0x336B, 0x02);
    sensor_write_register (0x339F, 0x01);
    sensor_write_register (0x33A2, 0x01);
    sensor_write_register (0x33A3, 0x01);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0xD3);
    sensor_write_register (0x0100, 0x01);
    sensor_write_register (0x0101, 0x00);
    
    bSensorInit = HI_TRUE;
    printf("=========================================================\n");
    printf("===panasonic mn34220 sensor 720P120fps(MIPI port) init success!=====\n");
    printf("=========================================================\n");
    
    return;
}

void sensor_linear_VGA160_init()
{
    
    /*BATCH
    # N571M_S12_P4_VGA_V562_12b_594MHz_160fps_vM17e_141208_Mst_I2C_d.txt
    # VCYCLE:562 HCYCLE:300 (@MCLK)*/
    
    sensor_write_register (0x3022, 0x00);
    sensor_write_register (0x3023, 0x33);
    sensor_write_register (0x300E, 0x01);
    sensor_write_register (0x300F, 0x00);
    sensor_write_register (0x0304, 0x00);
    sensor_write_register (0x0305, 0x02);
    sensor_write_register (0x0306, 0x00);
    sensor_write_register (0x0307, 0x30);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0x03);
    sensor_write_register (0x0112, 0x0C);
    sensor_write_register (0x0113, 0x0C);
    sensor_write_register (0x3005, 0x67);
    sensor_write_register (0x3007, 0x10);
    sensor_write_register (0x3009, 0x0E);
    sensor_write_register (0x300A, 0xFF);
    sensor_write_register (0x300B, 0x00);
    sensor_write_register (0x3018, 0x43);
    sensor_write_register (0x3019, 0x10);
    sensor_write_register (0x301A, 0xB9);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0x53);
    sensor_write_register (0x300E, 0x00);
    sensor_write_register (0x300F, 0x00);
#if 0
    sensor_write_register (0x0202, 0x02);
    sensor_write_register (0x0203, 0x30);
    
    sensor_write_register (0x0204, 0x01);
    sensor_write_register (0x0205, 0xC0);
#else
    sensor_write_register (0x0202, 0x00);
    sensor_write_register (0x0203, 0x02);

    sensor_write_register (0x0204, 0x01);
    sensor_write_register (0x0205, 0x00);
#endif
    
    sensor_write_register (0x3036, 0x00);
    sensor_write_register (0x3039, 0x2E);
    sensor_write_register (0x0340, 0x02);
    sensor_write_register (0x0341, 0x32);
    sensor_write_register (0x0342, 0x08);
    sensor_write_register (0x0343, 0xA0);
    sensor_write_register (0x0346, 0x01);
    sensor_write_register (0x0347, 0x68);
    sensor_write_register (0x034A, 0x03);
    sensor_write_register (0x034B, 0x53);
    sensor_write_register (0x034E, 0x01);
    sensor_write_register (0x034F, 0xEC);
    sensor_write_register (0x3031, 0x02);
    sensor_write_register (0x3032, 0x02);
    sensor_write_register (0x3036, 0x00);
    sensor_write_register (0x3039, 0x2E);
    sensor_write_register (0x3041, 0x12);
    sensor_write_register (0x3058, 0x0F);
    sensor_write_register (0x3059, 0xFF);
    sensor_write_register (0x305B, 0x00);
    sensor_write_register (0x3062, 0x10);
    sensor_write_register (0x3063, 0x24);
    sensor_write_register (0x306E, 0x0C);
    sensor_write_register (0x306F, 0x00);
    sensor_write_register (0x3074, 0x01);
    sensor_write_register (0x3076, 0x40);
    sensor_write_register (0x307C, 0x2C);
    sensor_write_register (0x3085, 0x33);
    sensor_write_register (0x3087, 0x03);
    sensor_write_register (0x3088, 0x14);
    sensor_write_register (0x3089, 0x04);
    sensor_write_register (0x308B, 0x0B);
    sensor_write_register (0x308C, 0x08);
    sensor_write_register (0x308D, 0x04);
    sensor_write_register (0x308E, 0x04);
    sensor_write_register (0x308F, 0x09);
    sensor_write_register (0x3090, 0x05);
    sensor_write_register (0x3094, 0x09);
    sensor_write_register (0x3095, 0x90);
    sensor_write_register (0x3098, 0x00);
    sensor_write_register (0x3099, 0x40);
    sensor_write_register (0x309A, 0x10);
    sensor_write_register (0x3101, 0x00);
    sensor_write_register (0x3104, 0x04);
    sensor_write_register (0x3106, 0x00);
    sensor_write_register (0x3107, 0xC0);
    sensor_write_register (0x312B, 0x00);
    sensor_write_register (0x312D, 0x00);
    sensor_write_register (0x312F, 0x00);
    sensor_write_register (0x3141, 0x70);
    sensor_write_register (0x3143, 0x01);
    sensor_write_register (0x3144, 0x03);
    sensor_write_register (0x3145, 0x02);
    sensor_write_register (0x3146, 0x05);
    sensor_write_register (0x3147, 0x00);
    sensor_write_register (0x3148, 0x00);
    sensor_write_register (0x3149, 0x00);
    sensor_write_register (0x314A, 0x03);
    sensor_write_register (0x314B, 0x01);
    sensor_write_register (0x314C, 0x01);
    sensor_write_register (0x314D, 0x01);
    sensor_write_register (0x314E, 0x02);
    sensor_write_register (0x314F, 0x02);
    sensor_write_register (0x3150, 0x02);
    sensor_write_register (0x3152, 0x01);
    sensor_write_register (0x3153, 0xE3);
    sensor_write_register (0x3155, 0x11);
    sensor_write_register (0x3157, 0x30);
    sensor_write_register (0x3159, 0x33);
    sensor_write_register (0x315B, 0x36);
    sensor_write_register (0x315D, 0x35);
    sensor_write_register (0x315F, 0x3C);
    sensor_write_register (0x3161, 0x3F);
    sensor_write_register (0x3163, 0x3A);
    sensor_write_register (0x3165, 0x39);
    sensor_write_register (0x3167, 0x28);
    sensor_write_register (0x3169, 0x2B);
    sensor_write_register (0x316B, 0x2E);
    sensor_write_register (0x316D, 0x2D);
    sensor_write_register (0x316F, 0x22);
    sensor_write_register (0x3171, 0x22);
    sensor_write_register (0x3173, 0x61);
    sensor_write_register (0x3175, 0x80);
    sensor_write_register (0x318E, 0x20);
    sensor_write_register (0x318F, 0x70);
    sensor_write_register (0x3196, 0x08);
    sensor_write_register (0x31FC, 0x03);
    sensor_write_register (0x31FE, 0x06);
    sensor_write_register (0x3211, 0x0C);
    sensor_write_register (0x323A, 0x80);
    sensor_write_register (0x323B, 0x91);
    sensor_write_register (0x323C, 0x71);
    sensor_write_register (0x323D, 0x90);
    sensor_write_register (0x323E, 0x01);
    sensor_write_register (0x3243, 0x75);
    sensor_write_register (0x3247, 0xA5);
    sensor_write_register (0x3248, 0x00);
    sensor_write_register (0x3249, 0x00);
    sensor_write_register (0x324A, 0x30);
    sensor_write_register (0x324B, 0x1B);
    sensor_write_register (0x324C, 0x02);
    sensor_write_register (0x3253, 0x7B);
    sensor_write_register (0x3256, 0x32);
    sensor_write_register (0x3259, 0x9A);
    sensor_write_register (0x325A, 0x14);
    sensor_write_register (0x325E, 0xB8);
    sensor_write_register (0x3272, 0x0C);
    sensor_write_register (0x3273, 0xB8);
    sensor_write_register (0x3280, 0x30);
    sensor_write_register (0x3285, 0x19);
    sensor_write_register (0x3288, 0x01);
    sensor_write_register (0x328A, 0x62);
    sensor_write_register (0x330E, 0x05);
    sensor_write_register (0x3310, 0x02);
    sensor_write_register (0x3315, 0x1F);
    sensor_write_register (0x331A, 0x03);
    sensor_write_register (0x331B, 0x03);
    sensor_write_register (0x332C, 0x00);
    sensor_write_register (0x3339, 0x03);
    sensor_write_register (0x336B, 0x02);
    sensor_write_register (0x339F, 0x01);
    sensor_write_register (0x33A2, 0x01);
    sensor_write_register (0x33A3, 0x01);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0xD3);
    sensor_write_register (0x0100, 0x01);
    sensor_write_register (0x0101, 0x00);

    bSensorInit = HI_TRUE;
    printf("=========================================================\n");
    printf("===panasonic mn34220 sensor VGA 160fps(MIPI port) init success!=====\n");
    printf("=========================================================\n");
    
    return;
}


void sensor_wdr_init()
{
    /*
    # N035M_S12_P4_FHD_WDRx2_V1250_12b_594MHz_30fps_vM17e_141002_37.125MHz_Mst_I2C_d.txt
    # VCYCLE:1250 HCYCLE:990 (@MCLK)
    */
    sensor_write_register (0x3022, 0x00);
    sensor_write_register (0x3023, 0x33);
    sensor_write_register (0x300E, 0x01);
    sensor_write_register (0x300F, 0x00);
    sensor_write_register (0x0304, 0x00);
    sensor_write_register (0x0305, 0x02);
    sensor_write_register (0x0306, 0x00);
    sensor_write_register (0x0307, 0x30);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0x03);
    sensor_write_register (0x0112, 0x0C);
    sensor_write_register (0x0113, 0x0C);
    sensor_write_register (0x3005, 0x67);
    sensor_write_register (0x3007, 0x10);
    sensor_write_register (0x3009, 0x0E);
    sensor_write_register (0x300A, 0xFF);
    sensor_write_register (0x300B, 0x00);
    sensor_write_register (0x3018, 0x43);
    sensor_write_register (0x3019, 0x10);
    sensor_write_register (0x301A, 0xB9);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0x53);
    sensor_write_register (0x300E, 0x00);
    sensor_write_register (0x300F, 0x00);
    sensor_write_register (0x0202, 0x00);
    sensor_write_register (0x0203, 0x20);
    sensor_write_register (0x3036, 0x00);
    sensor_write_register (0x3039, 0x2E);
    sensor_write_register (0x0340, 0x04);
    sensor_write_register (0x0341, 0xE2);
    sensor_write_register (0x0342, 0x14);
    sensor_write_register (0x0343, 0xA0);
    sensor_write_register (0x0346, 0x00);
    sensor_write_register (0x0347, 0x3C);
    sensor_write_register (0x034A, 0x04);
    sensor_write_register (0x034B, 0x7F);
    sensor_write_register (0x034E, 0x04);
    sensor_write_register (0x034F, 0x44);
    sensor_write_register (0x3031, 0x00);
    sensor_write_register (0x3032, 0x00);
    sensor_write_register (0x3036, 0x00);
    sensor_write_register (0x3039, 0x2E);
    sensor_write_register (0x3041, 0x2C);
    sensor_write_register (0x3058, 0x0F);
    sensor_write_register (0x3059, 0xFF);
    sensor_write_register (0x305B, 0x00);
    sensor_write_register (0x3062, 0x10);
    sensor_write_register (0x3063, 0x24);
    sensor_write_register (0x306E, 0x0C);
    sensor_write_register (0x306F, 0x00);
    sensor_write_register (0x3074, 0x01);
    sensor_write_register (0x3076, 0x40);
    sensor_write_register (0x307C, 0x2C);
    sensor_write_register (0x3085, 0x33);
    sensor_write_register (0x3087, 0x03);
    sensor_write_register (0x3088, 0x14);
    sensor_write_register (0x3089, 0x04);
    sensor_write_register (0x308B, 0x0B);
    sensor_write_register (0x308C, 0x08);
    sensor_write_register (0x308D, 0x04);
    sensor_write_register (0x308E, 0x04);
    sensor_write_register (0x308F, 0x09);
    sensor_write_register (0x3090, 0x05);
    sensor_write_register (0x3094, 0x0B);
    sensor_write_register (0x3095, 0x76);
    sensor_write_register (0x3098, 0x00);
    sensor_write_register (0x3099, 0x00);
    sensor_write_register (0x309A, 0x01);
    sensor_write_register (0x3101, 0x01);
    sensor_write_register (0x3104, 0x04);
    sensor_write_register (0x3106, 0x00);
    sensor_write_register (0x3107, 0xC0);
    sensor_write_register (0x312B, 0x20);
    sensor_write_register (0x312D, 0x20);
    sensor_write_register (0x312F, 0x20);
    sensor_write_register (0x3141, 0x40);
    sensor_write_register (0x3143, 0x03);
    sensor_write_register (0x3144, 0x04);
    sensor_write_register (0x3145, 0x03);
    sensor_write_register (0x3146, 0x05);
    sensor_write_register (0x3147, 0x05);
    sensor_write_register (0x3148, 0x02);
    sensor_write_register (0x3149, 0x02);
    sensor_write_register (0x314A, 0x05);
    sensor_write_register (0x314B, 0x03);
    sensor_write_register (0x314C, 0x06);
    sensor_write_register (0x314D, 0x07);
    sensor_write_register (0x314E, 0x06);
    sensor_write_register (0x314F, 0x06);
    sensor_write_register (0x3150, 0x07);
    sensor_write_register (0x3152, 0x06);
    sensor_write_register (0x3153, 0xE3);
    sensor_write_register (0x3155, 0xCA);
    sensor_write_register (0x3157, 0xCA);
    sensor_write_register (0x3159, 0xCA);
    sensor_write_register (0x315B, 0xCA);
    sensor_write_register (0x315D, 0xCA);
    sensor_write_register (0x315F, 0xCA);
    sensor_write_register (0x3161, 0xCA);
    sensor_write_register (0x3163, 0xCA);
    sensor_write_register (0x3165, 0xCA);
    sensor_write_register (0x3167, 0xCA);
    sensor_write_register (0x3169, 0xCA);
    sensor_write_register (0x316B, 0xCA);
    sensor_write_register (0x316D, 0xCA);
    sensor_write_register (0x316F, 0xC6);
    sensor_write_register (0x3171, 0xCA);
    sensor_write_register (0x3173, 0xCA);
    sensor_write_register (0x3175, 0x80);
    sensor_write_register (0x318E, 0x20);
    sensor_write_register (0x318F, 0x70);
    sensor_write_register (0x3196, 0x08);
    sensor_write_register (0x31FC, 0x02);
    sensor_write_register (0x31FE, 0x07);
    sensor_write_register (0x3211, 0x0C);
    sensor_write_register (0x323A, 0x81);
    sensor_write_register (0x323B, 0x91);
    sensor_write_register (0x323C, 0x70);
    sensor_write_register (0x323D, 0x90);
    sensor_write_register (0x323E, 0x00);
    sensor_write_register (0x3243, 0xD1);
    sensor_write_register (0x3247, 0xD6);
    sensor_write_register (0x3248, 0x00);
    sensor_write_register (0x3249, 0x00);
    sensor_write_register (0x324A, 0x30);
    sensor_write_register (0x324B, 0x18);
    sensor_write_register (0x324C, 0x02);
    sensor_write_register (0x3253, 0xD4);
    sensor_write_register (0x3256, 0x11);
    sensor_write_register (0x3259, 0xE6);
    sensor_write_register (0x325A, 0x39);
    sensor_write_register (0x325E, 0x84);
    sensor_write_register (0x3272, 0x55);
    sensor_write_register (0x3273, 0xAB);
    sensor_write_register (0x3280, 0x30);
    sensor_write_register (0x3285, 0x1B);
    sensor_write_register (0x3288, 0x01);
    sensor_write_register (0x328A, 0xE8);
    sensor_write_register (0x330E, 0x05);
    sensor_write_register (0x3310, 0x02);
    sensor_write_register (0x3315, 0x1F);
    sensor_write_register (0x331A, 0x02);
    sensor_write_register (0x331B, 0x02);
    sensor_write_register (0x332C, 0x02);
    sensor_write_register (0x3339, 0x02);
    sensor_write_register (0x336B, 0x03);
    sensor_write_register (0x339F, 0x03);
    sensor_write_register (0x33A2, 0x03);
    sensor_write_register (0x33A3, 0x03);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0xD3);
    sensor_write_register (0x0100, 0x01);
    sensor_write_register (0x0101, 0x00);

    printf("===panasonic mn34220 sensor 1080P30fps 2to1 WDR(30fps)(MIPI port) init success!=====\n");

    bSensorInit = HI_TRUE;

    return;
}


