Note the following about the OV9750 sequence and AE control:

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

