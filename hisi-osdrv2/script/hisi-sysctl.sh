#!/bin/sh

# This is a sample, you should rewrite it according to your chip #


# mddrc pri&timeout setting

#########################################################################################
# param $1=1 --- online
# param $1=0 --- offline

vi_vpss_online_config()
{
	# -------------vi vpss online open
	if [ $b_vpss_online -eq 1 ]; then
		echo "==============vi_vpss_online==============";
		devmem  0x20120004 32 0x40001000;			# online, SPI1 CS0; [12]-ive
		# pri config
		devmem  0x20120058 32 0x26666401			# each module 4bit£∫vedu       ddrt_md  ive  aio    jpge    tde   vicap  vdp
		devmem  0x2012005c 32 0x66666103			# each module 4bit£∫sfc_nand   sfc_nor  nfc  sdio1  sdio0   a7    vpss   vgs
		devmem  0x20120060 32 0x66266666			# each module 4bit£∫reserve    reserve  avc  usb    cipher  dma2  dma1   gsf
		# timeout config
		devmem  0x20120064 32 0x00000011			# each module 4bit£∫vedu       ddrt_md  ive  aio    jpge    tde   vicap  vdp
		devmem  0x20120068 32 0x00000010			# each module 4bit£∫sfc_nand   sfc_nor  nfc  sdio1  sdio0   a7    vpss   vgs
		devmem  0x2012006c 32 0x00000000			# each module 4bit£∫reserve    reserve  avc  usb    cipher  dma2  dma1   gsf
	else
		echo "==============vi_vpss_offline==============";
		devmem  0x20120004 32 0x1000;		   		# offline, mipi SPI1 CS0; [12]-ive
		# pri config
		devmem  0x20120058 32 0x26666400			# each module 4bit£∫vedu       ddrt_md  ive  aio    jpge    tde   vicap  vdp
		devmem  0x2012005c 32 0x66666123			# each module 4bit£∫sfc_nand   sfc_nor  nfc  sdio1  sdio0   a7    vpss   vgs
		devmem  0x20120060 32 0x66266666			# each module 4bit£∫reserve    reserve  avc  usb    cipher  dma2  dma1   gsf
		# timeout config
		devmem  0x20120064 32 0x00000011			# each module 4bit£∫vedu       ddrt_md  ive  aio    jpge    tde   vicap  vdp
		devmem  0x20120068 32 0x00000000			# each module 4bit£∫sfc_nand   sfc_nor  nfc  sdio1  sdio0   a7    vpss   vgs
		devmem  0x2012006c 32 0x00000000			# each module 4bit£∫reserve    reserve  avc  usb    cipher  dma2  dma1   gsf
	fi
}
#########################################################################################


# mddrc pri&timeout setting
mddrc_pri_tmout_setting()
{
	#–¥√¸¡Ó”≈œ»º∂
	devmem  0x201100c0 32 0x76543210     # ports0
	devmem  0x201100c4 32 0x76543210     # ports1
	devmem  0x201100c8 32 0x76543210     # ports2
	devmem  0x201100cc 32 0x76543210     # ports3
	devmem  0x201100d0 32 0x76543210     # ports4
	devmem  0x201100d4 32 0x76543210     # ports5
	devmem  0x201100d8 32 0x76543210     # ports6

	#∂¡√¸¡Ó”≈œ»º∂
	devmem  0x20110100 32 0x76543210     # ports0
	devmem  0x20110104 32 0x76543210     # ports1
	devmem  0x20110108 32 0x76543210     # ports2
	devmem  0x2011010c 32 0x76543210     # ports3
	devmem  0x20110110 32 0x76543210     # ports4
	devmem  0x20110114 32 0x76543210     # ports5
	devmem  0x20110118 32 0x76543210     # ports6

	#–¥√¸¡Ótimeout
	devmem  0x20110140 32 0x08040200     # ports0
	devmem  0x20110144 32 0x08040100     # ports1
	devmem  0x20110148 32 0x08040200     # ports2
	devmem  0x2011014c 32 0x08040200     # ports3
	devmem  0x20110150 32 0x08040200     # ports4
	devmem  0x20110154 32 0x08040200     # ports5
	devmem  0x20110158 32 0x08040200     # ports6

	#∂¡√¸¡Ótimeout                 
	devmem  0x20110180 32 0x08040200     # ports0
	devmem  0x20110184 32 0x08040200     # ports1
	devmem  0x20110188 32 0x08040200     # ports2
	devmem  0x2011018c 32 0x08040200     # ports3
	devmem  0x20110190 32 0x08040200     # ports4
	devmem  0x20110194 32 0x08040200     # ports5
	devmem  0x20110198 32 0x08040200     # ports6

	#map mode
	devmem   0x20110040 32  0x01001000   # ports0
	devmem   0x20110044 32  0x01001000   # ports1
	devmem   0x20110048 32  0x01001000   # ports2
	devmem   0x2011004c 32  0x01001000   # ports3
	devmem   0x20110050 32  0x01001000   # ports4
	devmem   0x20110054 32  0x01001000   # ports5
	devmem   0x20110058 32  0x01001000   # ports6
}



devmem  0x201200E0 32 0xd			# internal codec£¨AIO MCLK out, CODEC AIO TX MCLK
#devmem  0x201200E0 32 0xe			# external codec: AIC31£¨AIO MCLK out, CODEC AIO TX MCLK


echo "++++++++++++++++++++++++++++++++++++++++++++++"
b_vpss_online=1

if [ $# -ge 1 ]; then
    b_vpss_online=$1
fi

vi_vpss_online_config;
mddrc_pri_tmout_setting;

# outstanding
cfg_outstanding()
{
	devmem  0x20580010 32 0x00003030		# VICAP outstanding[r12:15, w4:7]
	devmem  0x205c0034 32 0x00000003		# VDP   outstanding
	devmem  0x20600314 32 0x00000033		# VPSS  outstanding[r0:3, w4:7]
	#devmem  0x20610844 32 0x00444010  	        # TDE   w:[24:21];r:[20:17];r[16:13]
	devmem  0x206200A4 32 0x3			# AVC
	devmem  0x206600A4 32 0x3			# JPEG
}

# ive utili
#devmem  0x206A0000 32	0x2				# Open utili statistic
#devmem  0x206A0080 32 0x11E1A300  		        # Utili peri,default 0x11E1A300 cycle

