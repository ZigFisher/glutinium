#!/bin/sh

# This is a sample, you should rewrite it according to your chip #

# mddrc pri&timeout setting

himm 0x20110150  0x03ff6         #DMA1 DMA2
himm 0x20110154  0x03ff6         #ETH
himm 0x20110158  0x03ff6         #USB
himm 0x2011015C  0x03ff6         #CIPHER   0x15C
himm 0x20110160  0x03ff6         #SDIO   0X160
himm 0x20110164  0x03ff6         #NAND SFC   0X164
himm 0x20110168  0x10201         #ARMD  0X168
himm 0x2011016C  0x10201         #ARMI	0X16C
himm 0x20110170  0x03ff6         #IVE  0X170
himm 0x20110174  0x03ff6 	 			 #MD, DDR_TEST  0x174
himm 0x20110178  0x03ff6         #JPGE #0x178
himm 0x2011017C  0x03ff3         #TDE0 0X17C
himm 0x20110180  0x03ff4         #VPSS  0X180
himm 0x20110184  0x10c82         #VENC  0X184
himm 0x20110188  0x10101         #VICAP FPGA
#himm 0x20110188 0x10640         #VICAP ESL
himm 0x2011018c  0x10100         #VDP
himm 0x20110100  0x67 		 #mddrc order enable mddrc idmap mode select

#himm 0x20050054 0x123564        #[2:0] VENC [6:4] VPSS [10:8] TDE [14:12] JPGE [18:16] MD

#himm 0x200500d8 0x3             #DDR0只使能VICAP和VDP乱序
himm 0x20050038  0x3             #DDR0只使能VICAP和VDP乱序

#outstanding
cfg_outstanding()
{
	himm 0x20580010 0x3		# VICAP outstanding
	himm 0x205cce00 0x80030030	# VDP	 outstanding
	himm 0x20600314 0x01640000	# VPSS outstanding[r0:3, w4:7]
	himm 0x2061002c 0x00444010  	# TDE  w:[24:21];r:[20:17];r[16:13]
	himm 0x206200A4 0x3		# VENC
	himm 0x206600A4 0x3		# JPEG
}

