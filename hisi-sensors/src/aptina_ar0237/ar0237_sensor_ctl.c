/******************************************************************************

  Copyright (C), 2001-2013, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : ar0330_sensor_ctl.c
  Version       : Initial Draft
  Author        : Hisilicon BVT ISP group
  Created       : 2014/10/1
  Description   : Aptina ar0330 sensor driver
  History       :
  1.Date        : 2014/10/1
  Author        : yy
  Modification  : Created file

******************************************************************************/

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

const unsigned char sensor_i2c_addr  = 0x20; /* I2C Address of AR0230 */
const unsigned int  sensor_addr_byte = 2;    /* ADDR byte of AR0230 */
const unsigned int  sensor_data_byte = 2;    /* DATA byte of AR0230 */
static int g_fd = -1;

extern HI_U8 gu8SensorImageMode;
extern WDR_MODE_E genSensorMode;
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

    ret = ioctl(g_fd, GPIO_sensor_write_register(, &i2c_data);

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
        printf("sensor_write_register( error!\n");
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
void sensor_linear_1080p60_init();
void sensor_wdr_1080p30_init();
void sensor_wdr_1080p60_init();


void sensor_init()
{
    unsigned int cmp_value;
    
    sensor_i2c_init();
    cmp_value = (gu8SensorImageMode << 4) | genSensorMode;

    switch (cmp_value)
    {
        case 0x10:
        {
            sensor_linear_1080p30_init();
            break;
        }              
        case 0x20:
        {
            sensor_linear_1080p60_init();
            break;
        } 
        case 0x12:
        {
            sensor_wdr_1080p30_init();
            break;
        } 
        case 0x22:
        {
            sensor_wdr_1080p60_init();
            break;
        }  
        default :
        {
            printf("Not support!\n");
            return ;
        }
    }

    bSensorInit = HI_TRUE;
    
    return ;
}

void sensor_exit()
{
    sensor_i2c_exit();

    return;
}

void sensor_linear_1080p30_init()
{


    //#[HiSPi Linear 1080p30 Low Power Mode - 4 Lane]
    //#Load = Reset
    sensor_write_register(0x301A, 0x0001);
    delay_ms(200);
    sensor_write_register(0x301A, 0x10D8);
    //#LOAD = Linear_mode_sequencer
    //#[Linear_mode_sequencer] 
    sensor_write_register(0x3088, 0x8000); 
    sensor_write_register(0x3086, 0x4558); 
    sensor_write_register(0x3086, 0x72A6); 
    sensor_write_register(0x3086, 0x4A31); 
    sensor_write_register(0x3086, 0x4342); 
    sensor_write_register(0x3086, 0x8E03); 
    sensor_write_register(0x3086, 0x2A14); 
    sensor_write_register(0x3086, 0x4578); 
    sensor_write_register(0x3086, 0x7B3D); 
    sensor_write_register(0x3086, 0xFF3D); 
    sensor_write_register(0x3086, 0xFF3D); 
    sensor_write_register(0x3086, 0xEA2A); 
    sensor_write_register(0x3086, 0x043D); 
    sensor_write_register(0x3086, 0x102A); 
    sensor_write_register(0x3086, 0x052A); 
    sensor_write_register(0x3086, 0x1535); 
    sensor_write_register(0x3086, 0x2A05); 
    sensor_write_register(0x3086, 0x3D10); 
    sensor_write_register(0x3086, 0x4558); 
    sensor_write_register(0x3086, 0x2A04); 
    sensor_write_register(0x3086, 0x2A14); 
    sensor_write_register(0x3086, 0x3DFF); 
    sensor_write_register(0x3086, 0x3DFF); 
    sensor_write_register(0x3086, 0x3DEA); 
    sensor_write_register(0x3086, 0x2A04); 
    sensor_write_register(0x3086, 0x622A); 
    sensor_write_register(0x3086, 0x288E); 
    sensor_write_register(0x3086, 0x0036); 
    sensor_write_register(0x3086, 0x2A08); 
    sensor_write_register(0x3086, 0x3D64); 
    sensor_write_register(0x3086, 0x7A3D); 
    sensor_write_register(0x3086, 0x0444); 
    sensor_write_register(0x3086, 0x2C4B); 
    sensor_write_register(0x3086, 0xA403); 
    sensor_write_register(0x3086, 0x430D); 
    sensor_write_register(0x3086, 0x2D46); 
    sensor_write_register(0x3086, 0x4316); 
    sensor_write_register(0x3086, 0x2A90); 
    sensor_write_register(0x3086, 0x3E06); 
    sensor_write_register(0x3086, 0x2A98); 
    sensor_write_register(0x3086, 0x5F16); 
    sensor_write_register(0x3086, 0x530D); 
    sensor_write_register(0x3086, 0x1660); 
    sensor_write_register(0x3086, 0x3E4C); 
    sensor_write_register(0x3086, 0x2904); 
    sensor_write_register(0x3086, 0x2984); 
    sensor_write_register(0x3086, 0x8E03); 
    sensor_write_register(0x3086, 0x2AFC); 
    sensor_write_register(0x3086, 0x5C1D); 
    sensor_write_register(0x3086, 0x5754); 
    sensor_write_register(0x3086, 0x495F); 
    sensor_write_register(0x3086, 0x5305); 
    sensor_write_register(0x3086, 0x5307); 
    sensor_write_register(0x3086, 0x4D2B); 
    sensor_write_register(0x3086, 0xF810); 
    sensor_write_register(0x3086, 0x164C); 
    sensor_write_register(0x3086, 0x0955); 
    sensor_write_register(0x3086, 0x562B); 
    sensor_write_register(0x3086, 0xB82B); 
    sensor_write_register(0x3086, 0x984E); 
    sensor_write_register(0x3086, 0x1129); 
    sensor_write_register(0x3086, 0x9460); 
    sensor_write_register(0x3086, 0x5C19); 
    sensor_write_register(0x3086, 0x5C1B); 
    sensor_write_register(0x3086, 0x4548); 
    sensor_write_register(0x3086, 0x4508); 
    sensor_write_register(0x3086, 0x4588); 
    sensor_write_register(0x3086, 0x29B6); 
    sensor_write_register(0x3086, 0x8E01); 
    sensor_write_register(0x3086, 0x2AF8); 
    sensor_write_register(0x3086, 0x3E02); 
    sensor_write_register(0x3086, 0x2AFA); 
    sensor_write_register(0x3086, 0x3F09); 
    sensor_write_register(0x3086, 0x5C1B); 
    sensor_write_register(0x3086, 0x29B2); 
    sensor_write_register(0x3086, 0x3F0C); 
    sensor_write_register(0x3086, 0x3E03); 
    sensor_write_register(0x3086, 0x3E15); 
    sensor_write_register(0x3086, 0x5C13); 
    sensor_write_register(0x3086, 0x3F11); 
    sensor_write_register(0x3086, 0x3E0F); 
    sensor_write_register(0x3086, 0x5F2B); 
    sensor_write_register(0x3086, 0x902B); 
    sensor_write_register(0x3086, 0x803E); 
    sensor_write_register(0x3086, 0x062A); 
    sensor_write_register(0x3086, 0xF23F); 
    sensor_write_register(0x3086, 0x103E); 
    sensor_write_register(0x3086, 0x0160); 
    sensor_write_register(0x3086, 0x29A2); 
    sensor_write_register(0x3086, 0x29A3); 
    sensor_write_register(0x3086, 0x5F4D); 
    sensor_write_register(0x3086, 0x1C2A); 
    sensor_write_register(0x3086, 0xFA29); 
    sensor_write_register(0x3086, 0x8345); 
    sensor_write_register(0x3086, 0xA83E); 
    sensor_write_register(0x3086, 0x072A); 
    sensor_write_register(0x3086, 0xFB3E); 
    sensor_write_register(0x3086, 0x2945); 
    sensor_write_register(0x3086, 0x8824); 
    sensor_write_register(0x3086, 0x3E08); 
    sensor_write_register(0x3086, 0x2AFA); 
    sensor_write_register(0x3086, 0x5D29); 
    sensor_write_register(0x3086, 0x9288); 
    sensor_write_register(0x3086, 0x102B); 
    sensor_write_register(0x3086, 0x048B); 
    sensor_write_register(0x3086, 0x1686); 
    sensor_write_register(0x3086, 0x8D48); 
    sensor_write_register(0x3086, 0x4D4E); 
    sensor_write_register(0x3086, 0x2B80); 
    sensor_write_register(0x3086, 0x4C0B); 
    sensor_write_register(0x3086, 0x3F36); 
    sensor_write_register(0x3086, 0x2AF2); 
    sensor_write_register(0x3086, 0x3F10); 
    sensor_write_register(0x3086, 0x3E01); 
    sensor_write_register(0x3086, 0x6029); 
    sensor_write_register(0x3086, 0x8229); 
    sensor_write_register(0x3086, 0x8329); 
    sensor_write_register(0x3086, 0x435C); 
    sensor_write_register(0x3086, 0x155F); 
    sensor_write_register(0x3086, 0x4D1C); 
    sensor_write_register(0x3086, 0x2AFA); 
    sensor_write_register(0x3086, 0x4558); 
    sensor_write_register(0x3086, 0x8E00); 
    sensor_write_register(0x3086, 0x2A98); 
    sensor_write_register(0x3086, 0x3F0A); 
    sensor_write_register(0x3086, 0x4A0A); 
    sensor_write_register(0x3086, 0x4316); 
    sensor_write_register(0x3086, 0x0B43); 
    sensor_write_register(0x3086, 0x168E); 
    sensor_write_register(0x3086, 0x032A); 
    sensor_write_register(0x3086, 0x9C45); 
    sensor_write_register(0x3086, 0x783F); 
    sensor_write_register(0x3086, 0x072A); 
    sensor_write_register(0x3086, 0x9D3E); 
    sensor_write_register(0x3086, 0x305D); 
    sensor_write_register(0x3086, 0x2944); 
    sensor_write_register(0x3086, 0x8810); 
    sensor_write_register(0x3086, 0x2B04); 
    sensor_write_register(0x3086, 0x530D); 
    sensor_write_register(0x3086, 0x4558); 
    sensor_write_register(0x3086, 0x3E08); 
    sensor_write_register(0x3086, 0x8E01); 
    sensor_write_register(0x3086, 0x2A98); 
    sensor_write_register(0x3086, 0x8E00); 
    sensor_write_register(0x3086, 0x76A7); 
    sensor_write_register(0x3086, 0x77A7); 
    sensor_write_register(0x3086, 0x4644); 
    sensor_write_register(0x3086, 0x1616); 
    sensor_write_register(0x3086, 0xA57A); 
    sensor_write_register(0x3086, 0x1244); 
    sensor_write_register(0x3086, 0x4B18); 
    sensor_write_register(0x3086, 0x4A04); 
    sensor_write_register(0x3086, 0x4316); 
    sensor_write_register(0x3086, 0x0643); 
    sensor_write_register(0x3086, 0x1605); 
    sensor_write_register(0x3086, 0x4316); 
    sensor_write_register(0x3086, 0x0743); 
    sensor_write_register(0x3086, 0x1658); 
    sensor_write_register(0x3086, 0x4316); 
    sensor_write_register(0x3086, 0x5A43); 
    sensor_write_register(0x3086, 0x1645); 
    sensor_write_register(0x3086, 0x588E); 
    sensor_write_register(0x3086, 0x032A); 
    sensor_write_register(0x3086, 0x9C45); 
    sensor_write_register(0x3086, 0x787B); 
    sensor_write_register(0x3086, 0x3F07); 
    sensor_write_register(0x3086, 0x2A9D); 
    sensor_write_register(0x3086, 0x530D); 
    sensor_write_register(0x3086, 0x8B16); 
    sensor_write_register(0x3086, 0x863E); 
    sensor_write_register(0x3086, 0x2345); 
    sensor_write_register(0x3086, 0x5825); 
    sensor_write_register(0x3086, 0x3E10); 
    sensor_write_register(0x3086, 0x8E01); 
    sensor_write_register(0x3086, 0x2A98); 
    sensor_write_register(0x3086, 0x8E00); 
    sensor_write_register(0x3086, 0x3E10); 
    sensor_write_register(0x3086, 0x8D60); 
    sensor_write_register(0x3086, 0x1244);
    sensor_write_register(0x3086, 0x4BB9);
    sensor_write_register(0x3086, 0x2C2C);
    sensor_write_register(0x3086, 0x2C2C);

    //#LOAD= Default_Settings_12_Aug
    //#[Default_Settings_12_Aug]
    sensor_write_register(0x30F0, 0x1283);
    sensor_write_register(0x3064, 0x1802);
    sensor_write_register(0x3EEE, 0xA0AA);
    sensor_write_register(0x30BA, 0x762C);
    sensor_write_register(0x3FA4, 0x0F70); 
    sensor_write_register(0x309E, 0x016A); 
    sensor_write_register(0x3096, 0xF880);
    sensor_write_register(0x3F32, 0xF880);
    sensor_write_register(0x3092, 0x006F);

    sensor_write_register(0x301A, 0x0058);
    sensor_write_register(0x30B0, 0x0118);
    sensor_write_register(0x31AC, 0x0C0C);

    //#LOAD= PLL_settings - 4 Lane 12-bit HiSPi Low Power Mode
    //#[PLL_settings - 4 Lane 12-bit HiSPi Low Power Mode]
    sensor_write_register(0x302A, 0x000C);
    sensor_write_register(0x302C, 0x0001);
    sensor_write_register(0x302E, 0x0004);
    sensor_write_register(0x3030, 0x0042);
    sensor_write_register(0x3036, 0x000C);
    sensor_write_register(0x3038, 0x0002);
                               
    sensor_write_register(0x3002, 0x0000);
    sensor_write_register(0x3004, 0x0000);
    sensor_write_register(0x3006, 0x0437);
    sensor_write_register(0x3008, 0x0787);
    sensor_write_register(0x300A, 0x0454);
    sensor_write_register(0x300C, 0x045E);
    sensor_write_register(0x3012, 0x0416);
    sensor_write_register(0x30A2, 0x0001);
    sensor_write_register(0x30A6, 0x0001);
    sensor_write_register(0x30AE, 0x0001);
    sensor_write_register(0x30A8, 0x0001);
    sensor_write_register(0x3040, 0x0000);

    //#LOAD= Linear Mode Setup
    sensor_write_register(0x3082, 0x0009);
    //#sensor_write_register(0x30BA 0x762C);
    sensor_write_register(0x3096, 0x0080);
    sensor_write_register(0x3098, 0x0080);
    //#LOAD= Linear Mode Low Conversion Gain
    //#LOAD= 1.5x analog Gain
    sensor_write_register(0x3060, 0x000B);

    sensor_write_register(0x3100, 0x0000);
    //#LOAD= Companding Disabled
    sensor_write_register(0x31D0, 0x0000);
    //#LOAD= AR0230_Rev1_AWB_CCM
    //#LOAD= Enable Embedded Data and Stats
    //#sensor_write_register(0x3064 0x1982);

    sensor_write_register(0x31AE, 0x0304);
    sensor_write_register(0x31C6, 0x0400);
    sensor_write_register(0x306E, 0x9010);
    //#patch start########################### 
    sensor_write_register(0x3ECC, 0x6E2D);
    sensor_write_register(0x3ED2, 0xFE96);
    sensor_write_register(0x3ED6, 0x34B3);
    sensor_write_register(0x3EEA, 0x2818);
    //#patch end ############################ 
    sensor_write_register(0x301A, 0x005C);
    delay_ms(33);

    printf("Aptina AR0237 sensor linear 2M-1080p 30fps init success!\n");
    
    return ;
}


    
void sensor_linear_1080p60_init()
{
    printf("Aptina AR0237 sensor linear 2M-1080p 60fps init failure!\n");
}


void sensor_wdr_1080p30_init()
{
    //#[HiSPi Linear 1080p30 Low Power Mode - 4 Lane]
    //#Load = Reset
    sensor_write_register(0x301A, 0x0001);
    delay_ms(200);
    sensor_write_register(0x301A, 0x10D8);

    //#LOAD = Line_Interleaved_Sequencer
    //#[Line_Interleaved_Sequencer]
    sensor_write_register(0x3088, 0x816A);
    sensor_write_register(0x3086, 0x4558);
    sensor_write_register(0x3086, 0x729B);
    sensor_write_register(0x3086, 0x4A31);
    sensor_write_register(0x3086, 0x4342);
    sensor_write_register(0x3086, 0x8E03);
    sensor_write_register(0x3086, 0x2A14);
    sensor_write_register(0x3086, 0x4578);
    sensor_write_register(0x3086, 0x7B3D);
    sensor_write_register(0x3086, 0xFF3D);
    sensor_write_register(0x3086, 0xFF3D);
    sensor_write_register(0x3086, 0xEA2A);
    sensor_write_register(0x3086, 0x043D);
    sensor_write_register(0x3086, 0x102A);
    sensor_write_register(0x3086, 0x052A);
    sensor_write_register(0x3086, 0x1535);
    sensor_write_register(0x3086, 0x2A05);
    sensor_write_register(0x3086, 0x3D10);
    sensor_write_register(0x3086, 0x4558);
    sensor_write_register(0x3086, 0x2A04);
    sensor_write_register(0x3086, 0x2A14);
    sensor_write_register(0x3086, 0x3DFF);
    sensor_write_register(0x3086, 0x3DFF);
    sensor_write_register(0x3086, 0x3DEA);
    sensor_write_register(0x3086, 0x2A04);
    sensor_write_register(0x3086, 0x622A);
    sensor_write_register(0x3086, 0x288E);
    sensor_write_register(0x3086, 0x0036);
    sensor_write_register(0x3086, 0x2A08);
    sensor_write_register(0x3086, 0x3D64);
    sensor_write_register(0x3086, 0x7A3D);
    sensor_write_register(0x3086, 0x0444);
    sensor_write_register(0x3086, 0x2C4B);
    sensor_write_register(0x3086, 0x8F00);
    sensor_write_register(0x3086, 0x430C);
    sensor_write_register(0x3086, 0x2D63);
    sensor_write_register(0x3086, 0x4316);
    sensor_write_register(0x3086, 0x2A90);
    sensor_write_register(0x3086, 0x3E06);
    sensor_write_register(0x3086, 0x2A98);
    sensor_write_register(0x3086, 0x168E);
    sensor_write_register(0x3086, 0x032A);
    sensor_write_register(0x3086, 0xFC5C);
    sensor_write_register(0x3086, 0x1D57);
    sensor_write_register(0x3086, 0x5449);
    sensor_write_register(0x3086, 0x5F53);
    sensor_write_register(0x3086, 0x0553);
    sensor_write_register(0x3086, 0x074D);
    sensor_write_register(0x3086, 0x2BF8);
    sensor_write_register(0x3086, 0x1016);
    sensor_write_register(0x3086, 0x4C08);
    sensor_write_register(0x3086, 0x5556);
    sensor_write_register(0x3086, 0x2BB8);
    sensor_write_register(0x3086, 0x2B98);
    sensor_write_register(0x3086, 0x4E11);
    sensor_write_register(0x3086, 0x2904);
    sensor_write_register(0x3086, 0x2984);
    sensor_write_register(0x3086, 0x2994);
    sensor_write_register(0x3086, 0x605C);
    sensor_write_register(0x3086, 0x195C);
    sensor_write_register(0x3086, 0x1B45);
    sensor_write_register(0x3086, 0x4845);
    sensor_write_register(0x3086, 0x0845);
    sensor_write_register(0x3086, 0x8829);
    sensor_write_register(0x3086, 0xB68E);
    sensor_write_register(0x3086, 0x012A);
    sensor_write_register(0x3086, 0xF83E);
    sensor_write_register(0x3086, 0x022A);
    sensor_write_register(0x3086, 0xFA3F);
    sensor_write_register(0x3086, 0x095C);
    sensor_write_register(0x3086, 0x1B29);
    sensor_write_register(0x3086, 0xB23F);
    sensor_write_register(0x3086, 0x0C3E);
    sensor_write_register(0x3086, 0x023E);
    sensor_write_register(0x3086, 0x135C);
    sensor_write_register(0x3086, 0x133F);
    sensor_write_register(0x3086, 0x113E);
    sensor_write_register(0x3086, 0x0B5F);
    sensor_write_register(0x3086, 0x2B90);
    sensor_write_register(0x3086, 0x2B80);
    sensor_write_register(0x3086, 0x3E06);
    sensor_write_register(0x3086, 0x162A);
    sensor_write_register(0x3086, 0xF23F);
    sensor_write_register(0x3086, 0x103E);
    sensor_write_register(0x3086, 0x0160);
    sensor_write_register(0x3086, 0x29A2);
    sensor_write_register(0x3086, 0x29A3);
    sensor_write_register(0x3086, 0x5F4D);
    sensor_write_register(0x3086, 0x192A);
    sensor_write_register(0x3086, 0xFA29);
    sensor_write_register(0x3086, 0x8345);
    sensor_write_register(0x3086, 0xA83E);
    sensor_write_register(0x3086, 0x072A);
    sensor_write_register(0x3086, 0xFB3E);
    sensor_write_register(0x3086, 0x2945);
    sensor_write_register(0x3086, 0x8821);
    sensor_write_register(0x3086, 0x3E08);
    sensor_write_register(0x3086, 0x2AFA);
    sensor_write_register(0x3086, 0x5D29);
    sensor_write_register(0x3086, 0x9288);
    sensor_write_register(0x3086, 0x102B);
    sensor_write_register(0x3086, 0x048B);
    sensor_write_register(0x3086, 0x1685);
    sensor_write_register(0x3086, 0x8D48);
    sensor_write_register(0x3086, 0x4D4E);
    sensor_write_register(0x3086, 0x2B80);
    sensor_write_register(0x3086, 0x4C0B);
    sensor_write_register(0x3086, 0x3F2B);
    sensor_write_register(0x3086, 0x2AF2);
    sensor_write_register(0x3086, 0x3F10);
    sensor_write_register(0x3086, 0x3E01);
    sensor_write_register(0x3086, 0x6029);
    sensor_write_register(0x3086, 0x8229);
    sensor_write_register(0x3086, 0x8329);
    sensor_write_register(0x3086, 0x435C);
    sensor_write_register(0x3086, 0x155F);
    sensor_write_register(0x3086, 0x4D19);
    sensor_write_register(0x3086, 0x2AFA);
    sensor_write_register(0x3086, 0x4558);
    sensor_write_register(0x3086, 0x8E00);
    sensor_write_register(0x3086, 0x2A98);
    sensor_write_register(0x3086, 0x3F06);
    sensor_write_register(0x3086, 0x1244);
    sensor_write_register(0x3086, 0x4A04);
    sensor_write_register(0x3086, 0x4316);
    sensor_write_register(0x3086, 0x0543);
    sensor_write_register(0x3086, 0x1658);
    sensor_write_register(0x3086, 0x4316);
    sensor_write_register(0x3086, 0x5A43);
    sensor_write_register(0x3086, 0x1606);
    sensor_write_register(0x3086, 0x4316);
    sensor_write_register(0x3086, 0x0743);
    sensor_write_register(0x3086, 0x168E);
    sensor_write_register(0x3086, 0x032A);
    sensor_write_register(0x3086, 0x9C45);
    sensor_write_register(0x3086, 0x787B);
    sensor_write_register(0x3086, 0x3F07);
    sensor_write_register(0x3086, 0x2A9D);
    sensor_write_register(0x3086, 0x3E2E);
    sensor_write_register(0x3086, 0x4558);
    sensor_write_register(0x3086, 0x253E);
    sensor_write_register(0x3086, 0x068E);
    sensor_write_register(0x3086, 0x012A);
    sensor_write_register(0x3086, 0x988E);
    sensor_write_register(0x3086, 0x0012);
    sensor_write_register(0x3086, 0x444B);
    sensor_write_register(0x3086, 0x0343);
    sensor_write_register(0x3086, 0x2D46);
    sensor_write_register(0x3086, 0x4316);
    sensor_write_register(0x3086, 0xA343);
    sensor_write_register(0x3086, 0x165D);
    sensor_write_register(0x3086, 0x0D29);
    sensor_write_register(0x3086, 0x4488);
    sensor_write_register(0x3086, 0x102B);
    sensor_write_register(0x3086, 0x0453);
    sensor_write_register(0x3086, 0x0D8B);
    sensor_write_register(0x3086, 0x1685);
    sensor_write_register(0x3086, 0x448E);
    sensor_write_register(0x3086, 0x032A);
    sensor_write_register(0x3086, 0xFC5C);
    sensor_write_register(0x3086, 0x1D8D);
    sensor_write_register(0x3086, 0x6057);
    sensor_write_register(0x3086, 0x5417);
    sensor_write_register(0x3086, 0xFF17);
    sensor_write_register(0x3086, 0x4B2A);
    sensor_write_register(0x3086, 0xF43E);
    sensor_write_register(0x3086, 0x062A);
    sensor_write_register(0x3086, 0xFC49);
    sensor_write_register(0x3086, 0x5F53);
    sensor_write_register(0x3086, 0x0553);
    sensor_write_register(0x3086, 0x074D);
    sensor_write_register(0x3086, 0x2BF8);
    sensor_write_register(0x3086, 0x1016);
    sensor_write_register(0x3086, 0x4C08);
    sensor_write_register(0x3086, 0x5556);
    sensor_write_register(0x3086, 0x2BB8);
    sensor_write_register(0x3086, 0x2B98);
    sensor_write_register(0x3086, 0x4E11);
    sensor_write_register(0x3086, 0x2904);
    sensor_write_register(0x3086, 0x2984);
    sensor_write_register(0x3086, 0x2994);
    sensor_write_register(0x3086, 0x605C);
    sensor_write_register(0x3086, 0x195C);
    sensor_write_register(0x3086, 0x1B45);
    sensor_write_register(0x3086, 0x4845);
    sensor_write_register(0x3086, 0x0845);
    sensor_write_register(0x3086, 0x8829);
    sensor_write_register(0x3086, 0xB68E);
    sensor_write_register(0x3086, 0x012A);
    sensor_write_register(0x3086, 0xF83E);
    sensor_write_register(0x3086, 0x022A);
    sensor_write_register(0x3086, 0xFA3F);
    sensor_write_register(0x3086, 0x095C);
    sensor_write_register(0x3086, 0x1B29);
    sensor_write_register(0x3086, 0xB23F);
    sensor_write_register(0x3086, 0x0C3E);
    sensor_write_register(0x3086, 0x023E);
    sensor_write_register(0x3086, 0x135C);
    sensor_write_register(0x3086, 0x133F);
    sensor_write_register(0x3086, 0x113E);
    sensor_write_register(0x3086, 0x0B5F);
    sensor_write_register(0x3086, 0x2B90);
    sensor_write_register(0x3086, 0x2B80);
    sensor_write_register(0x3086, 0x3E10);
    sensor_write_register(0x3086, 0x2AF2);
    sensor_write_register(0x3086, 0x3F10);
    sensor_write_register(0x3086, 0x3E01);
    sensor_write_register(0x3086, 0x6029);
    sensor_write_register(0x3086, 0xA229);
    sensor_write_register(0x3086, 0xA35F);
    sensor_write_register(0x3086, 0x4D1C);
    sensor_write_register(0x3086, 0x2AFA);
    sensor_write_register(0x3086, 0x2983);
    sensor_write_register(0x3086, 0x45A8);
    sensor_write_register(0x3086, 0x3E07);
    sensor_write_register(0x3086, 0x2AFB);
    sensor_write_register(0x3086, 0x3E29);
    sensor_write_register(0x3086, 0x4588);
    sensor_write_register(0x3086, 0x243E);
    sensor_write_register(0x3086, 0x082A);
    sensor_write_register(0x3086, 0xFA5D);
    sensor_write_register(0x3086, 0x2992);
    sensor_write_register(0x3086, 0x8810);
    sensor_write_register(0x3086, 0x2B04);
    sensor_write_register(0x3086, 0x8B16);
    sensor_write_register(0x3086, 0x868D);
    sensor_write_register(0x3086, 0x484D);
    sensor_write_register(0x3086, 0x4E2B);
    sensor_write_register(0x3086, 0x804C);
    sensor_write_register(0x3086, 0x0B3F);
    sensor_write_register(0x3086, 0x332A);
    sensor_write_register(0x3086, 0xF23F);
    sensor_write_register(0x3086, 0x103E);
    sensor_write_register(0x3086, 0x0160);
    sensor_write_register(0x3086, 0x2982);
    sensor_write_register(0x3086, 0x2983);
    sensor_write_register(0x3086, 0x2943);
    sensor_write_register(0x3086, 0x5C15);
    sensor_write_register(0x3086, 0x5F4D);
    sensor_write_register(0x3086, 0x1C2A);
    sensor_write_register(0x3086, 0xFA45);
    sensor_write_register(0x3086, 0x588E);
    sensor_write_register(0x3086, 0x002A);
    sensor_write_register(0x3086, 0x983F);
    sensor_write_register(0x3086, 0x064A);
    sensor_write_register(0x3086, 0x739D);
    sensor_write_register(0x3086, 0x0A43);
    sensor_write_register(0x3086, 0x160B);
    sensor_write_register(0x3086, 0x4316);
    sensor_write_register(0x3086, 0x8E03);
    sensor_write_register(0x3086, 0x2A9C);
    sensor_write_register(0x3086, 0x4578);
    sensor_write_register(0x3086, 0x3F07);
    sensor_write_register(0x3086, 0x2A9D);
    sensor_write_register(0x3086, 0x3E12);
    sensor_write_register(0x3086, 0x4558);
    sensor_write_register(0x3086, 0x3F04);
    sensor_write_register(0x3086, 0x8E01);
    sensor_write_register(0x3086, 0x2A98);
    sensor_write_register(0x3086, 0x8E00);
    sensor_write_register(0x3086, 0x9176);
    sensor_write_register(0x3086, 0x9C77);
    sensor_write_register(0x3086, 0x9C46);
    sensor_write_register(0x3086, 0x4416);
    sensor_write_register(0x3086, 0x1690);
    sensor_write_register(0x3086, 0x7A12);
    sensor_write_register(0x3086, 0x444B);
    sensor_write_register(0x3086, 0x4A00);
    sensor_write_register(0x3086, 0x4316);
    sensor_write_register(0x3086, 0x6343);
    sensor_write_register(0x3086, 0x1608);
    sensor_write_register(0x3086, 0x4316);
    sensor_write_register(0x3086, 0x5043);
    sensor_write_register(0x3086, 0x1665);
    sensor_write_register(0x3086, 0x4316);
    sensor_write_register(0x3086, 0x6643);
    sensor_write_register(0x3086, 0x168E);
    sensor_write_register(0x3086, 0x032A);
    sensor_write_register(0x3086, 0x9C45);
    sensor_write_register(0x3086, 0x783F);
    sensor_write_register(0x3086, 0x072A);
    sensor_write_register(0x3086, 0x9D5D);
    sensor_write_register(0x3086, 0x0C29);
    sensor_write_register(0x3086, 0x4488);
    sensor_write_register(0x3086, 0x102B);
    sensor_write_register(0x3086, 0x0453);
    sensor_write_register(0x3086, 0x0D8B);
    sensor_write_register(0x3086, 0x1686);
    sensor_write_register(0x3086, 0x3E1F);
    sensor_write_register(0x3086, 0x4558);
    sensor_write_register(0x3086, 0x283E);
    sensor_write_register(0x3086, 0x068E);
    sensor_write_register(0x3086, 0x012A);
    sensor_write_register(0x3086, 0x988E);
    sensor_write_register(0x3086, 0x008D);
    sensor_write_register(0x3086, 0x6012);
    sensor_write_register(0x3086, 0x444B);
    sensor_write_register(0x3086, 0xB92C);
    sensor_write_register(0x3086, 0x2C2C);

    //#LOAD= Default_Settings_12_Aug
    //#[Default_Settings_12_Aug]
    sensor_write_register(0x30F0, 0x1283);
    sensor_write_register(0x3064, 0x1802);
    sensor_write_register(0x3EEE, 0xA0AA);
    sensor_write_register(0x30BA, 0x762C);
    sensor_write_register(0x3FA4, 0x0F70);
    sensor_write_register(0x309E, 0x016A);
    sensor_write_register(0x3096, 0xF880);
    sensor_write_register(0x3F32, 0xF880);
    sensor_write_register(0x3092, 0x006F);
    //##
    sensor_write_register(0x301A, 0x0058);
    sensor_write_register(0x30B0, 0x0118);
    //#sensor_write_register(0x31AC 0x100C);
    sensor_write_register(0x31AC, 0x0C0C);
    sensor_write_register(0x31C0, 0x0000);

    //#LOAD= PLL_settings - 4 Lane 12-bit HiSPi
    //#[PLL_settings - 4 Lane 12-bit HiSPi]
    sensor_write_register(0x302A, 0x0006);
    sensor_write_register(0x302C, 0x0001);
    sensor_write_register(0x302E, 0x0004);
    sensor_write_register(0x3030, 0x0042);
    sensor_write_register(0x3036, 0x000C);
    sensor_write_register(0x3038, 0x0001);
    //##
    sensor_write_register(0x3002, 0x0000);
    sensor_write_register(0x3004, 0x0000);
    sensor_write_register(0x3006, 0x0437);
    sensor_write_register(0x3008, 0x0787);
    sensor_write_register(0x300A, 0x04C4);
    sensor_write_register(0x300C, 0x07F0);
    sensor_write_register(0x3012, 0x0103);
    sensor_write_register(0x30A2, 0x0001);
    sensor_write_register(0x30A6, 0x0001);
    sensor_write_register(0x30AE, 0x0001);
    sensor_write_register(0x30A8, 0x0001);
    sensor_write_register(0x3040, 0x0000);

    //#LOAD= HDR Mode 16x Setup
    //#[HDR Mode 16x Setup]
    sensor_write_register(0x3082, 0x0008);
    //	#LOAD= HDR Mode Setup
    //	#LOAD= HDR Mode Low Conversion Gain
    //  #LOAD= 1.5x analog Gain
    sensor_write_register(0x3060, 0x000B);
    //##
    sensor_write_register(0x3096, 0x0480);
    sensor_write_register(0x3098, 0x0480);
    sensor_write_register(0x3100, 0x0000);

    sensor_write_register(0x30BA, 0x772c);


    sensor_write_register(0x31AE, 0x0304);
    sensor_write_register(0x31C6, 0x0400);
    sensor_write_register(0x306E, 0x9218);

    //#LOAD= Disable Embedded Data and Stats
    //#sensor_write_register(0x3064 0x1802);

    sensor_write_register(0x318E, 0x1200);
    //#patch start########################### 
    sensor_write_register(0x3ECC, 0x6E2D);
    sensor_write_register(0x3ED2, 0xFE96);
    sensor_write_register(0x3ED6, 0x34B3);
    sensor_write_register(0x3EEA, 0x2818);
    //#patch end ############################   
    
    sensor_write_register(0x301A, 0x005C);
    sensor_write_register(0x31D0, 0x0000);

    sensor_write_register(0x3100, 0x0104);  // T1&T2 use same AGain Reg
    sensor_write_register(0x3060, 0x003B);
    sensor_write_register(0x312C, 0x0030);
    sensor_write_register(0x3082, 0x0808);
    sensor_write_register(0x3212, 0x001F);
    delay_ms(33);

    printf("Aptina AR0237 sensor wdr 2M-1080p 30fps init success!\n");
    return ;
}

void sensor_wdr_1080p60_init()
{
    printf("Aptina AR0237 sensor wdr 2M-1080p 60fps init falure!\n");
}

