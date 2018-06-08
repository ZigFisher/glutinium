////////////////////////////////////////////////////////////////////////////////////////////
imx178 provides initialization sequence of two clock frequencies: 25MHz and 37.125MHz.
   1. 25MHz version is supported in default case. It can be used without any modification(corresponding sony_imx178 in sensor catalog). The clock frequency which the chip generates for the imx178 is 25MHz(config in mpp/ko/load3516a). The initialization sequence of imx178 is adjusted based on 27MHz version. The power consumption of this version is low and there is no risk with the conform of sony.
   You can modify in the following manner if you want to use the initialization sequence of 27MHz:
   a. Replace "himm 0x2003002C 0xF0007" with "himm 0x2003002C 0xB0007" in imx178 add-ons in load3516a.
   b. Set the VMAX register value 0x09ab in initialization sequence in ctrl.c. Set the macro VMAX_5M30 value 0x09ab in cmos.c.
   c. Set the VI clock 250MHz. Replace "himm 0x20030104 0x3" with "himm 0x20030104 0x0" in mpp/ko/clkcfg_hi3516a.sh. The VI clock will be control by 0x2003002c. The VI clock will be 250MHz if the value is 0xB0007.

   2. 37.125MHz version(corresponding sony_imx178_37M in sensor catalog). The clock frequency which the chip generates has to be 37.125MHz. You can modify in the following manner:
   a. Replace "himm 0x2003002C 0xF0007" with "himm 0x2003002C 0x90007" in imx178 add-ons of load3516a.
   b. Set the VI clock 250MHz. Replace "himm 0x20030104 0x3" with "himm 0x20030104 0x0" in mpp/ko/clkcfg_hi3516a.sh. The VI clock will be control by 0x2003002c. The VI clock will be 250MHz if the value is 0x90007.
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
You should change HZ config in kernel when sensor(such as mn2240) is running at 120fps or above. Because default HZ is 100, meaning 10ms tick;
////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
IMX123:
	1.In IMX123 line-based wdr mode, when using 27MHz input clock and running at 30fps frame rate, the maximum short exposure time  can only reach 0.489ms(47 line), degrade the line-based wdr image quality when at low light scene.
	2.As the frame rate decrease, the maximum short exposure time increase. when the frame rate to 28.6fps below, the maximum short exposure time can reach 2.036ms(195 line), and the long exposure time can reach 32.583ms(3120 line), meet the basic need of image quality.
	3.As limitation of the maximum of the short exposure time, the exposure ratio is configured to a fixed value of 16:1.

///////////////////////////////////////////////////////////////////////////////////////////	
IMX117:
	1. The Hi3516A demo board of hisilicon use pwm5 & 7 to generate synchronization signal, so you should know that pwm channel must be changed correspond to hardware. 
	2. The default of pwm channel for Auto Iris was pwm5, so choose pwm4 instead of default. And add "pwm_num=4" when you insmod hi_isp.ko. Replace "insmod hi3516a_isp.ko" with "insmod hi3516a_isp.ko pwm_num=4" in "load3516a" script.
///////////////////////////////////////////////////////////////////////////////////////////