#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef HI_GPIO_I2C
#include "gpioi2c_ex.h"
#include "gpio_i2c.h"
#else
#include "hi_i2c.h"
#endif

const unsigned char sensor_i2c_addr	=	0x20;		/* I2C Address of AR0130 */
const unsigned int  sensor_addr_byte	=	2;
const unsigned int  sensor_data_byte	=	2;

int sensor_read_register(int addr)
{
	// TODO: 
	
	return 0;
}

int sensor_write_register(int addr, int data)
{
#ifdef HI_GPIO_I2C
    int fd = -1;
    int ret;
    I2C_DATA_S i2c_data;
    
    fd = open("/dev/gpioi2c_ex", 0);
    if(fd<0)
    {
        printf("Open gpioi2c_ex error!\n");
        return -1;
    }

    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sensor_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sensor_data_byte;

    ret = ioctl(fd, GPIO_I2C_WRITE, &i2c_data);

    if (ret)
    {
        printf("GPIO-I2C write faild!\n");
        close(fd);
        return -1;
    }

    close(fd);
#else
    int fd = -1;
    int ret;
    I2C_DATA_S i2c_data;
	
    fd = open("/dev/hi_i2c", 0);
    if(fd<0)
    {
        printf("Open hi_i2c error!\n");
        return -1;
    }
    
    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sensor_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sensor_data_byte;

    ret = ioctl(fd, CMD_I2C_WRITE, &i2c_data);

    if (ret)
    {
        printf("hi_i2c write faild!\n");
        close(fd);
        return -1;
    }

    close(fd);
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

void sensor_init()
{
//[720p30]
#if 0	
    sensor_write_register(0x301A, 0x0001);  // RESET_REGISTER
    sensor_write_register(0x301A, 0x10D8);  // RESET_REGISTER
    
    delay_ms(200);  //DELAY= 200
    
    sensor_write_register(0x3088, 0x8000);  // SEQ_CTRL_PORT
    sensor_write_register(0x3086, 0x0225);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x5050);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2D26);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0828);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0D17);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0926);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0028);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0526);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0xA728);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0725);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x8080);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2917);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0525);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0040);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2702);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1616);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2706);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1736);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x26A6);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1703);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x26A4);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x171F);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2805);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2620);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2804);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2520);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2027);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0017);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1E25);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0020);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2117);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1028);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x051B);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1703);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2706);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1703);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1741);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2660);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x17AE);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2500);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x9027);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0026);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1828);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x002E);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2A28);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x081E);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0831);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1440);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x4014);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2020);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1410);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1034);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1400);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1014);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0020);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1400);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x4013);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1802);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1470);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x7004);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1470);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x7003);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1470);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x7017);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2002);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1400);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2002);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1400);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x5004);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1400);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2004);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x1400);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x5022);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0314);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0020);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0314);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x0050);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2C2C);  // SEQ_DATA_PORT
    sensor_write_register(0x3086, 0x2C2C);  // SEQ_DATA_PORT
    sensor_write_register(0x309E, 0x0000);  // ERS_PROG_START_ADDR
    
    delay_ms(200);  //DELAY= 200
    
    sensor_write_register(0x30E4, 0x6372);  // ADC_BITS_6_7
    sensor_write_register(0x30E2, 0x7253);  // ADC_BITS_4_5
    sensor_write_register(0x30E0, 0x5470);  // ADC_BITS_2_3
    sensor_write_register(0x30E6, 0xC4CC);  // ADC_CONFIG1
    sensor_write_register(0x30E8, 0x8050);  // ADC_CONFIG2
    sensor_write_register(0x3082, 0x0029);  // OPERATION_MODE_CTRL
    sensor_write_register(0x30B0, 0x1300);  // DIGITAL_TEST
    sensor_write_register(0x30D4, 0xE007);  // COLUMN_CORRECTION
    sensor_write_register(0x301A, 0x10DC);  // RESET_REGISTER
    sensor_write_register(0x301A, 0x10D8);  // RESET_REGISTER
    sensor_write_register(0x3044, 0x0400);  // DARK_CONTROL
    sensor_write_register(0x3EDA, 0x0F03);  // DAC_LD_14_15
    sensor_write_register(0x3ED8, 0x01EF);  // DAC_LD_12_13
    sensor_write_register(0x3012, 0x02A0);  // COARSE_INTEGRATION_TIME
    sensor_write_register(0x3032, 0x0000);  // DIGITAL_BINNING
    sensor_write_register(0x3002, 0x003e);  // Y_ADDR_START
    sensor_write_register(0x3004, 0x0004);  // X_ADDR_START
    sensor_write_register(0x3006, 0x030d);  // Y_ADDR_END
    sensor_write_register(0x3008, 0x0503);  // X_ADDR_END
    sensor_write_register(0x300A, 0x02EE);  // FRAME_LENGTH_LINES
    sensor_write_register(0x300C, 0x0CE4);  // LINE_LENGTH_PCK
    sensor_write_register(0x301A, 0x10D8);  // RESET_REGISTER
    sensor_write_register(0x31D0, 0x0001);  // HDR_COMP

    //Load = PLL Enabled 27Mhz to 74.25Mhz
    sensor_write_register(0x302C, 0x0002);  // VT_SYS_CLK_DIV
    sensor_write_register(0x302A, 0x0004);  // VT_PIX_CLK_DIV
    sensor_write_register(0x302E, 0x0002);  // PRE_PLL_CLK_DIV
    sensor_write_register(0x3030, 0x002C);  // PLL_MULTIPLIER
    sensor_write_register(0x30B0, 0x0000);  // DIGITAL_TEST 
    delay_ms(100);  //DELAY= 100
#endif
    sensor_write_register( 0x301A, 0x0001 );    // RESET_REGISTER
    delay_ms(200); //ms
    sensor_write_register( 0x301A, 0x10D8 );    // RESET_REGISTER
    delay_ms(200); //ms
    //Linear Mode Setup
    //AR0130 Rev1 Linear sequencer load  8-2-2011
    sensor_write_register( 0x3088, 0x8000 );// SEQ_CTRL_PORT
    sensor_write_register( 0x3086, 0x0225 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x5050 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2D26 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0828 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0D17 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0926 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0028 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0526 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0xA728 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0725 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x8080 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2917 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0525 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0040 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2702 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1616 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2706 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1736 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x26A6 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1703 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x26A4 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x171F );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2805 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2620 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2804 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2520 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2027 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0017 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1E25 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0020 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2117 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1028 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x051B );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1703 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2706 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1703 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1747 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2660 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x17AE );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2500 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x9027 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0026 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1828 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x002E );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2A28 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x081E );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0831 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1440 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x4014 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2020 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1410 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1034 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1400 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1014 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0020 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1400 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x4013 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1802 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1470 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x7004 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1470 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x7003 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1470 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x7017 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2002 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1400 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2002 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1400 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x5004 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1400 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2004 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x1400 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x5022 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0314 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0020 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0314 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x0050 );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2C2C );// SEQ_DATA_PORT
    sensor_write_register( 0x3086, 0x2C2C );// SEQ_DATA_PORT
    sensor_write_register( 0x309E, 0x0000 );// DCDS_PROG_START_ADDR
    sensor_write_register( 0x30E4, 0x6372 );// ADC_BITS_6_7
    sensor_write_register( 0x30E2, 0x7253 );// ADC_BITS_4_5
    sensor_write_register( 0x30E0, 0x5470 );// ADC_BITS_2_3
    sensor_write_register( 0x30E6, 0xC4CC );// ADC_CONFIG1
    sensor_write_register( 0x30E8, 0x8050 );// ADC_CONFIG2
    delay_ms(200); //ms
    sensor_write_register( 0x3082, 0x0029 );    // OPERATION_MODE_CTRL
    //AR0130 Rev1 Optimized settings
    sensor_write_register( 0x301E, 0x00C8); // DATA_PEDESTAL
    sensor_write_register( 0x3EDA, 0x0F03); // DAC_LD_14_15
    sensor_write_register( 0x3EDE, 0xC005); // DAC_LD_18_19
    sensor_write_register( 0x3ED8, 0x09EF); // DAC_LD_12_13
    sensor_write_register( 0x3EE2, 0xA46B); // DAC_LD_22_23
    sensor_write_register( 0x3EE0, 0x047D); // DAC_LD_20_21
    sensor_write_register( 0x3EDC, 0x0070); // DAC_LD_16_17
    sensor_write_register( 0x3044, 0x0404); // DARK_CONTROL
    sensor_write_register( 0x3EE6, 0x8303); // DAC_LD_26_27
    sensor_write_register( 0x3EE4, 0xD208); // DAC_LD_24_25
    sensor_write_register( 0x3ED6, 0x00BD); // DAC_LD_10_11
    sensor_write_register( 0x30B0, 0x1300); // DIGITAL_TEST
    sensor_write_register( 0x30D4, 0xE007); // COLUMN_CORRECTION
    sensor_write_register( 0x301A, 0x10DC); // RESET_REGISTER
    delay_ms(500 );//ms                 
    sensor_write_register( 0x301A, 0x10D8); // RESET_REGISTER
    delay_ms(500); //ms                   
    sensor_write_register( 0x3044, 0x0400); // DARK_CONTROL
                                    
    sensor_write_register( 0x3012, 0x02A0); // COARSE_INTEGRATION_TIME

    
    //720p 30fps Setup                   
    sensor_write_register( 0x3032, 0x0000); // DIGITAL_BINNING
    sensor_write_register( 0x3002, 0x0002); // Y_ADDR_START
    sensor_write_register( 0x3004, 0x0000); // X_ADDR_START
    sensor_write_register( 0x3006, 0x02D1);//Row End (A) = 721
    sensor_write_register( 0x3008, 0x04FF);//Column End (A) = 1279
    sensor_write_register( 0x300A, 0x02EA);//Frame Lines (A) = 746
    sensor_write_register( 0x300C, 0x08ba);
    sensor_write_register( 0x3012, 0x0133);//Coarse_IT_Time (A) = 307
    sensor_write_register( 0x306e, 0x9211);//Coarse_IT_Time (A) = 307


    //Enable Parallel Mode
    sensor_write_register( 0x301A, 0x10D8); // RESET_REGISTER
    sensor_write_register( 0x31D0, 0x0001); // HDR_COMP
    
    //PLL Enabled 27Mhz to 50Mhz
    sensor_write_register( 0x302A, 0x0009 );//VT_PIX_CLK_DIV = 9
    sensor_write_register( 0x302C, 0x0001 );//VT_SYS_CLK_DIV = 1
    sensor_write_register( 0x302E, 0x0003 );//PRE_PLL_CLK_DIV = 3
    sensor_write_register( 0x3030, 0x0032 );//PLL_MULTIPLIER = 50
    sensor_write_register( 0x30B0, 0x1300 );    // DIGITAL_TEST
    delay_ms(100); //ms
    sensor_write_register( 0x301A, 0x10DC );    // RESET_REGISTER
    sensor_write_register( 0x301A, 0x10DC );    // RESET_REGISTER
                                          
    //exposure                            
    sensor_write_register( 0x3012, 0x0671 );    // COARSE_INTEGRATION_TIME
    sensor_write_register( 0x30B0, 0x1330 );    // DIGITAL_TEST
    sensor_write_register( 0x3056, 0x003B );    // GREEN1_GAIN
    sensor_write_register( 0x305C, 0x003B );    // GREEN2_GAIN
    sensor_write_register( 0x305A, 0x003B );    // RED_GAIN
    sensor_write_register( 0x3058, 0x003B );    // BLUE_GAIN
    //High Conversion gain                
    sensor_write_register( 0x3100, 0x0004 );    // AE_CTRL_REG


    //LOAD= Disable Embedded Data and Stats
    sensor_write_register(0x3064, 0x1802);  // SMIA_TEST, EMBEDDED_STATS_EN, 0x0000
    sensor_write_register(0x3064, 0x1802);  // SMIA_TEST, EMBEDDED_DATA, 0x0000 

    sensor_write_register(0x30BA, 0x0008);       //20120502

    sensor_write_register(0x3EE4, 0xD308);  //the default value former is 0xd208

    sensor_write_register(0x301A, 0x10DC);  // RESET_REGISTER

    delay_ms(200);  //DELAY= 200


    printf("Aptina AR0130 sensor 720P30fps init success!\n");
    
}


