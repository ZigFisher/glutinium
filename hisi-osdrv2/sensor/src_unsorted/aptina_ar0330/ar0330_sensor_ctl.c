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

const unsigned char sensor_i2c_addr  = 0x20;  /* I2C Address of AR0330 */
const unsigned int  sensor_addr_byte = 2;    /* ADDR byte of AR0330 */
const unsigned int  sensor_data_byte = 2;    /* DATA byte of AR0330 */
static int g_fd = -1;

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

void sensor_init_2M_1080p();
void sensor_init_3M_1536p();
void sensor_init_3M_1296p();

void sensor_init()
{
    sensor_i2c_init();
    
    switch (gu8SensorImageMode)
    {
        case 1 :
            {
                sensor_init_2M_1080p();
                bSensorInit = HI_TRUE;
                break;
            }              
        case 2 :
            {
                sensor_init_3M_1536p();
                bSensorInit = HI_TRUE;
                break;
            } 
        case 3 :
            {
                sensor_init_3M_1296p();
                bSensorInit = HI_TRUE;
                break;
            }         
        default :
            {
                printf("Not support!\n");              
            }
    }
}

void sensor_exit()
{
    sensor_i2c_exit();

    return;
}

void sensor_init_2M_1080p()
{
    /*[TEST 1920x1080 30fps 12bit - Mipi 2lane, MCLK=24MHz]*/
    sensor_write_register(0x301A,0x0001); /*RESET_REGISTER = 4312*/
    delay_ms(100);                           /*Delay 100ms*/
                            
    sensor_write_register(0x3052,0xA114);         
    sensor_write_register(0x304A,0x0070); 
    delay_ms(100);
                               
    sensor_write_register(0x31AE,0x0202); /*Output Interface Configured to 2 lane mipi*/            
    sensor_write_register(0x301A,0x0058); /*Disable streaming*/            
    delay_ms(34);
                             
    sensor_write_register(0x3064,0x1802); /*Disable Embedded Data*/            
    sensor_write_register(0x3078,0x0001); 
                                         
    /*OTPM V5*/                                
    sensor_write_register(0x3ED2,0x0146);     
    sensor_write_register(0x3EDA,0x88BC);     
    sensor_write_register(0x3EDC,0xAA63);     
                                         
    /*Toggle Flash on Each Frame*/         
    sensor_write_register(0x3046,0x0000); /* Enable Flash Pin 0x4038*/            
    sensor_write_register(0x3048,0x8480); /* Flash Pulse Length*/            
    sensor_write_register(0x31E0,0x0203);
                                       
    /*Serial 12-bit PLL 98MHz*/            
    sensor_write_register(0x302A,0x0006); /* vt_pix_clk_div*/            
    sensor_write_register(0x302C,0x0002); /* vt_sys_clk_div*/            
    sensor_write_register(0x302e,0x0002); /* pre_pll_clk_div*/            
    sensor_write_register(0x3030,0x0031); /* pll_multiplier*/            
    sensor_write_register(0x3036,0x000c); /* op_pix_clk_div*/             
    sensor_write_register(0x3038,0x0001); /* op_sys_clk_div*/            
    sensor_write_register(0x31AC,0x0C0C); /* data_format is 12-bit*/       
    sensor_write_register(0x31B0,0x002D); /* FRAME PREAMBLE */ 
    sensor_write_register(0x31B2,0x0012); /* LINE PREAMBLE */
    sensor_write_register(0x31B4,0x3B44); /* MIPI TIMING 0*/
    sensor_write_register(0x31B6,0x314D); /* MIPI TIMING 1*/
    sensor_write_register(0x31B8,0x2089); /* MIPI TIMING 2*/
    sensor_write_register(0x31BA,0x0206); /* MIPI TIMING 3*/
    sensor_write_register(0x31BC,0x8005); /* MIPI TIMING 4*/
    sensor_write_register(0x31BE,0x2003);    
    delay_ms(10);                      
                                       
    /*ARRAY READOUT SETTINGS*/            
    sensor_write_register(0x3004,0x00C6); /* X_ADDR_START*/            
    sensor_write_register(0x3008,0x0845); /* X_ADDR_END*/            
    sensor_write_register(0x3002,0x00EA); /* Y_ADDR_START*/    
    sensor_write_register(0x3006,0x0521); /* Y_ADDR_END*/            
                                       
    /*Sub-sampling*/                     
    sensor_write_register(0x30A2,0x0001); /* X_ODD_INCREMENT*/            
    sensor_write_register(0x30A6,0x0001); /* Y_ODD_INCREMENT*/    
    sensor_write_register(0x3040,0x3000); /* Row/column Bin*/
    sensor_write_register(0x3ED4,0x8F6C);
    sensor_write_register(0x3ED6,0x66CC);
                                      
    /*Frame-Timing*/                     
    sensor_write_register(0x300C,0x04E0); /* LINE_LENGTH_PCK*/            
    sensor_write_register(0x300A,0x051C); /* FRAME_LENGTH_LINES*/            
    sensor_write_register(0x3014,0x0000); /* FINE_INTEGRATION_TIME*/            
    sensor_write_register(0x3012,0x0514); /* Coarse_Integration_Time*/            
    sensor_write_register(0x3042,0x0000); /* EXTRA_DELAY*/            
    sensor_write_register(0x30BA,0x002C); /* Digital_Ctrl_Adc_High_Speed*/    
    
    delay_ms(30);
    sensor_write_register(0x3088,0x80BA);
    sensor_write_register(0x3086,0x0253);                                       
    sensor_write_register(0x301A,0x0004); /* Start Streaming*/    

    printf("Aptina AR0330 sensor 2M-1080p 30fps init success!\n");
}

void sensor_init_3M_1536p()
{
#ifndef HI_FPGA
    /*[TEST 2048x1536 30fps 12bit - Mipi 2lane, MCLK=24MHz]*/    
    sensor_write_register(0x301A, 0x0001); /*RESET_REGISTER = 4312*/
    delay_ms(100);                         /*Delay 100ms*/
                             
    sensor_write_register(0x3052, 0xA114);         
    sensor_write_register(0x304A, 0x0070); 
    delay_ms(100); 
                                    
    sensor_write_register(0x31AE, 0x0202); /*Output Interface Configured to 2 lane mipi*/          
    sensor_write_register(0x301A, 0x0058); /*Disable streaming*/           
    delay_ms(34); 
                     
    sensor_write_register(0x3064, 0x1802); /*Disable Embedded Data*/           
    sensor_write_register(0x3078, 0x0001); 
                                 
    /*OTPM V5*/                 
    sensor_write_register(0x3ED2, 0x0146);     
    sensor_write_register(0x3EDA, 0x88BC);     
    sensor_write_register(0x3EDC, 0xAA63);     
                                 
    /*Toggle Flash on Each Frame*/          
    sensor_write_register(0x3046, 0x0000); /* Enable Flash Pin 0x4038*/         
    sensor_write_register(0x3048, 0x8480); /* Flash Pulse Length*/       
    sensor_write_register(0x31E0, 0x0203);
                                 
    /*Serial 12-bit PLL 128MHz*/             
    sensor_write_register(0x302A, 0x0006); /* vt_pix_clk_div*/           
    sensor_write_register(0x302C, 0x0002); /* vt_sys_clk_div */      
    sensor_write_register(0x302e, 0x0001); /* pre_pll_clk_div*/          
    sensor_write_register(0x3030, 0x0020); /* pll_multiplier*/           
    sensor_write_register(0x3036, 0x000c); /* op_pix_clk_div */          
    sensor_write_register(0x3038, 0x0001); /* op_sys_clk_div */      
    sensor_write_register(0x31AC, 0x0C0C); /* data_format is 12-bit*/
    sensor_write_register(0x31B0, 0x0044); /* FRAME PREAMBLE */ 
    sensor_write_register(0x31B2, 0x0029); /* LINE PREAMBLE */
    sensor_write_register(0x31B4, 0x1A54); /* MIPI TIMING 0*/
    sensor_write_register(0x31B6, 0x11D3); /* MIPI TIMING 1*/
    sensor_write_register(0x31B8, 0x3049); /* MIPI TIMING 2*/
    sensor_write_register(0x31BA, 0x0208); /* MIPI TIMING 3*/
    sensor_write_register(0x31BC, 0x0007); /* MIPI TIMING 4*/
    //sensor_write_register(0x31BE, 0x2003);
    delay_ms(10);                
                                
    /*ARRAY READOUT SETTINGS*/              
    sensor_write_register(0x3004, 0x0086); /* X_ADDR_START*/         
    sensor_write_register(0x3008, 0x0885); /* X_ADDR_END*/           
    sensor_write_register(0x3002, 0x0006); /* Y_ADDR_START*/ 
    sensor_write_register(0x3006, 0x0605); /* Y_ADDR_END*/           
                                 
    /*Sub-sampling*/                 
    sensor_write_register(0x30A2, 0x0001); /* X_ODD_INCREMENT*/          
    sensor_write_register(0x30A6, 0x0001); /* Y_ODD_INCREMENT*/  
    sensor_write_register(0x3040, 0x0000);
    sensor_write_register(0x3ED4, 0x8F6C);
    sensor_write_register(0x3ED6, 0x66CC);
                                 
    /*Frame-Timing*/                 
    sensor_write_register(0x300C, 0x04E0); /* LINE_LENGTH_PCK*/          
    sensor_write_register(0x300A, 0x06AD); /* FRAME_LENGTH_LINES*/           
    sensor_write_register(0x3014, 0x0000); /* FINE_INTEGRATION_TIME*/            
    sensor_write_register(0x3012, 0x06AC); /* Coarse_Integration_Time*/          
    sensor_write_register(0x3042, 0x0000); /* EXTRA_DELAY*/          
    sensor_write_register(0x30BA, 0x002C); /* Digital_Ctrl_Adc_High_Speed*/          

    //sensor_write_register(0x301A, 0x0000);    
    delay_ms(30);
    sensor_write_register(0x3088, 0x80BA);
    sensor_write_register(0x3086, 0x0253);
    sensor_write_register(0x301A, 0x0004); /* Start Streaming*/          

    printf("Aptina AR0330 sensor 3M-1536p 30fps init success!\n"); 
#else
    /*[TEST 2048x1536 25fps 12bit - Mipi 2lane, MCLK=24MHz]*/    
    sensor_write_register(0x301A, 0x0001); /*RESET_REGISTER = 4312*/
    delay_ms(100);                         /*Delay 100ms*/
                             
    sensor_write_register(0x3052, 0xA114);         
    sensor_write_register(0x304A, 0x0070); 
    delay_ms(100); 
                                    
    sensor_write_register(0x31AE, 0x0202); /*Output Interface Configured to 2 lane mipi*/          
    sensor_write_register(0x301A, 0x0058); /*Disable streaming*/           
    delay_ms(34); 
                     
    sensor_write_register(0x3064, 0x1802); /*Disable Embedded Data*/           
    sensor_write_register(0x3078, 0x0001); 
                                 
    /*OTPM V5*/                 
    sensor_write_register(0x3ED2, 0x0146);     
    sensor_write_register(0x3EDA, 0x88BC);     
    sensor_write_register(0x3EDC, 0xAA63);     
                                 
    /*Toggle Flash on Each Frame*/          
    sensor_write_register(0x3046, 0x4038); /* Enable Flash Pin*/         
    sensor_write_register(0x3048, 0x8480); /* Flash Pulse Length*/       
    sensor_write_register(0x31E0, 0x0203);
                                 
    /*Serial 12-bit PLL 98MHz*/             
    sensor_write_register(0x302A, 0x0006); /* vt_pix_clk_div*/           
    sensor_write_register(0x302C, 0x0002); /* vt_sys_clk_div */      
    sensor_write_register(0x302e, 0x0002); /* pre_pll_clk_div*/          
    sensor_write_register(0x3030, 0x0031); /* pll_multiplier*/           
    sensor_write_register(0x3036, 0x000c); /* op_pix_clk_div */          
    sensor_write_register(0x3038, 0x0001); /* op_sys_clk_div */      
    sensor_write_register(0x31AC, 0x0C0C); /* data_format is 12-bit*/
    sensor_write_register(0x31B0, 0x0044); /* FRAME PREAMBLE */ 
    sensor_write_register(0x31B2, 0x0029); /* LINE PREAMBLE */
    sensor_write_register(0x31B4, 0x1A54); /* MIPI TIMING 0*/
    sensor_write_register(0x31B6, 0x11D3); /* MIPI TIMING 1*/
    sensor_write_register(0x31B8, 0x3049); /* MIPI TIMING 2*/
    sensor_write_register(0x31BA, 0x0208); /* MIPI TIMING 3*/
    sensor_write_register(0x31BC, 0x0007); /* MIPI TIMING 4*/
    sensor_write_register(0x31BE, 0x2003);
    delay_ms(10);                
                                
    /*ARRAY READOUT SETTINGS*/              
    sensor_write_register(0x3004, 0x0086); /* X_ADDR_START*/         
    sensor_write_register(0x3008, 0x0885); /* X_ADDR_END*/           
    sensor_write_register(0x3002, 0x0006); /* Y_ADDR_START*/ 
    sensor_write_register(0x3006, 0x0605); /* Y_ADDR_END*/           
                                 
    /*Sub-sampling*/                 
    sensor_write_register(0x30A2, 0x0001); /* X_ODD_INCREMENT*/          
    sensor_write_register(0x30A6, 0x0001); /* Y_ODD_INCREMENT*/  
    sensor_write_register(0x3040, 0x0000);
    sensor_write_register(0x3ED4, 0x8F6C);
    sensor_write_register(0x3ED6, 0x66CC);
                                 
    /*Frame-Timing*/                 
    sensor_write_register(0x300C, 0x04E0); /* LINE_LENGTH_PCK*/          
    sensor_write_register(0x300A, 0x0622); /* FRAME_LENGTH_LINES*/           
    sensor_write_register(0x3014, 0x0000); /* FINE_INTEGRATION_TIME*/            
    sensor_write_register(0x3012, 0x05DC); /* Coarse_Integration_Time*/          
    sensor_write_register(0x3042, 0x0000); /* EXTRA_DELAY*/          
    sensor_write_register(0x30BA, 0x002C); /* Digital_Ctrl_Adc_High_Speed*/          
    
    delay_ms(30);
    sensor_write_register(0x3088, 0x80BA);
    sensor_write_register(0x3086, 0x0253);
    sensor_write_register(0x301A, 0x0004); /* Start Streaming*/          

    printf("Aptina AR0330 sensor 3M-1536p 25fps init success!\n");
#endif    
}

void sensor_init_3M_1296p()
{
    /*[TEST 2304x1296 30fps 12bit - Mipi 2lane, MCLK=24MHz]*/

    sensor_write_register(0x301A, 0x0001); /*RESET_REGISTER = 4312*/
    delay_ms(100);                         /*Delay 100ms*/
                             
    sensor_write_register(0x3052, 0xA114);         
    sensor_write_register(0x304A, 0x0070); 
    delay_ms(100); 
                                    
    sensor_write_register(0x31AE, 0x0202); /*Output Interface Configured to 2 lane mipi*/          
    sensor_write_register(0x301A, 0x0058); /*Disable streaming*/           
    delay_ms(34); 
                     
    sensor_write_register(0x3064, 0x1802); /*Disable Embedded Data*/           
    sensor_write_register(0x3078, 0x0001); 
                                 
    /*OTPM V5*/                 
    sensor_write_register(0x3ED2, 0x0146);     
    sensor_write_register(0x3EDA, 0x88BC);     
    sensor_write_register(0x3EDC, 0xAA63);     
                                 
    /*Toggle Flash on Each Frame*/          
    sensor_write_register(0x3046, 0x0000); /* Enable Flash Pin 0x4038*/         
    sensor_write_register(0x3048, 0x8480); /* Flash Pulse Length*/       
    sensor_write_register(0x31E0, 0x0203);
                                 
    /*Serial 12-bit PLL 98MHz*/             
    sensor_write_register(0x302A, 0x0006); /* vt_pix_clk_div*/           
    sensor_write_register(0x302C, 0x0002); /* vt_sys_clk_div */      
    sensor_write_register(0x302e, 0x0002); /* pre_pll_clk_div*/          
    sensor_write_register(0x3030, 0x0031); /* pll_multiplier*/           
    sensor_write_register(0x3036, 0x000c); /* op_pix_clk_div */          
    sensor_write_register(0x3038, 0x0001); /* op_sys_clk_div */      
    sensor_write_register(0x31AC, 0x0C0C); /* data_format is 12-bit*/
    sensor_write_register(0x31B0, 0x002D); /* FRAME PREAMBLE */ 
    sensor_write_register(0x31B2, 0x0012); /* LINE PREAMBLE */
    sensor_write_register(0x31B4, 0x3B44); /* MIPI TIMING 0*/
    sensor_write_register(0x31B6, 0x314D); /* MIPI TIMING 1*/
    sensor_write_register(0x31B8, 0x2089); /* MIPI TIMING 2*/
    sensor_write_register(0x31BA, 0x0206); /* MIPI TIMING 3*/
    sensor_write_register(0x31BC, 0x8005); /* MIPI TIMING 4*/
    sensor_write_register(0x31BE, 0x2003);     
    delay_ms(10);                
                                
    /*ARRAY READOUT SETTINGS*/              
    sensor_write_register(0x3004, 0x0006); /* X_ADDR_START*/         
    sensor_write_register(0x3008, 0x0905); /* X_ADDR_END*/           
    sensor_write_register(0x3002, 0x0078); /* Y_ADDR_START*/ 
    sensor_write_register(0x3006, 0x0587); /* Y_ADDR_END*/           
                                 
    /*Sub-sampling*/                 
    sensor_write_register(0x30A2, 0x0001); /* X_ODD_INCREMENT*/          
    sensor_write_register(0x30A6, 0x0001); /* Y_ODD_INCREMENT*/  
    sensor_write_register(0x3040, 0x0000);
    sensor_write_register(0x3ED4, 0x8F6C);
    sensor_write_register(0x3ED6, 0x66CC);
                                 
    /*Frame-Timing*/                 
    sensor_write_register(0x300C, 0x04E0); /* LINE_LENGTH_PCK*/          
    sensor_write_register(0x300A, 0x051C); /* FRAME_LENGTH_LINES*/           
    sensor_write_register(0x3014, 0x0000); /* FINE_INTEGRATION_TIME*/            
    sensor_write_register(0x3012, 0x0514); /* Coarse_Integration_Time*/          
    sensor_write_register(0x3042, 0x0000); /* EXTRA_DELAY*/          
    sensor_write_register(0x30BA, 0x002C); /* Digital_Ctrl_Adc_High_Speed*/          

    delay_ms(30);
    sensor_write_register(0x3088, 0x80BA);
    sensor_write_register(0x3086, 0x0253);                                 
    sensor_write_register(0x301A, 0x0004); /* Start Streaming*/          
    
    printf("Aptina AR0330 sensor 3M-1296p 30fps init success!\n");
}

