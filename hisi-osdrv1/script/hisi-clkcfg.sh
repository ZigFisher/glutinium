#!/bin/sh

# This is a sample, you should rewrite it according to your chip #
# clock will be closed at uboot, so you needn't call it!

clk_close()
{
	# Below clock operation is all from Hi3518, you should modify by datasheet!
	# devmem 0x2003002c 32 0x15       # VIU reset and clock close(PERI_CRG16)
	# devmem 0x20030034 32 0x57       # VOU reset and clock close, DAC PowerDown (PERI_CRG17)
	# devmem 0x20030040 32 0x0        # VEDU reset and clock close (PERI_CRG18)  
	# devmem 0x20030060 32 0x0        # JPEG reset and clock close (PERI_CRG19)
	# devmem 0x20030068 32 0x1        # MDU reset and clock close (PERI_CRG20)
	# devmem 0x2003006c 32 0x1        # IVE reset and clock close (PERI_CRG22)
	# devmem 0x20030058 32 0x1        # TDE reset and clock close (PERI_CRG23)
	# devmem 0x2003008c 32 0x1;       # SIO reset and clock close
	echo "clock close operation done!"
}

# open module clock while you need it!
clk_cfg()
{
	devmem 0x2003002c 32 0x2a;		# VICAP, ISP unreset & clock enable
	devmem 0x20030048 32 0x2;		# VPSS unreset, code also config
	devmem 0x20030034 32 0x510;		# VDP  unreset & HD clock enable
	devmem 0x20030040 32 0x2;		# VEDU unreset
	devmem 0x20030060 32 0x2;		# JPEG unreset
	devmem 0x20030058 32 0x2;		# TDE  unreset
	devmem 0x20030068 32 0x2;		# MDU  unreset

	#devmem 0x2003006c 32 0x2;		# IVE  unreset
	#devmem 0x2003008c 32 0x2;		# SIO unreset and clock enable,m/f/bclk config in code.
	#devmem 0x20050068 32 0x58000000 # Audio Codec channel config for power down.

	# USB not set
	# SDIO not set
	# SFC not set
	# NAND not set
	# CIPHER not set				#(PERI_CRG31) devmem 0x2003007c 32 0x2
	# RTC use external clk
	# PWM not set					#(PERI_CRG38) devmem 0x20030038 32 0x2
	# DMAC not set					#(PERI_CRG56) devmem 0x200300E0 32 0x2
	# SPI not set
	# I2C not set
	# SENSE CLK not set
	# WDG not set

	echo "clock configure operation done!"
}

#clk_close
clk_cfg
