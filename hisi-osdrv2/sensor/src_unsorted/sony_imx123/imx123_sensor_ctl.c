#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "hi_comm_video.h"

#include "hi_spi.h"


extern WDR_MODE_E genSensorMode;
extern HI_U8 gu8SensorImageMode;
extern HI_BOOL bSensorInit;

static int g_fd= -1;

int sensor_spi_init(void)
{
    if (g_fd >= 0)
    {
        return 0;
    }     
    unsigned int value;
    int ret = 0;
    char file_name[] = "/dev/spidev0.0";

    g_fd = open(file_name, 0);
    if (g_fd < 0)
    {
        printf("Open %s error!\n",file_name);
        return -1;
    }

    value = SPI_MODE_3 | SPI_LSB_FIRST;// | SPI_LOOP;
    ret = ioctl(g_fd, SPI_IOC_WR_MODE, &value);
    if (ret < 0)
    {
        printf("ioctl SPI_IOC_WR_MODE err, value = %d ret = %d\n", value, ret);
        return ret;
    }

    value = 8;
    ret = ioctl(g_fd, SPI_IOC_WR_BITS_PER_WORD, &value);
    if (ret < 0)
    {
        printf("ioctl SPI_IOC_WR_BITS_PER_WORD err, value = %d ret = %d\n",value, ret);
        return ret;
    }

    value = 2000000;
    ret = ioctl(g_fd, SPI_IOC_WR_MAX_SPEED_HZ, &value);
    if (ret < 0)
    {
        printf("ioctl SPI_IOC_WR_MAX_SPEED_HZ err, value = %d ret = %d\n",value, ret);
        return ret;
    }

    return 0;
}

int sensor_spi_exit(void)
{
    if (g_fd >= 0)
    {
        close(g_fd);
        g_fd = -1;
        return 0;
    }
    return -1;
}

int sensor_write_register(unsigned int addr, unsigned char data)
{
    int ret;
    struct spi_ioc_transfer mesg[1];
    unsigned char  tx_buf[8] = {0};
    unsigned char  rx_buf[8] = {0};
    
    tx_buf[0] = (addr & 0xff00) >> 8;
    tx_buf[0] &= (~0x80);
    tx_buf[1] = addr & 0xff;
    tx_buf[2] = data;

    memset(mesg, 0, sizeof(mesg));  
    mesg[0].tx_buf = (__u32)tx_buf;  
    mesg[0].len    = 3;  
    mesg[0].rx_buf = (__u32)rx_buf; 
    mesg[0].cs_change = 1;

    ret = ioctl(g_fd, SPI_IOC_MESSAGE(1), mesg);
    if (ret < 0) {  
        printf("SPI_IOC_MESSAGE error \n");  
        return -1;  
    }

    return 0;
}

int sensor_read_register(unsigned int addr)
{
    int ret = 0;
    struct spi_ioc_transfer mesg[1];
    unsigned char  tx_buf[8] = {0};
    unsigned char  rx_buf[8] = {0};
    
    tx_buf[0] = (addr & 0xff00) >> 8;
    tx_buf[0] |= 0x80;
    tx_buf[1] = addr & 0xff;
    tx_buf[2] = 0;

    memset(mesg, 0, sizeof(mesg));
    mesg[0].tx_buf = (__u32)tx_buf;
    mesg[0].len    = 3;
    mesg[0].rx_buf = (__u32)rx_buf;
    mesg[0].cs_change = 1;

    ret = ioctl(g_fd, SPI_IOC_MESSAGE(1), mesg);
    if (ret  < 0) {  
        printf("SPI_IOC_MESSAGE error \n");  
        return -1;  
    }
    
    return rx_buf[2];
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

void sensor_wdr_qxga_30_init();
void sensor_linear_qxga_30_init();
void sensor_linear_1080p60_init();
void sensor_linear_qxga_60_init();


void sensor_init()
{
    bSensorInit = HI_TRUE;
    /* 1. sensor spi init */
    sensor_spi_init();

    switch (gu8SensorImageMode)
    {        
        case 0: // 1080P60
        sensor_linear_1080p60_init();
            
        break;
        case 1: // 1080P30
            if(WDR_MODE_2To1_LINE == genSensorMode)
            {
                
            }
            else
            {
               // sensor_linear_1080p30_init();
            }
            break;
        case 2: // QXGA 30
            if(WDR_MODE_2To1_LINE == genSensorMode)
            {
                sensor_wdr_qxga_30_init();
            }
            else
            {
                sensor_linear_qxga_30_init();
            }
            break;
        case 3: // QXGA 60
            if(WDR_MODE_2To1_LINE == genSensorMode)
            {
                
            }
            else
            {
                sensor_linear_qxga_60_init();
            }
            break;
        default:
            printf("Not support this mode\n");
            bSensorInit = HI_FALSE;
    }


}

void sensor_exit()
{
    sensor_spi_exit();

    return;
}

// 37.125MHz clock input
// 2970Mbps (=371.25Mbps/ch x 8ch)
void sensor_linear_qxga_60_init()
{
    sensor_write_register(0x203, 0x01);  //soft reset
    //delay_ms(200);  
    
    sensor_write_register(0x203, 0x00);  //normal operation
    //delay_ms(200);  

    sensor_write_register(0x200, 0x01);  //STANDBY
    //delay_ms(20);  

    sensor_write_register(0x202, 0x01); /* Master mode stop */
    
    sensor_write_register(0x209, 0x00);  //org
    
    sensor_write_register(0x20c, 0x00);     
    
    sensor_write_register(0x212, 0x86);
    sensor_write_register(0x213, 0x01);
    sensor_write_register(0x216,0x89);
       //VMAX
    sensor_write_register(0x218, 0x08);     //0x708 = 1800  //org
    sensor_write_register(0x219, 0x07);
       //HMAX
    sensor_write_register(0x21b, 0x3c);     //0x23c = 572
    sensor_write_register(0x21c, 0x02);
    sensor_write_register(0x21e, 0x08);
    sensor_write_register(0x221, 0xf0);
    sensor_write_register(0x224, 0x00); 
    sensor_write_register(0x22e, 0xda);  
    sensor_write_register(0x231, 0x1c);  
    sensor_write_register(0x243, 0x00);  
    sensor_write_register(0x244, 0xb5);// dck2en
    sensor_write_register(0x246, 0x44);   //org    
    sensor_write_register(0x249, 0x0a);    /* XVS & XHS output start */
    
    sensor_write_register(0x254, 0x61);     //org
    
    sensor_write_register(0x261, 0x81); 
    sensor_write_register(0x262, 0x00);  
    sensor_write_register(0x29d, 0x82); 
    sensor_write_register(0x29f, 0x01); 
    sensor_write_register(0x2b3, 0x91); 
    sensor_write_register(0x2c8, 0xbc); 
    sensor_write_register(0x2c9, 0x00); 
    sensor_write_register(0x2ca, 0xbc); 
    
    sensor_write_register(0x2cb, 0x00); 
    
    sensor_write_register(0x2f0, 0x86); 
    sensor_write_register(0x2f1, 0x01); 
    sensor_write_register(0x2f4, 0x86); 
    sensor_write_register(0x2f5, 0x01);
    sensor_write_register(0x2f8, 0x86);
    sensor_write_register(0x2f9, 0x01);
    
    sensor_write_register(0x303, 0x07);
    sensor_write_register(0x304, 0x07);
    sensor_write_register(0x306, 0x07);
    sensor_write_register(0x307, 0x00);
    sensor_write_register(0x308, 0x00);
    sensor_write_register(0x309, 0x00);
    sensor_write_register(0x30a, 0x00);
    sensor_write_register(0x30b, 0x00);
    sensor_write_register(0x310, 0xf2);
    sensor_write_register(0x311, 0x03);
    sensor_write_register(0x312, 0xeb);
    sensor_write_register(0x313, 0x07);
    sensor_write_register(0x314, 0xed);
    sensor_write_register(0x315, 0x07);    
    sensor_write_register(0x326, 0x91);
    sensor_write_register(0x333, 0x12);
    sensor_write_register(0x334, 0x10);
    sensor_write_register(0x335, 0x12);
    sensor_write_register(0x336, 0x10);
    sensor_write_register(0x33a, 0x0c);
    sensor_write_register(0x33b, 0x0c);
    sensor_write_register(0x33c, 0x0c);
    sensor_write_register(0x33d, 0x0c);
    sensor_write_register(0x340, 0x00);
    sensor_write_register(0x341, 0x00);
    sensor_write_register(0x344, 0x1e);
    sensor_write_register(0x349, 0x55);
    sensor_write_register(0x34b, 0x99);
    sensor_write_register(0x34c, 0x99);
    sensor_write_register(0x354, 0xe7);
    sensor_write_register(0x35a, 0x04);
    sensor_write_register(0x36c, 0x20);
    sensor_write_register(0x36d, 0x09);
    sensor_write_register(0x370, 0x40);
    sensor_write_register(0x371, 0x14);
    sensor_write_register(0x379, 0x94);
    sensor_write_register(0x37a, 0x06);
    sensor_write_register(0x3f6, 0x00);     // new added
    
    sensor_write_register(0x401, 0x3c);
    sensor_write_register(0x402, 0x01);
    sensor_write_register(0x403, 0x0e);
    sensor_write_register(0x413, 0x05);
    sensor_write_register(0x41f, 0x05);
    sensor_write_register(0x45f, 0x03);
    sensor_write_register(0x469, 0x03);
    sensor_write_register(0x4b6, 0x03);
    sensor_write_register(0x4ba, 0x01);
    sensor_write_register(0x4c4, 0x01);
    sensor_write_register(0x4cb, 0x01);
    sensor_write_register(0x4d9, 0x80);
    sensor_write_register(0x4dc, 0xb0);
    sensor_write_register(0x4dd, 0x13);

    sensor_write_register(0x52a, 0xff);
    sensor_write_register(0x52b, 0xff);
    sensor_write_register(0x52c, 0xff);
    sensor_write_register(0x52d, 0xff);
    sensor_write_register(0x52e, 0xff);
    sensor_write_register(0x52f, 0xff);
    sensor_write_register(0x535, 0x50);
    sensor_write_register(0x536, 0x80);
    sensor_write_register(0x537, 0x1b);
    sensor_write_register(0x53c, 0x01);
    sensor_write_register(0x53d, 0x03);

    sensor_write_register(0x200, 0x00); /* Standby cancel */
    delay_ms(20);
    sensor_write_register(0x244, 0xf5); /* DCKRST = 1 */
    sensor_write_register(0x244, 0xb5); /* DCKRST = 0 */
    sensor_write_register(0x202, 0x00); /* Master mode start */

    printf("-------Sony IMX123 Sensor QXGA 60fps Linear Initial OK!-------\n");
}


// INCLK= 37.125MHz , QXGA mode 
// 2376Mbps Normal mode 30fps
// Serial LVDS 8ch output 12bit 297Mbps/ch
// Vmax = 1650, Hmax = 1500
void sensor_linear_qxga_30_init()
{
    sensor_write_register(0x203, 0x01);  //soft reset
    //delay_ms(200);  
    sensor_write_register(0x203, 0x00);  //normal operation
    //delay_ms(200);  

    sensor_write_register(0x200, 0x01);  //STANDBY
    //delay_ms(200);  

    sensor_write_register(0x202,0x01);  /* Master mode stop */
    sensor_write_register(0x212,0x86);
    sensor_write_register(0x213,0x01);
    sensor_write_register(0x216,0x89);
    sensor_write_register(0x218,0x72);      // VMax
    sensor_write_register(0x21b,0xdc);
    sensor_write_register(0x21c,0x05);
    sensor_write_register(0x21e,0x08);
    sensor_write_register(0x221,0x08);
    sensor_write_register(0x224,0x08);
    sensor_write_register(0x227,0x08);
    sensor_write_register(0x244,0xb5);//0x35 //DCK2EN
    sensor_write_register(0x249,0x0a);
    sensor_write_register(0x261,0xa1);
    sensor_write_register(0x262,0x00);
    sensor_write_register(0x29d,0x82);
    sensor_write_register(0x29f,0x01);
    sensor_write_register(0x2b3,0x91);
    sensor_write_register(0x2f0,0x86);
    sensor_write_register(0x2f1,0x01);
    sensor_write_register(0x2f4,0x86);
    sensor_write_register(0x2f5,0x01);
    sensor_write_register(0x2f8,0x86);
    sensor_write_register(0x2f9,0x01);

    sensor_write_register(0x303,0x3f);
    sensor_write_register(0x304,0x06);
    sensor_write_register(0x306,0x07);
    sensor_write_register(0x307,0x00);
    sensor_write_register(0x308,0x00);
    sensor_write_register(0x309,0x00);
    sensor_write_register(0x30a,0x00);
    sensor_write_register(0x30b,0x00);
    sensor_write_register(0x310,0xf2);
    sensor_write_register(0x311,0x03);
    sensor_write_register(0x312,0xeb);
    sensor_write_register(0x313,0x07);
    sensor_write_register(0x314,0xed);
    sensor_write_register(0x315,0x07);
    sensor_write_register(0x326,0x91);
    sensor_write_register(0x333,0x12);
    sensor_write_register(0x334,0x10);
    sensor_write_register(0x335,0x12);
    sensor_write_register(0x336,0x10);
    sensor_write_register(0x33a,0x0c);
    sensor_write_register(0x33b,0x0c);
    sensor_write_register(0x33c,0x0c);
    sensor_write_register(0x33d,0x0c);
    sensor_write_register(0x340,0x00);
    sensor_write_register(0x341,0x00);
    sensor_write_register(0x344,0x1e);
    sensor_write_register(0x349,0x55);
    sensor_write_register(0x34b,0x99);
    sensor_write_register(0x34c,0x99);
    sensor_write_register(0x354,0xe7);
    sensor_write_register(0x35a,0x04);
    sensor_write_register(0x36c,0x20);
    sensor_write_register(0x36d,0x09);
    sensor_write_register(0x371,0x20);
    sensor_write_register(0x379,0x94);
    sensor_write_register(0x37a,0x06);
    sensor_write_register(0x3f6,0x11);

    sensor_write_register(0x401,0x3c);
    sensor_write_register(0x402,0x01);
    sensor_write_register(0x403,0x0e);
    sensor_write_register(0x413,0x05);
    sensor_write_register(0x41f,0x05);
    sensor_write_register(0x45f,0x03);
    sensor_write_register(0x469,0x03);
    sensor_write_register(0x4b6,0x03);
    sensor_write_register(0x4ba,0x01);
    sensor_write_register(0x4c4,0x01);
    sensor_write_register(0x4cb,0x01);
    sensor_write_register(0x4d9,0x80);
    sensor_write_register(0x4dc,0xb0);
    sensor_write_register(0x4dd,0x13);

    sensor_write_register(0x52a,0xff);
    sensor_write_register(0x52b,0xff);
    sensor_write_register(0x52c,0xff);
    sensor_write_register(0x52d,0xff);
    sensor_write_register(0x52e,0xff);
    sensor_write_register(0x52f,0xff);
    sensor_write_register(0x535,0x50);
    sensor_write_register(0x536,0x80);
    sensor_write_register(0x537,0x1b);
    sensor_write_register(0x53c,0x01);
    sensor_write_register(0x53d,0x03);

    sensor_write_register(0x200, 0x00); /* Standby cancel */
    delay_ms(20);
    sensor_write_register(0x244, 0xf5); /* DCKRST = 1 */
    sensor_write_register(0x244, 0xb5); /* DCKRST = 0 */
    sensor_write_register(0x202, 0x00); /* Master mode start */

    printf("-------Sony IMX123 Sensor QXGA 30 Linear Initial OK!-------\n");
}


// 37.125MHz clock input
// 2970Mbps (=371.25Mbps/ch x 8ch)
void sensor_wdr_qxga_30_init()
{
    sensor_write_register(0x203, 0x01);  //soft reset
    //delay_ms(200);  
    sensor_write_register(0x203, 0x00);  //normal operation
    //delay_ms(200);  

    sensor_write_register(0x200, 0x01);  //STANDBY
    //delay_ms(200);  

    sensor_write_register(0x202, 0x01); /* Master mode stop */
    
    sensor_write_register(0x209, 0x00);  //org
    sensor_write_register(0x20c, 0x14);     //org
    sensor_write_register(0x212, 0x0e);
    sensor_write_register(0x213, 0x01);
       //VMAX
    sensor_write_register(0x218, 0x08);     //0x708 = 1800  //org
    sensor_write_register(0x219, 0x07);
       //HMAX
    sensor_write_register(0x21b, 0x3c);     //0x23c = 572
    sensor_write_register(0x21c, 0x02);
    sensor_write_register(0x21e, 0x08);
    sensor_write_register(0x221, 0xf0);
    sensor_write_register(0x224, 0x00); 
    sensor_write_register(0x22e, 0xda);  
    sensor_write_register(0x231, 0x1c);  
    sensor_write_register(0x243, 0x00);  
    sensor_write_register(0x244, 0xb5);// dck2en
    sensor_write_register(0x246, 0x44);   //org    
    sensor_write_register(0x249, 0x0a);       
    sensor_write_register(0x254, 0x6d);     //org
    sensor_write_register(0x261, 0x81); 
    sensor_write_register(0x262, 0x00);  
    sensor_write_register(0x29d, 0x82); 
    sensor_write_register(0x29f, 0x01); 
    sensor_write_register(0x2b3, 0x91); 
    sensor_write_register(0x2c8, 0xbc); 
    sensor_write_register(0x2c9, 0x00); 
    sensor_write_register(0x2ca, 0xbc); 
    sensor_write_register(0x2cb, 0x20); 
    sensor_write_register(0x2f0, 0x0e); 
    sensor_write_register(0x2f1, 0x01); 
    sensor_write_register(0x2f4, 0x0e); 
    sensor_write_register(0x2f5, 0x01);
    sensor_write_register(0x2f8, 0x0e);
    sensor_write_register(0x2f9, 0x01);

    sensor_write_register(0x303, 0x07);
    sensor_write_register(0x304, 0x07);
    sensor_write_register(0x306, 0x07);
    sensor_write_register(0x307, 0x00);
    sensor_write_register(0x308, 0x00);
    sensor_write_register(0x309, 0x00);
    sensor_write_register(0x30a, 0x00);
    sensor_write_register(0x30b, 0x00);
    sensor_write_register(0x310, 0xf2);
    sensor_write_register(0x311, 0x03);
    sensor_write_register(0x312, 0xeb);
    sensor_write_register(0x313, 0x07);
    sensor_write_register(0x314, 0xed);
    sensor_write_register(0x315, 0x07);    
    sensor_write_register(0x326, 0x91);
    sensor_write_register(0x333, 0x12);
    sensor_write_register(0x334, 0x10);
    sensor_write_register(0x335, 0x12);
    sensor_write_register(0x336, 0x10);
    sensor_write_register(0x33a, 0x0c);
    sensor_write_register(0x33b, 0x0c);
    sensor_write_register(0x33c, 0x0c);
    sensor_write_register(0x33d, 0x0c);
    sensor_write_register(0x340, 0x00);
    sensor_write_register(0x341, 0x00);
    sensor_write_register(0x344, 0x1e);
    sensor_write_register(0x349, 0x55);
    sensor_write_register(0x34b, 0x99);
    sensor_write_register(0x34c, 0x99);
    sensor_write_register(0x354, 0xe7);
    sensor_write_register(0x35a, 0x04);
    sensor_write_register(0x36c, 0x20);
    sensor_write_register(0x36d, 0x09);
    sensor_write_register(0x370, 0x40);
    sensor_write_register(0x371, 0x14);
    sensor_write_register(0x379, 0x94);
    sensor_write_register(0x37a, 0x06);

    sensor_write_register(0x3f6, 0x00);     // new added
    
    sensor_write_register(0x401, 0x3c);
    sensor_write_register(0x402, 0x01);
    sensor_write_register(0x403, 0x0e);
    sensor_write_register(0x413, 0x05);
    sensor_write_register(0x41f, 0x05);
    sensor_write_register(0x45f, 0x03);
    sensor_write_register(0x469, 0x03);
    sensor_write_register(0x4b6, 0x03);
    sensor_write_register(0x4ba, 0x01);
    sensor_write_register(0x4c4, 0x01);
    sensor_write_register(0x4cb, 0x01);
    sensor_write_register(0x4d9, 0x80);
    sensor_write_register(0x4dc, 0xb0);
    sensor_write_register(0x4dd, 0x13);

    sensor_write_register(0x52a, 0xff);
    sensor_write_register(0x52b, 0xff);
    sensor_write_register(0x52c, 0xff);
    sensor_write_register(0x52d, 0xff);
    sensor_write_register(0x52e, 0xff);
    sensor_write_register(0x52f, 0xff);
    sensor_write_register(0x535, 0x50);
    sensor_write_register(0x536, 0x80);
    sensor_write_register(0x537, 0x1b);
    sensor_write_register(0x53c, 0x01);
    sensor_write_register(0x53d, 0x03);

    sensor_write_register(0x200, 0x00); /* Standby cancel */
    delay_ms(20);
    sensor_write_register(0x244, 0xf5); /* DCKRST = 1 */
    sensor_write_register(0x244, 0xb5); /* DCKRST = 0 */
    sensor_write_register(0x202, 0x00); /* Master mode start */

    printf("-------Sony IMX123 Sensor QXGA 30 WDR Initial OK!-------\n");

}


// HD 1080p mode; 1782Mbps Normal mode 60fps
// INCK = 37.125MHz; Vmax = 1125; Hmax = 1100;
// Serial LVDS 8ch output 12 bit 
void sensor_linear_1080p60_init()
{
    sensor_write_register(0x203, 0x01);  //soft reset
    //delay_ms(200);  
    sensor_write_register(0x203, 0x00);  //normal operation
    //delay_ms(200);  

    sensor_write_register(0x200, 0x01);  //STANDBY
    //delay_ms(200);  

    sensor_write_register(0x202, 0x01); /* Master mode stop */
    sensor_write_register(0x207, 0x10);    //WINMODE 
    sensor_write_register(0x212, 0x86);    // 
    sensor_write_register(0x213, 0x01);    // 
    sensor_write_register(0x216, 0x89);
    sensor_write_register(0x218, 0x65);    // 
    sensor_write_register(0x219, 0x04);    //VMAX 
    sensor_write_register(0x21B, 0x4c);
    sensor_write_register(0x21C, 0x04);    //HMAX 
    sensor_write_register(0x21E, 0x08);    //shs1
    sensor_write_register(0x221, 0x08);    //shs2
    sensor_write_register(0x224, 0x08); 
    sensor_write_register(0x227, 0x08); 
    sensor_write_register(0x244, 0xb5);    //ODBIT 12BIT;OMODE LOW LVDS;//DCK2EN
    sensor_write_register(0x249, 0x0a); 
    sensor_write_register(0x261, 0x81); 
    sensor_write_register(0x262, 0x00); 
    sensor_write_register(0x29d, 0x82); 
    sensor_write_register(0x29f, 0x01); 
    sensor_write_register(0x2b3, 0x91); 
    sensor_write_register(0x2f0, 0x86); 
    sensor_write_register(0x2f1, 0x01); 
    sensor_write_register(0x2f4, 0x86); 
    sensor_write_register(0x2f5, 0x01); 
    sensor_write_register(0x2f8, 0x86); 
    sensor_write_register(0x2f9, 0x01); 

    sensor_write_register(0x303, 0x64);
    sensor_write_register(0x304, 0x04);
    sensor_write_register(0x306, 0x07);
    sensor_write_register(0x307, 0x00);
    sensor_write_register(0x308, 0x00);
    sensor_write_register(0x309, 0x00);
    sensor_write_register(0x30a, 0x00);
    sensor_write_register(0x30b, 0x00);
    sensor_write_register(0x310, 0xf2);
    sensor_write_register(0x311, 0x03);
    sensor_write_register(0x312, 0xeb);
    sensor_write_register(0x313, 0x07);
    sensor_write_register(0x314, 0xed);
    sensor_write_register(0x315, 0x07);    
    sensor_write_register(0x326, 0x91);
    sensor_write_register(0x330, 0x4d);
    sensor_write_register(0x333, 0x12);
    sensor_write_register(0x334, 0x10);
    sensor_write_register(0x335, 0x12);
    sensor_write_register(0x336, 0x10);
    sensor_write_register(0x33a, 0x0c);
    sensor_write_register(0x33b, 0x0c);
    sensor_write_register(0x33c, 0x0c);
    sensor_write_register(0x33d, 0x0c);
    sensor_write_register(0x340, 0x00);
    sensor_write_register(0x341, 0x00);
    sensor_write_register(0x344, 0x1e);
    sensor_write_register(0x349, 0x55);
    sensor_write_register(0x34b, 0x99);
    sensor_write_register(0x34c, 0x99);
    sensor_write_register(0x354, 0xe7);
    sensor_write_register(0x35a, 0x04);
    sensor_write_register(0x36c, 0x20);
    sensor_write_register(0x36d, 0x09);
    sensor_write_register(0x379, 0x94);
    sensor_write_register(0x37a, 0x06);
    sensor_write_register(0x3eb, 0x44);
    sensor_write_register(0x3f6, 0x11);

    sensor_write_register(0x401, 0x3c);
    sensor_write_register(0x402, 0x01);
    sensor_write_register(0x403, 0x0e);
    sensor_write_register(0x413, 0x05);
    sensor_write_register(0x41f, 0x05);
    sensor_write_register(0x45f, 0x03);
    sensor_write_register(0x469, 0x03);
    sensor_write_register(0x4b6, 0x03);
    sensor_write_register(0x4ba, 0x01);
    sensor_write_register(0x4c4, 0x01);
    sensor_write_register(0x4cb, 0x01);
    sensor_write_register(0x4d9, 0x80);
    sensor_write_register(0x4dc, 0xb0);
    sensor_write_register(0x4dd, 0x13);

    sensor_write_register(0x52a, 0xff);
    sensor_write_register(0x52b, 0xff);
    sensor_write_register(0x52c, 0xff);
    sensor_write_register(0x52d, 0xff);
    sensor_write_register(0x52e, 0xff);
    sensor_write_register(0x52f, 0xff);
    sensor_write_register(0x535, 0x50);
    sensor_write_register(0x536, 0x80);
    sensor_write_register(0x537, 0x1b);
    sensor_write_register(0x53c, 0x01);
    sensor_write_register(0x53d, 0x03);

    sensor_write_register(0x200, 0x00); /* Standby cancel */
    delay_ms(20);
    sensor_write_register(0x244, 0xf5); /* DCKRST = 1 */
    sensor_write_register(0x244, 0xb5); /* DCKRST = 0 */
    sensor_write_register(0x202, 0x00); /* Master mode start */

    printf("-------Sony IMX123 Sensor 1080p60 Linear Initial OK!-------\n");
}

