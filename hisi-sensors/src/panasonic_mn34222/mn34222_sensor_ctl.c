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

const unsigned char sensor_i2c_addr        =    0x6c;        /* I2C Address of MN34222 */
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

void sensor_linear_1080p30_init();

void sensor_init()
{
    sensor_i2c_init();
    
    /* When sensor first init, config all registers */
    if (HI_FALSE == bSensorInit) 
    {
        
                sensor_linear_1080p30_init();
    }
    /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
    else 
    {
        
                sensor_linear_1080p30_init();    
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
    /*# N340M_S12_P2_FHD_V1125_12b_445MHz_30fps_vM17e_150703_MCLK37_Mst_I2C_d.txt
      # VCYCLE:1125 HCYCLE:1100 (@MCLK)*/
    sensor_write_register (0x300E, 0x01);
    sensor_write_register (0x300F, 0x00);
    sensor_write_register (0x0305, 0x02);
    sensor_write_register (0x0307, 0x24);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0x03);
    sensor_write_register (0x0112, 0x0C);
    sensor_write_register (0x0113, 0x0C);
    sensor_write_register (0x3004, 0x01);
    sensor_write_register (0x3005, 0x64);
    sensor_write_register (0x3008, 0xF1);
    sensor_write_register (0x3009, 0x0E);//mipi:bit3=1 lvds:bit3=0
    sensor_write_register (0x300B, 0x00);
    sensor_write_register (0x3018, 0x43);
    sensor_write_register (0x3019, 0x10);
    sensor_write_register (0x301A, 0xB9);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0x53);
    sensor_write_register (0x300E, 0x00);
    sensor_write_register (0x300F, 0x00);
    sensor_write_register (0x0202, 0x04);
    sensor_write_register (0x0203, 0x63);
    sensor_write_register (0x3036, 0x00);
    sensor_write_register (0x3039, 0x2E);
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
    sensor_write_register (0x3104, 0x04);
    sensor_write_register (0x3106, 0x00);
    sensor_write_register (0x3107, 0xC0);
    sensor_write_register (0x3141, 0x40);
    sensor_write_register (0x3143, 0x02);
    sensor_write_register (0x3144, 0x02);
    sensor_write_register (0x3145, 0x02);
    sensor_write_register (0x3146, 0x00);
    sensor_write_register (0x3147, 0x02);
    sensor_write_register (0x314A, 0x01);
    sensor_write_register (0x314B, 0x02);
    sensor_write_register (0x314C, 0x02);
    sensor_write_register (0x314D, 0x02);
    sensor_write_register (0x314E, 0x01);
    sensor_write_register (0x314F, 0x02);
    sensor_write_register (0x3150, 0x02);
    sensor_write_register (0x3152, 0x04);
    sensor_write_register (0x3153, 0xE3);
    sensor_write_register (0x316F, 0xC6);
    sensor_write_register (0x3175, 0x80);
    sensor_write_register (0x318E, 0x20);
    sensor_write_register (0x318F, 0x70);
    sensor_write_register (0x3196, 0x08);
    sensor_write_register (0x3211, 0x0C);
    sensor_write_register (0x323A, 0x80);
    sensor_write_register (0x323B, 0x91);
    sensor_write_register (0x323D, 0x90);
    sensor_write_register (0x3243, 0xD7);
    sensor_write_register (0x3246, 0x03);
    sensor_write_register (0x3247, 0x27);
    sensor_write_register (0x3248, 0x03);
    sensor_write_register (0x3249, 0x79);
    sensor_write_register (0x324A, 0x30);
    sensor_write_register (0x324B, 0x18);
    sensor_write_register (0x324C, 0x02);
    sensor_write_register (0x3253, 0xDE);
    sensor_write_register (0x3258, 0x03);
    sensor_write_register (0x3259, 0x17);
    sensor_write_register (0x3272, 0x46);
    sensor_write_register (0x3280, 0x30);
    sensor_write_register (0x3288, 0x01);
    sensor_write_register (0x330E, 0x05);
    sensor_write_register (0x3310, 0x02);
    sensor_write_register (0x3315, 0x1F);
    sensor_write_register (0x332C, 0x02);
    sensor_write_register (0x3339, 0x02);
    sensor_write_register (0x3000, 0x00);
    sensor_write_register (0x3001, 0xD3);
    sensor_write_register (0x0100, 0x01);
    sensor_write_register (0x0101, 0x00);
    
    printf("===panasonic mn34222 sensor lvds 1ch2lane 1080P30fps linear mode init success!=====\n");
    
    bSensorInit = HI_TRUE;

    return;
}




