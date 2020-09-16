#!/bin/sh

# This is a sample, you should rewrite it according to your chip #

# mddrc pri&timeout setting

devmem 0x20110150 32 0x03ff6         #DMA1 DMA2
devmem 0x20110154 32 0x03ff6         #ETH
devmem 0x20110158 32 0x03ff6         #USB
devmem 0x2011015C 32 0x03ff6         #CIPHER   0x15C
devmem 0x20110160 32 0x03ff6         #SDIO   0X160
devmem 0x20110164 32 0x03ff6         #NAND SFC   0X164
devmem 0x20110168 32 0x10201         #ARMD  0X168
devmem 0x2011016C 32 0x10201         #ARMI  0X16C
devmem 0x20110170 32 0x03ff6         #IVE   0X170
devmem 0x20110174 32 0x03ff6         #MD, DDR_TEST  0x174
devmem 0x20110178 32 0x03ff6         #JPGE #0x178
devmem 0x2011017C 32 0x03ff3         #TDE0 0X17C
devmem 0x20110180 32 0x03ff4         #VPSS  0X180
devmem 0x20110184 32 0x10c82         #VENC  0X184
devmem 0x20110188 32 0x10101         #VICAP FPGA
#devmem 0x20110188 32 0x10640         #VICAP ESL
devmem 0x2011018c 32 0x10100         #VDP
devmem 0x20110100 32 0x67            #mddrc order enable mddrc idmap mode select

#devmem 0x20050054 32 0x123564        #[2:0] VENC [6:4] VPSS [10:8] TDE [14:12] JPGE [18:16] MD

#devmem 0x200500d8 32 0x3             #DDR0只使能VICAP和VDP乱序
devmem 0x20050038  32 0x3             #DDR0只使能VICAP和VDP乱序

#outstanding
cfg_outstanding()
{
	devmem 0x20580010 32 0x3		# VICAP outstanding
	devmem 0x205cce00 32 0x80030030		# VDP outstanding
	devmem 0x20600314 32 0x01640000		# VPSS outstanding[r0:3, w4:7]
	devmem 0x2061002c 32 0x00444010		# TDE  w:[24:21];r:[20:17];r[16:13]
	devmem 0x206200A4 32 0x3		# VENC
	devmem 0x206600A4 32 0x3		# JPEG
}

