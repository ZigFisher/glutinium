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
///////////////////////////////////////////////////////////////////////////////////////////Note the following about the OV9750 sequence and AE control:

1. R0x3503[3] = 1. The OV FAE has made is clear that R0x3503[3] should be set to 1. However, when R0x3503[2] is set to 0, that is, when the sensor real gain mode is used, the precision of the sensor Again is not the same as the description of OV (see item 6). In this mode, the AE convergence is not smooth and intermittence occurs during small range adjustment.

2. R0x3503[3] = 0. The OV does not recommend this setting. However, if this setting is used in real gain mode, the sensor precision is the same as the OV description (see item 6) and the AE adjustment process is smooth. However, this setting causes another issue: when Again is less than 1.5x, the picture data is not saturated (the maximum value of the 12-bit data cannot reach 4095), and the overexposure regions, such as the fluorescent tube, have a purple cast. Therefore, the minimum Again needs to be limited to 1.5x.

3. The reference value of DCgain provided by the OV is 2.3x, but the tested DCgain value is 2.53x. In actual scenarios, the value 2.5x is used. The value 2.5x is not obtained after a lot of tests. In the tests, when the randomly selected sensors use this multiple, luminance jump does not occur during AE control.

4. The method of configuring the DCgain varies according to the sensor version.
When sensor is R1C and 0x302C[3:0] is 1, HCG: 0x37C7 = 0x38; LCG: 0x37C7 = 0x39
When sensor is R1A/R1B and 0x302C[3:0] is 0, HCG: 0x37C7 = 0x08; LCG: 0x37C7 = 0x09

5. The sensor Dgain is determined by R0x5003 bit[7:6] (~4x). If bit[7:6] is 0, Dgain is 1x; if bit[7:6] is 1 or 2, Dgain is 2x; if bit[7:6] is 3, Dgain is 4x. Note that if bit[7:6] is 2, Dgain is not 3x.

6. The precision distribution of again in real gain mode is as follows:
again < 2x, 1/128;
2x < again < 4x, 1/64;
4x < again < 8x, 1/32;
8x < again < 15.5x, 1/16

7. The OV FAE has made it clear that OV9750 cannot use the sensor gain mode.

8. In the current OV9750 driver, R0x3503[3] = 1 and the real gain mode is used. Use the look-up table method for the again to reduce the again precision and ensure smooth AE convergence.


Notes about configuring sensor for clients: 

1. The ISP configuration parameters of imx322 is also suitable for imx222. Due to the different sensitivity of respective packages, the register 0x023f of imx323 should be assigned to 0x0a (the default value of imx222 and imx322 is 0x00). Other configuration parameters could be consistent. 

2. Ar0230 supports two output modes as the linear mode and the sensor-build-in WDR mode, while ar0237 support two output modes as linear mode and 2to1_LINE WDR mode. The 2_to1_LINE WDR mode is not supported with Hi3518EV20X and Hi3516CV200. So only the linear mode of ar0237 is available with Hi3518EV20X / Hi3516CV200. The exposure associated registers (exposure time, analog gain and digital gain, etc.) of ar0230 and ar0237 are consistent. So AE configuration is fully suitable. The other configurations can also refer the corresponding modules. There are some certain initialization sequence differences between ar0230 and ar0237 in linear mode. Please take the initialization sequences provided by sensor manufacturers as criterion.
 
3. The initialization sequences of mn34220, mn34227 and mn34229 in linear mode with 1080P30fps is consistent as mn34222. Mn34220, mn34227, mn34229 can refer the AE configuration of mn34222.  What is the difference is the mn34222 is a RGBIR sensor that there is an IR component of four components collected. The RGBIR module should be closed in cmos.c when configuring mn34220, mn34227 and mn34229. The other configurations need to be re-optimized. 

