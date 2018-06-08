#!/bin/sh

# This is a sample, you should rewrite it according to your chip #
# clock will be closed at uboot, so you needn't call it!

clk_close()
{
	# Below clock operation is all from Hi3516A, you should modify by datasheet!
	echo "clock close operation done!"
}

# open module clock while you need it!
clk_cfg()
{
	himm 0x2003002c 0xc4003;        # VICAP, ISP unreset & clock enable, Sensor clock enable, clk reverse
	
	#himm 0x20030040 0x2000;        # AVC unreset, code also config
	#himm 0x20030048 0x2;           # VPSS unreset, code also config
	#himm 0x20030058 0x2;           # TDE  unreset
	#himm 0x2003005c 0x2;           # VGS
	#himm 0x20030060 0x2;           # JPGE unreset
	#himm 0x2003006c 0xa;           # IVE/HASH  unreset
	#himm 0x2003007c 0x2;           # Cipher
	#himm 0x200300d4 0x7;           # GZIP
	#himm 0x200300d8 0x2a;          # DDRT¡¢Efuse¡¢DMA 
	
	#himm 0x2003008c 0x2;           # AIO unreset and clock enable,m/f/bclk config in code.
	#himm 0x20030100 0x20;          # RSA
	#himm 0x20030104 0x0;           # AVC-148.5M VGS-148.5M VPSS-99M


	# USB not set
	# SDIO not set 
	# SFC not set 
	# NAND not set
	# RTC use external clk	
	# PWM not set					 #(PERI_CRG38)himm 0x20030038 0x2        
	# DMAC not set 				 #(PERI_CRG56)himm 0x200300E0 0x2        
	# SPI not set 
	# I2C not set
	# SENSE CLK not set 
	# WDG not set 

	echo "clock configure operation done!"
}

#clk_close
clk_cfg
