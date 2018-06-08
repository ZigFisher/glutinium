#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "hi_ssp.h"


int sony_sensor_write_packet(unsigned int data)
{
	int fd = -1;
	int ret;
	unsigned int value;

	fd = open("/dev/ssp", 0);
	if(fd<0)
	{
		printf("Open /dev/ssp error!\n");
		return -1;
	}

	value = data;

	ret = ioctl(fd, SSP_WRITE_ALT, &value);

	//printf("ssp_write %#x\n", value);

	close(fd);

	return 0;
}

int sensor_write_register(int addr, int data)
{
    return sony_sensor_write_packet((unsigned int)((addr<<16) | (data & 0xFFFF)));
}

static void delay_ms(int ms) { }

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
    printf("-------sensor ICX692 sensor_init!----\n");    

    sensor_write_register(0xc022  ,0x0001);// a. software reset                                                                                  
    sensor_write_register(0xc031  ,0x0001);// b. sck select                                                                                      
    sensor_write_register(0xc0ab  ,0x1100);   //[8] = Enable Oscillator Bias Current                                                             
    sensor_write_register(0xc020  ,0x028B);// c. ENABLE AFE, Buffer,reference                                                                    
    sensor_write_register(0xc021  ,0x0000);// d. Write to CLIDIVIDE as 1xclock                                                                   
    sensor_write_register(0xc009  ,0x3207);   // e. Disable PLL                                                                                  
    sensor_write_register(0xc020  ,0x028f);// f. ENABLE TG core                                                                                  
    sensor_write_register(0xc02f  ,0x024c); //0x020c                                                                                                     
    sensor_write_register(0xc030  ,0x0001);// g. CLI SELECT  
    //sensor_write_register(0xc028    ,0x00f0);// g. CLI SELECT
    sensor_write_register(0xc000  ,0x0003);   // h.AFE operation OPRMODE                                                                         
    sensor_write_register(0xc001  ,0x0003);   // cds gain                                                                                        
    sensor_write_register(0xc002  ,0x0015);   // vga gain                                                                                        
    sensor_write_register(0xc003  ,0x003c);   // black level     0x3d                                                                                
    sensor_write_register(0xc060  ,0x0006);// enable 0xC001,0xC002 update in VD rising edge 
    sensor_write_register(0xc020  ,0x02FF);   // Enable DCLK, DOUT, Hdrv OUT                                                                     
    sensor_write_register(0xc0c0  ,0x0001);   // startup1                                                                                        
    sensor_write_register(0xc0c1  ,0x0040);   // startup2                                                                                        
    sensor_write_register(0xc023  ,0x0000);// i.VD/HD INPUT ENABLE Master mode:00; Slave mode: 03                                                
    sensor_write_register(0xc024  ,0x01F6);   //  Master mode: 1f6 Enable VD/HD GPO1~5 as output; Slave mode:1f4 disabel VD/HD as output           
    sensor_write_register(0xc025  ,0x0001);// Disabling RESET on sync pin and enable Sync pin as input                                           
    sensor_write_register(0xc026  ,0x0001);   // Master mode:1;Slave Mode:0                                                                        
    sensor_write_register(0xc027  ,0x0007);   // configure Sync as a rising edge trigger and VD/HD active polarity                                 
    sensor_write_register(0x7c0b  ,0x00FF);// Master mode: 0xff ENABLE All TG Channels; slave mode 0xfB disable TG VD/HD channel                 
    sensor_write_register(0xc029  ,0x0021);// bit0:XV1 SELECT for XV16 to XV26; bit5: GUI XSG16 mappping                                         
    sensor_write_register(0xc040  ,0x0003);// j. HCLKMODE: 0x0, Mode 0i //*****!!!!!6/30 changing HCLKMODE=3                                     
    sensor_write_register(0xc041  ,0x2000);   // H1 Phase                                                                                        
    sensor_write_register(0xc042  ,0x4120);   // H2 Phase  //***** delay H2 rising and falling edge        0x0122                                      
    sensor_write_register(0xc045  ,0x2000);   //0x2505                                                                                                   
    sensor_write_register(0xc046  ,0x4a3d);   //0x1404                                                                                                   
    sensor_write_register(0xc047  ,0x1323);   // SHP Phase *1620 0x1019                                                                                
    sensor_write_register(0xc048  ,0x0003);   // SHD Phase      0x0000                                                                                 
    sensor_write_register(0xC04a  ,0x2000);   // Dout phase Phase                                                                                
    sensor_write_register(0xC04c  ,0x0e0f);   // enable H4 retime                                                                                
    sensor_write_register(0xC04d  ,0x0040);   // Add one to H2/3/4                                                                               
    sensor_write_register(0xc052  ,0x0002);// select HPAT, generated from TG                                                                     
    sensor_write_register(0xc053  ,0x7777);// H1-clock drive strength    0x3323                                                                        
    sensor_write_register(0xc054  ,0x5555);   // H2-clock drive strength    0x2323                                                                     
    sensor_write_register(0xc055  ,0x5656);// H3-clock drive strength                                                                            
    sensor_write_register(0xc056  ,0x5656);   // H4-clock drive strength                                                                         
    sensor_write_register(0xc057  ,0x0043);  // RG driver strength  // ***** reduce RGDRV from 6 to 2 to remove the oblique line noise    0x0042       
    sensor_write_register(0x7c7b  ,0x0008);   // set XSUBCK to high                                                                              
    sensor_write_register(0x4000  ,0x0000);                                                                                                          
    sensor_write_register(0x4001  ,0x0000);                                                                                                          
    sensor_write_register(0x4002  ,0x000b);                                                                                                          
    sensor_write_register(0x4003  ,0x0116);                                                                                                          
    sensor_write_register(0x4004  ,0x0000);                                                                                                          
    sensor_write_register(0x4005  ,0x012c);                                                                                                          
    sensor_write_register(0x4006  ,0x0000);                                                                                                          
    sensor_write_register(0x4007  ,0x0000);                                                                                                          
    sensor_write_register(0x4008  ,0x0000);                                                                                                          
    sensor_write_register(0x4009  ,0x0000);                                                                                                          
    sensor_write_register(0x400a  ,0x0000);                                                                                                          
    sensor_write_register(0x400b  ,0x0000);                                                                                                          
    sensor_write_register(0x400c  ,0x0000);                                                                                                          
    sensor_write_register(0x403e  ,0x0108);                                                                                                          
    sensor_write_register(0x4040  ,0x0002);                                                                                                          
    sensor_write_register(0x4041  ,0x0050);                                                                                                          
    sensor_write_register(0x4042  ,0x0051);                                                                                                          
    sensor_write_register(0x4043  ,0x0202);                                                                                                          
    sensor_write_register(0x4044  ,0x1202);                                                                                                          
    sensor_write_register(0x4045  ,0x0072);                                                                                                          
    sensor_write_register(0x4046  ,0x0051);                                                                                                          
    sensor_write_register(0x4047  ,0x0202);                                                                                                          
    sensor_write_register(0x4048  ,0x0000);                                                                                                          
    sensor_write_register(0x4049  ,0x00c2);                                                                                                          
    sensor_write_register(0x404a  ,0x0648);                                                                                                          
    sensor_write_register(0x404b  ,0x005a);                                                                                                          
    sensor_write_register(0x404c  ,0x0051);                                                                                                          
    sensor_write_register(0x404d  ,0x0202);                                                                                                          
    sensor_write_register(0x404e  ,0x02eb);                                                                                                          
    sensor_write_register(0x404f  ,0x0026);                                                                                                          
    sensor_write_register(0x4050  ,0xff08);                                                                                                          
    sensor_write_register(0x4051  ,0xffff);                                                                                                          
    sensor_write_register(0x4052  ,0x0407);                                                                                                          
    sensor_write_register(0x4053  ,0xffff);                                                                                                          
    sensor_write_register(0x4054  ,0x1202);                                                                                                          
    sensor_write_register(0x4055  ,0x0073);                                                                                                          
    sensor_write_register(0x4056  ,0x0078);                                                                                                          
    sensor_write_register(0x4057  ,0x0909);                                                                                                          
    sensor_write_register(0x4058  ,0x0000);                                                                                                          
    sensor_write_register(0x4059  ,0x00f6);                                                                                                          
    sensor_write_register(0x405a  ,0x0648);                                                                                                          
    sensor_write_register(0x405b  ,0x007d);                                                                                                          
    sensor_write_register(0x405c  ,0x007f);                                                                                                          
    sensor_write_register(0x405d  ,0x0404);                                                                                                          
    sensor_write_register(0x405e  ,0x02eb);                                                                                                          
    sensor_write_register(0x405f  ,0x0008);                                                                                                          
    sensor_write_register(0x4060  ,0x0306);                                                                                                          
    sensor_write_register(0x4061  ,0xffff);                                                                                                          
    sensor_write_register(0x4062  ,0x1202);                                                                                                          
    sensor_write_register(0x4063  ,0x005b);                                                                                                          
    sensor_write_register(0x4064  ,0x0066);                                                                                                          
    sensor_write_register(0x4065  ,0x1716);                                                                                                          
    sensor_write_register(0x4066  ,0x0000);                                                                                                          
    sensor_write_register(0x4067  ,0x00c2);                                                                                                          
    sensor_write_register(0x4068  ,0x0648);                                                                                                          
    sensor_write_register(0x4069  ,0x0052);                                                                                                          
    sensor_write_register(0x406a  ,0x0056);                                                                                                          
    sensor_write_register(0x406b  ,0x0807);                                                                                                          
    sensor_write_register(0x406c  ,0x02eb);                                                                                                          
    sensor_write_register(0x406d  ,0x0026);                                                                                                          
    sensor_write_register(0x406e  ,0x0306);                                                                                                          
    sensor_write_register(0x406f  ,0xffff);                                                                                                          
    sensor_write_register(0x4070  ,0x0207);                                                                                                          
    sensor_write_register(0x4071  ,0x2081);                                                                                                          
    sensor_write_register(0x4072  ,0x0640);                                                                                                          
    sensor_write_register(0x4073  ,0x0000);                                                                                                          
    sensor_write_register(0x4074  ,0x0807);                                                                                                          
    sensor_write_register(0x4075  ,0x2082);                                                                                                          
    sensor_write_register(0x4076  ,0x0001);                                                                                                          
    sensor_write_register(0x4077  ,0x0000);                                                                                                          
    sensor_write_register(0x4078  ,0x0207);                                                                                                          
    sensor_write_register(0x4079  ,0x2083);                                                                                                          
    sensor_write_register(0x407a  ,0x0640);                                                                                                          
    sensor_write_register(0x407b  ,0x0000);                                                                                                          
    sensor_write_register(0x407c  ,0x8807);                                                                                                          
    sensor_write_register(0x407d  ,0x2082);                                                                                                          
    sensor_write_register(0x407e  ,0x02ea);                                                                                                          
    sensor_write_register(0x407f  ,0x0000);                                                                                                          
    sensor_write_register(0x4080  ,0x4207);                                                                                                          
    sensor_write_register(0x4081  ,0x2082);                                                                                                          
    sensor_write_register(0x4082  ,0x0640);                                                                                                          
    sensor_write_register(0x4083  ,0x0000);                                                                                                          
    sensor_write_register(0x4084  ,0x080a);                                                                                                          
    sensor_write_register(0x4085  ,0x0406);                                                                                                          
    sensor_write_register(0x4086  ,0xff02);                                                                                                          
    sensor_write_register(0x4087  ,0xffff);                                                                                                          
    sensor_write_register(0x40a0  ,0x000a);                                                                                                          
    sensor_write_register(0x40a1  ,0x0028);                                                                                                          
    sensor_write_register(0x40a2  ,0x0000);                                                                                                          
    sensor_write_register(0x40a3  ,0x0000);                                                                                                          
    sensor_write_register(0x40a4  ,0x0002);                                                                                                          
    sensor_write_register(0x40a5  ,0x0016);                                                                                                          
    sensor_write_register(0x40a6  ,0x0022);                                                                                                          
    sensor_write_register(0x40a7  ,0x0022);                                                                                                          
    sensor_write_register(0x40a8  ,0x0016);                                                                                                          
    sensor_write_register(0x40a9  ,0x0022);                                                                                                          
    sensor_write_register(0x40aa  ,0x0016);                                                                                                          
    sensor_write_register(0x40ab  ,0x0000);                                                                                                          
    sensor_write_register(0x40ac  ,0x0001);                                                                                                          
    sensor_write_register(0x40ad  ,0x0004);                                                                                                          
    sensor_write_register(0x40ae  ,0x0002);                                                                                                          
    sensor_write_register(0x40af  ,0x0008);                                                                                                          
    sensor_write_register(0x40b0  ,0x0004);                                                                                                          
    sensor_write_register(0x40b1  ,0x0001);                                                                                                          
    sensor_write_register(0x40b2  ,0x0008);                                                                                                          
    sensor_write_register(0x40b3  ,0x0002);                                                                                                          
    sensor_write_register(0x40b4  ,0x000a);                                                                                                          
    sensor_write_register(0x40b5  ,0x00b6);                                                                                                          
    sensor_write_register(0x40b6  ,0x0002);                                                                                                          
    sensor_write_register(0x40b7  ,0x0016);                                                                                                          
    sensor_write_register(0x40b8  ,0x0044);                                                                                                          
    sensor_write_register(0x40b9  ,0x0016);                                                                                                          
    sensor_write_register(0x40ba  ,0x04ae);                                                                                                          
    sensor_write_register(0x40bb  ,0x00cc);                                                                                                          
    sensor_write_register(0x40bc  ,0x0016);                                                                                                          
    sensor_write_register(0x40bd  ,0x0022);                                                                                                          
    sensor_write_register(0x40be  ,0x0016);                                                                                                          
    sensor_write_register(0x40bf  ,0x0022);                                                                                                          
    sensor_write_register(0x40c0  ,0x0022);                                                                                                          
    sensor_write_register(0x40c1  ,0x0016);                                                                                                          
    sensor_write_register(0x40c2  ,0x0022);                                                                                                          
    sensor_write_register(0x40c3  ,0x0016);                                                                                                          
    sensor_write_register(0x40c4  ,0x0016);                                                                                                          
    sensor_write_register(0x40c5  ,0x0022);                                                                                                          
    sensor_write_register(0x40c6  ,0x0016);                                                                                                          
    sensor_write_register(0x40c7  ,0x0022);                                                                                                          
    sensor_write_register(0x40c8  ,0x0022);                                                                                                          
    sensor_write_register(0x40c9  ,0x0016);                                                                                                          
    sensor_write_register(0x40ca  ,0x0022);                                                                                                          
    sensor_write_register(0x40cb  ,0x0016);                                                                                                          
    sensor_write_register(0x40cc  ,0x0001);                                                                                                          
    sensor_write_register(0x40cd  ,0x0004);                                                                                                          
    sensor_write_register(0x40ce  ,0x000a);                                                                                                          
    sensor_write_register(0x40cf  ,0x0004);                                                                                                          
    sensor_write_register(0x40d0  ,0x0001);                                                                                                          
    sensor_write_register(0x40d1  ,0x0008);                                                                                                          
    sensor_write_register(0x40d2  ,0x0002);                                                                                                          
    sensor_write_register(0x40d3  ,0x0001);                                                                                                          
    sensor_write_register(0x40d4  ,0x0004);                                                                                                          
    sensor_write_register(0x40d5  ,0x0002);                                                                                                          
    sensor_write_register(0x40d6  ,0x0008);                                                                                                          
    sensor_write_register(0x40d7  ,0x0004);                                                                                                          
    sensor_write_register(0x40d8  ,0x0001);                                                                                                          
    sensor_write_register(0x40d9  ,0x0008);                                                                                                          
    sensor_write_register(0x40da  ,0x0002);                                                                                                          
    sensor_write_register(0x40db  ,0x0001);                                                                                                          
    sensor_write_register(0x40dc  ,0x0004);                                                                                                          
    sensor_write_register(0x40dd  ,0x0002);                                                                                                          
    sensor_write_register(0x40de  ,0x0008);                                                                                                          
    sensor_write_register(0x40df  ,0x0004);                                                                                                          
    sensor_write_register(0x40e0  ,0x0001);                                                                                                          
    sensor_write_register(0x40e1  ,0x0008);                                                                                                          
    sensor_write_register(0x40e2  ,0x0002);                                                                                                          
    sensor_write_register(0x40e3  ,0x0000);                                                                                                          
    sensor_write_register(0x40e4  ,0x000a);                                                                                                          
    sensor_write_register(0x40e5  ,0x07b8);                                                                                                          
    sensor_write_register(0x40e6  ,0x0002);                                                                                                          
    sensor_write_register(0x40e7  ,0x04b5);                                                                                                          
    sensor_write_register(0x40e8  ,0x0010);                                                                                                          
    sensor_write_register(0x40e9  ,0x004c);                                                                                                          
    sensor_write_register(0x40ea  ,0x0010);                                                                                                          
    sensor_write_register(0x40eb  ,0x0027);                                                                                                          
    sensor_write_register(0x40ec  ,0x00f8);                                                                                                          
    sensor_write_register(0x40ed  ,0x0548);                                                                                                          
    sensor_write_register(0x40ee  ,0x00f8);                                                                                                          
    sensor_write_register(0x40ef  ,0x0000);                                                                                                          
    sensor_write_register(0x40f0  ,0x800c);                                                                                                          
    sensor_write_register(0x40f1  ,0x0002);                                                                                                          
    sensor_write_register(0x40f2  ,0x0004);                                                                                                          
    sensor_write_register(0x40f3  ,0x0002);                                                                                                          
    sensor_write_register(0x40f4  ,0x0004);                                                                                                          
    sensor_write_register(0x40f5  ,0x800c);                                                                                                          
    sensor_write_register(0x40f6  ,0x800c);                                                                                                          
    sensor_write_register(0x40f7  ,0x800c);                                                                                                          
    sensor_write_register(0x40f8  ,0x800c);                                                                                                          
    sensor_write_register(0x40f9  ,0x0000);                                                                                                          
    sensor_write_register(0x40fa  ,0x0002);                                                                                                          
    sensor_write_register(0x40fb  ,0x0014);                                                                                                          
    sensor_write_register(0x40fc  ,0x000a);                                                                                                          
    sensor_write_register(0x40fd  ,0x00f0);                                                                                                          
    sensor_write_register(0x40fe  ,0x8001);                                                                                                          
    sensor_write_register(0x40ff  ,0x8001);                                                                                                          
    sensor_write_register(0x4100  ,0x800c);                                                                                                          
    sensor_write_register(0x4101  ,0x800c);                                                                                                          
    sensor_write_register(0x4102  ,0xc000);                                                                                                          
    sensor_write_register(0x4103  ,0x8064);                                                                                                          
    sensor_write_register(0x4104  ,0x8000);                                                                                                          
    sensor_write_register(0x4105  ,0x8064);                                                                                                          
    sensor_write_register(0x4106  ,0xc000);                                                                                                          
    sensor_write_register(0x4107  ,0x8064);                                                                                                          
    sensor_write_register(0x4108  ,0x0000);                                                                                                          
    sensor_write_register(0x4109  ,0x0003);                                                                                                          
    sensor_write_register(0x410a  ,0x0000);                                                                                                          
    sensor_write_register(0x410b  ,0x2030);                                                                                                          
    sensor_write_register(0x410c  ,0x010d);                                                                                                          
    sensor_write_register(0x410d  ,0x0109);                                                                                                          
    sensor_write_register(0x410e  ,0x0042);                                                                                                          
    sensor_write_register(0x410f  ,0x0037);                                                                                                          
    sensor_write_register(0x4110  ,0x0029);                                                                                                          
    sensor_write_register(0x4111  ,0x0030);                                                                                                          
    sensor_write_register(0x4112  ,0xffff);                                                                                                          
    sensor_write_register(0x4113  ,0xffff);                                                                                                          
    sensor_write_register(0x4114  ,0xffff);                                                                                                          
    sensor_write_register(0x4115  ,0xff6f);                                                                                                        
    sensor_write_register(0x4116  ,0x1a06);                                                                                                        
    sensor_write_register(0x4117  ,0x0126);                                                                                                        
    sensor_write_register(0x4118  ,0x1b86);                                                                                                        
    sensor_write_register(0x4119  ,0x0122);                                                                                                        
    sensor_write_register(0x411a  ,0x001b);                                                                                                        
    sensor_write_register(0x411b  ,0x0132);                                                                                                        
    sensor_write_register(0x411c  ,0x0586);                                                                                                        
    sensor_write_register(0x411d  ,0x00ff);                                                                                                        
    sensor_write_register(0x411e  ,0x8086);                                                                                                        
    sensor_write_register(0x411f  ,0x0001);                                                                                                        
    sensor_write_register(0x4120  ,0x0015);                                                                                                        
    sensor_write_register(0x4121  ,0x0025);                                                                                                        
    sensor_write_register(0x4122  ,0x8186);                                                                                                        
    sensor_write_register(0x4123  ,0x008f);                                                                                                        
    sensor_write_register(0x4124  ,0x0035);                                                                                                        
    sensor_write_register(0x4125  ,0x000d);                                                                                                          
    sensor_write_register(0x4126  ,0x039c);                                                                                                        
    sensor_write_register(0x4127  ,0x0001);                                                                                                        
    sensor_write_register(0x4128  ,0x001c);                                                                                                        
    sensor_write_register(0x4129  ,0x0000);                                                                                                        
    sensor_write_register(0x412a  ,0x0003);                                                                                                        
    sensor_write_register(0x412b  ,0x000d);                                                                                                          
    sensor_write_register(0x412c  ,0x001b);                                                                                                        
    sensor_write_register(0x412d  ,0x0132);                                                                                                        
    sensor_write_register(0x412e  ,0x8086);                                                                                                        
    sensor_write_register(0x412f  ,0x0002);                                                                                                        
    sensor_write_register(0x4130  ,0x0035);                                                                                                        
    sensor_write_register(0x4131  ,0x000d);                                                                                                          
    sensor_write_register(0x4132  ,0x039c);                                                                                                        
    sensor_write_register(0x4133  ,0x0000);                                                                                                        
    sensor_write_register(0x4134  ,0x0086);                                                                                                        
    sensor_write_register(0x4135  ,0x010c);                                                                                                        
    sensor_write_register(0x4136  ,0x4400);                                                                                                        
    sensor_write_register(0x4137  ,0x000d);                                                                                                          
    sensor_write_register(0x4138  ,0x411c);                                                                                                        
    sensor_write_register(0x4139  ,0x0000);                                                                                                        
    sensor_write_register(0x413a  ,0x611c);                                                                                                        
    sensor_write_register(0x413b  ,0x0001);                                                                                                        
    sensor_write_register(0x413c  ,0x638c);                                                                                                        
    sensor_write_register(0x413d  ,0x3d25);                                                                                                        
    sensor_write_register(0x413e  ,0x611c);                                                                                                        
    sensor_write_register(0x413f  ,0x0002);                                                                                                        
    sensor_write_register(0x4140  ,0x638c);                                                                                                        
    sensor_write_register(0x4141  ,0x3d40);                                                                                                        
    sensor_write_register(0x4142  ,0x611c);                                                                                                        
    sensor_write_register(0x4143  ,0x0003);                                                                                                        
    sensor_write_register(0x4144  ,0x638c);                                                                                                        
    sensor_write_register(0x4145  ,0x3d60);                                                                                                        
    sensor_write_register(0x4146  ,0x611c);                                                                                                        
    sensor_write_register(0x4147  ,0x0004);                                                                                                        
    sensor_write_register(0x4148  ,0x638c);                                                                                                        
    sensor_write_register(0x4149  ,0x3d80);                                                                                                        
    sensor_write_register(0x414a  ,0x611c);                                                                                                        
    sensor_write_register(0x414b  ,0x0005);                                                                                                        
    sensor_write_register(0x414c  ,0x638c);                                                                                                        
    sensor_write_register(0x414d  ,0x3c35);                                                                                                        
    sensor_write_register(0x414e  ,0x611c);                                                                                                        
    sensor_write_register(0x414f  ,0x0006);                                                                                                        
    sensor_write_register(0x4150  ,0x638c);                                                                                                        
    sensor_write_register(0x4151  ,0x3c36);                                                                                                        
    sensor_write_register(0x4152  ,0x611c);                                                                                                        
    sensor_write_register(0x4153  ,0x0008);                                                                                                        
    sensor_write_register(0x4154  ,0x638c);                                                                                                        
    sensor_write_register(0x4155  ,0x3c20);                                                                                                        
    sensor_write_register(0x4156  ,0x611c);                                                                                                        
    sensor_write_register(0x4157  ,0x0000);                                                                                                        
    sensor_write_register(0x4158  ,0xa19c);                                                                                                        
    sensor_write_register(0x4159  ,0x0000);                                                                                                        
    sensor_write_register(0x415a  ,0xa38c);                                                                                                        
    sensor_write_register(0x415b  ,0x3d42);                                                                                                        
    sensor_write_register(0x415c  ,0xa19c);                                                                                                        
    sensor_write_register(0x415d  ,0x0001);                                                                                                        
    sensor_write_register(0x415e  ,0xa38c);                                                                                                        
    sensor_write_register(0x415f  ,0x3d62);                                                                                                        
    sensor_write_register(0x4160  ,0xa19c);                                                                                                        
    sensor_write_register(0x4161  ,0x0002);                                                                                                        
    sensor_write_register(0x4162  ,0xa38c);                                                                                                        
    sensor_write_register(0x4163  ,0x3d83);                                                                                                        
    sensor_write_register(0x4164  ,0x611c);                                                                                                        
    sensor_write_register(0x4165  ,0x0007);                                                                                                        
    sensor_write_register(0x4166  ,0xa19c);                                                                                                        
    sensor_write_register(0x4167  ,0x0000);                                                                                                        
    sensor_write_register(0x4168  ,0xa38c);                                                                                                        
    sensor_write_register(0x4169  ,0x3c33);                                                                                                        
    sensor_write_register(0x416a  ,0x0055);                                                                                                        
    sensor_write_register(0x7DA0  ,0x0009);                                                                                                        
    sensor_write_register(0x7D41  ,0x0044);                                                                                                        
    sensor_write_register(0x7D05  ,0x0001);       //slave:0; master:1                                                                                
    sensor_write_register(0x7D02  ,0x00FF);                                                                                                        
    sensor_write_register(0x7C06  ,0x0001);                                                                                                        
    sensor_write_register(0x7C07  ,0x0000);                                                                                                        
    sensor_write_register(0x7D43  ,0x2000);                                                                                                        
    sensor_write_register(0x7D63  ,0x0005);                                                                                                        
    sensor_write_register(0x7D82  ,0x7FFF);                                                                                                        
    sensor_write_register(0x7D83  ,0x203C);                                                                                                        
    sensor_write_register(0x7d01  ,0x0004);        //Force initial value updated                                                                     
    sensor_write_register(0xc028  ,0x00f1);       //VDR ENABLE                                                                                       
    sensor_write_register(0xc020  ,0x02FF);   // Enable DCLK, DOUT, Hdrv                                                                         
    sensor_write_register(0x7c25  ,0x7FFF);//                                                                                                    
    sensor_write_register(0xc052  ,0x0000);                                                                                                      
    sensor_write_register(0xc04c  ,0x1111);                                                                                                      
    sensor_write_register(0xC080  ,0xFFFF);// Set outen_reg0 register to enabled.                                                                
    sensor_write_register(0xC081  ,0xFFFF);// Set outen_reg1 register to enabled.                                                                
    sensor_write_register(0xC082  ,0xFFFF);// Set outen_reg2 register to enabled.                                                                
    sensor_write_register(0xC083  ,0xFFFF);// Set outen_reg3 register to enabled.                                                                
    sensor_write_register(0xC084  ,0x1C03);// Set outstandby_reg0 register to polarities of TG outputs(XVSG1-XVSG3, VD, HD to 1, others to 0)    
    sensor_write_register(0xC085  ,0x1FFF);// Set outstandby_reg1 register to polarities of TG outputs(XVSG4-XVSG16 to 1, others to 0)           
    sensor_write_register(0xC086  ,0x0000);// Set outstandby_reg2 register to polarities of TG outputs(XV1-XV16:  all to 0)                      
    sensor_write_register(0xC087  ,0x0000);// Set outstandby_reg3 register to polarities of TG outputs(XV1-XV16, RGBLK, SUBCK: all to 0)         
    sensor_write_register(0x7c0f  ,0x0011);// enable memory compression mode                                                                     
    sensor_write_register(0x7c34  ,0x0002);// b.Fixed start address at 0x4002 (SPI)                                                              
    sensor_write_register(0x7c20  ,0xFFFF);// IMASK, disalbe all interrupt                                                                       
    sensor_write_register(0x7d00  ,0x0002);//  c.TG running mode enable                                                                          
    sensor_write_register(0x4000  ,0x0000);// mode Id:0                                                                                          
    sensor_write_register(0x7c22  ,0x0000);// d.ILAT (CLEAR INTERRUPTS)                                                                          
    sensor_write_register(0x7c21  ,0x0000);// clear the PMASK                                                                                    
    sensor_write_register(0x7c20  ,0xffef);// e.IMASK to allow SYNC Interrupt                                                                    
    sensor_write_register(0x7c22  ,0x0010);// f. ILAT to trigger SYNC ISR                                                                        
    sensor_write_register(0x7C73  ,0x0001);   //SUBCK_AUTO                                                                                       
    sensor_write_register(0x7C7B  ,0x0008);   //SUBCK_CTRL                                                                                       
    sensor_write_register(0x7C90  ,0x00bd);//TOG1, 25                                                                                            
    sensor_write_register(0x7C92  ,0x00ea);//TOG2, 52                                                                                            
    sensor_write_register(0x7C94  ,0xffff);//HP_TOG1, disable                                                                                    
    sensor_write_register(0x7C96  ,0xffff);//HP_TOG2, disable                                                                                    
    sensor_write_register(0x7C72  ,0x0001);//SGLINE                                                                                              
    sensor_write_register(0x7C13  ,0x0008);//subck startline from SGLINE                                                                         
    sensor_write_register(0x7C14  ,0x0020);//subck endline from SGLINE                                                                           
    sensor_write_register(0xc074  ,0x1110);// 9020 vdriver mux                                                                                   
    sensor_write_register(0xc075  ,0x1312);                                                                                                      
    sensor_write_register(0xc078  ,0x0908);                                                                                                      
    sensor_write_register(0xc079  ,0x0b0a);    

    sensor_write_register(0xc0ae  ,0x8088); 

    printf("-------sensor ICX692 initial ok!----\n");
}

