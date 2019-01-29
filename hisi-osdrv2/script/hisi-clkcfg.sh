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
	devmem  0x2003002c 32 0xc4003;        # VICAP, ISP unreset & clock enable, Sensor clock enable, clk reverse
	#devmem 0x20030034 32 0x64ff4;        # 6bit LCD
	#devmem 0x20030034 32 0x164ff4;       # 8bit LCD
	#devmem 0x20030034 32 0xff4;          # bt656

	#devmem  0x20030040 32 0x2000;        # AVC unreset, code also config
	#devmem  0x20030048 32 0x2;           # VPSS unreset, code also config
	#devmem  0x20030058 32 0x2;           # TDE  unreset
	#devmem  0x2003005c 32 0x2;           # VGS
	#devmem  0x20030060 32 0x2;           # JPGE unreset
	#devmem  0x20030068 32 0x02000000;    # LCD 27M:0x04000000, 13.5M:0x02000000
	#devmem  0x2003006c 32 0xa;           # IVE/HASH  unreset
	#devmem  0x2003007c 32 0x2;           # Cipher
	#devmem  0x200300d4 32 0x7;           # GZIP
	#devmem  0x200300d8 32 0x2a;          # DDRT Efuse DMA

	#devmem  0x2003008c 32 0x2;           # AIO unreset and clock enable,m/f/bclk config in code.
	#devmem  0x20030100 32 0x20;          # RSA
	#devmem  0x20030104 32 0x0;           # AVC-148.5M VGS-148.5M VPSS-99M

	# USB not set
	# SDIO not set
	# SFC not set
	# NAND not set
	# RTC use external clk
	# PWM not set                         #(PERI_CRG38)devmem  0x20030038 32 0x2
	# DMAC not set                        #(PERI_CRG56)devmem  0x200300E0 32 0x2
	# SPI not set
	# I2C not set
	# SENSE CLK not set
	# WDG not set

	echo "clock configure operation done!"
}

#clk_close
clk_cfg
