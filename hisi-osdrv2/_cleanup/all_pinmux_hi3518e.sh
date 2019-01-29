#!/bin/sh

# This is a sample, you should rewrite it according to your chip #

# You can configure your pinmux for the application here!

#VICAP default setting is VIU
vicap_pin_mux()
{
	himm 0x200f0000 0x00000001		# 0: GPIO0_4		1: SENSOR_CLK
	himm 0x200f0004 0x00000000		# 0: SENSOR_RSTN	1: GPIO0_5		
	himm 0x200f0008 0x00000001		# 0: GPIO0_6		1£ºFLASH_TRIG	2: SFC_EMMC_BOOT_MODE	3£ºSPI1_CSN1	4:VI_VS
	himm 0x200f000c 0x00000001		# 0£ºGPIO0_7	 	1£ºSHUTTER_TRIG	2£ºSFC_DEVICE_MODE		4: VI_HS
}

#SPI1 -> LCD 
spi1_pim_mux()
{
	himm 0x200f0050 0x1				# 001£ºSPI1_SCLK£»
	himm 0x200f0054 0x1				# 001£ºSPI1_SDO£»
	himm 0x200f0058 0x1				# 001£ºSPI1_SDI£»
	himm 0x200f005c 0x1				# 001£ºSPI1_CSN0£»
}

#I2C0 -> sensor
i2c0_pin_mux()
{
	himm 0x200f0040 0x00000002		# 0: GPIO3_3		1:spi0_sclk		2:i2c0_scl
	himm 0x200f0044 0x00000002		# 0: GPIO3_4		1:spi0_sdo		2:i2c0_sda
}

#I2C1 -> 7179
i2c1_pin_mux()
{
	himm 0x200f0050 0x00000002		# 010£ºI2C1_SCL£»
	himm 0x200f0054 0x00000002		# 010£ºI2C1_SDA£»
}

i2c2_pin_mux()
{
	himm 0x200f0060 0x1;			# i2c2_sda
	himm 0x200f0064 0x1;			# i2c2_scl
}

vo_output_mode()
{
	echo "------vo_output_mode------"
	#pinmux
	himm 0x200f0010 0x00000003		# 3£ºVO_CLK   & 0: GPIO2_0  & 1: RMII_CLK
	himm 0x200f0014 0x00000000		# 3£ºVO_VS    & 0: GPIO2_1  & 1: RMII_TX_EN & 4: SDIO1_CARD_DETECT
	himm 0x200f0018 0x00000003		# 3£ºVO_DATA5 & 0: GPIO2_2	& 1: RMII_TXD0	& 4: SDIO1_CWPR
	himm 0x200f001c 0x00000000		# 3£ºVO_DE    & 0: GPIO2_3	& 1: RMII_TXD1	& 4: SDIO1_CDATA1
	himm 0x200f0020 0x00000003		# 3£ºVO_DATA7 & 0: GPIO2_4	& 1: RMII_RX_DV	& 4: SDIO1_CDATA0
	himm 0x200f0024 0x00000003		# 3£ºVO_DATA2 & 0: GPIO2_5	& 1: RMII_RXD0	& 4: SDIO1_CDATA3
	himm 0x200f0028 0x00000003		# 3£ºVO_DATA3 & 0: GPIO2_6	& 1: RMII_RXD1	& 4: SDIO1_CCMD
	himm 0x200f002c 0x00000000		# 3£ºVO_HS    & 0: GPIO2_7	& 1: EPHY_RST	& 2: BOOT_SEL	& 4: SDIO1_CARD_POWER_EN
	himm 0x200f0030 0x00000003		# 3£ºVO_DATA0 & 0: GPIO0_3	& 1: SPI1_CSN1	
	himm 0x200f0034 0x00000003		# 3£ºVO_DATA1 & 0: GPIO3_0	& 1: EPHY_CLK	& 4: SDIO1_CDATA2
	himm 0x200f0038 0x00000003		# 3: VO_DATA6 & 0: GPIO3_1	& 1: MDCK		& 2£ºBOOTROM_SEL
	himm 0x200f003c 0x00000003		# 3£ºVO_DATA4 & 0: GPIO3_2	& 1: MDIO
	
}

bt656_drive_capability()
{
	#BT656 drive capability config
	himm 0x200f0810 0xd0;    		# VO_CLK     
	himm 0x200f0830 0x90;    		# VO_DATA0 
	himm 0x200f0834 0xd0;    		# VO_DATA1 
	himm 0x200f0824 0x90;    		# VO_DATA2 
	himm 0x200f0828 0x90;    		# VO_DATA3 
	himm 0x200f083c 0x90;    		# VO_DATA4
	himm 0x200f0818 0x90;    		# VO_DATA5 
	himm 0x200f0838 0x90;    		# VO_DATA6
	himm 0x200f0820 0x90;    		# VO_DATA7
}

lcd_drive_capability()
{
	#LCD drive capability config
	himm  0x200f0810 0xe0 
	himm  0x200f0830 0xa0  
	himm  0x200f0834 0xe0 
	himm  0x200f0824 0xa0
	himm  0x200f0828 0xa0
	himm  0x200f083c 0xa0
	himm  0x200f0818 0xa0
	himm  0x200f0838 0xa0
	himm  0x200f0820 0xa0
	himm  0x200f081c 0xa0
}

#RMII    
net_rmii_mode()
{
	echo "------net_rmii_mode------"
	himm 0x200f002c 0x00000001;     # 1: EPHY_RST   & 0: GPIO2_7  & 2: BOOT_SEL & 3£ºVO_HS & 4: SDIO1_CARD_POWER_EN
	himm 0x200f0034 0x00000001;     # 1: EPHY_CLK   & 0: GPIO3_0  & 3£ºVO_DATA1 & 4: SDIO1_CDATA2
	#
	himm 0x200f0010 0x00000001;     # 1: RMII_CLK   & 0: GPIO2_0  & 3£ºVO_CLK
	himm 0x200f0014 0x00000001;     # 1: RMII_TX_EN & 0: GPIO2_1  & 3£ºVO_VS    & 4: SDIO1_CARD_DETECT
	himm 0x200f0018 0x00000001;     # 1: RMII_TXD0  & 0: GPIO2_2  & 3£ºVO_DATA5 & 4: SDIO1_CWPR
	himm 0x200f001c 0x00000001;     # 1: RMII_TXD1  & 0: GPIO2_3  & 3£ºVO_DE    & 4: SDIO1_CDATA1
	himm 0x200f0020 0x00000001;     # 1: RMII_RX_DV & 0: GPIO2_4  & 3£ºVO_DATA7 & 4: SDIO1_CDATA
	himm 0x200f0024 0x00000001;     # 1: RMII_RXD0  & 0: GPIO2_5  & 3£ºVO_DATA2 & 4: SDIO1_CDATA3
	himm 0x200f0028 0x00000001;     # 1: RMII_RXD1  & 0: GPIO2_6  & 3£ºVO_DATA3 & 4: SDIO1_CCMD£»
	#
	himm 0x200f0038 0x00000001;     # 1: MDCK 	    & 0: GPIO3_1  & 2£ºBOOTROM_SEL & 3: VO_DATA6  
	himm 0x200f003c 0x00000001;     # 1: MDIO       & 0: GPIO3_2  & 3£ºVO_DATA4
	
	#ephy drive capability config
	himm 0x200f0810 0xd0;			# RMII_CLK
	himm 0x200f0814 0xa0;			# RMII_TX_EN
	himm 0x200f0818 0xa0;			# RMII_TXD0
	himm 0x200f081c 0xa0;			# RMII_TXD1
	himm 0x200f0820 0xb0;			# RMII_RX_DV
	himm 0x200f0824 0xb0;			# RMII_RXD0
	himm 0x200f0828 0xb0;			# RMII_RXD1
	himm 0x200f082c 0xb0;			# EPHY_RST
	himm 0x200f0834 0xd0;			# EPHY_CLK
	himm 0x200f0838 0x90;			# MDCK
	himm 0x200f083c 0xa0;			# MDIO
}      



######################parse arg###################################
b_arg_vo=0
b_arg_net=0

for arg in $@
do
	if [ $b_arg_vo -eq 1 ] ; then
		vo_type=$arg;
	fi
	case $arg in
		"-vo")
			b_arg_vo=1;
			;;
		"-net")
			b_arg_net=1;
			;;
	esac
done
#######################parse arg end########################

#######################Action###############################
if [ $b_arg_vo -eq 1 ]; then
	case $vo_type in
		"BT656")
			echo "===========VO TYPE BT656============";
			i2c1_pin_mux;         	                  #i2c1 -> 7179
			vo_output_mode;       	                  #vo output pin mux
			bt656_drive_capability;					  #drive capability
			rmmod adv_7179;              
			insmod extdrv/adv_7179.ko;                #PAL in default, 
			#insmod extdrv/adv_7179.ko norm_mode=1;   #NTSC     
			;;
		"LCD")
			echo "============VO TYPE LCD=============";
			spi1_pim_mux;                            #spi1 -> LCD
			vo_output_mode;                       	 #vo output pin mux
			lcd_drive_capability; 				     #drive capability
			himm 0x200f0014 0x00000003		         # 3£ºVO_VS    & 0: GPIO2_1  & 1: RMII_TX_EN & 4: SDIO1_CARD_DETECT
			himm 0x200f002c 0x00000003		         # 3£ºVO_HS    & 0: GPIO2_7	& 1: EPHY_RST	& 2: BOOT_SEL	& 4: SDIO1_CARD_POWER_EN
			himm 0x200f001c 0x00000003		         # 3£ºVO_DE    & 0: GPIO2_3	& 1: RMII_TXD1	& 4: SDIO1_CDATA1
			insmod extdrv/ssp_ili9342h_6bit.ko;      # ili9342 6bit LCD
			#insmod extdrv/ssp_ili9341v_6bit.ko;     # ili9341 6bit LCD
			#insmod extdrv/ssp_ota5182.ko;           # ota5182 8bit LCD
			;;
	esac
fi

if [ $b_arg_net -eq 1 ]; then
	echo "==============NET MODE================";
	net_rmii_mode;
fi

# When external audio codec be used,you must open only one of the i2s_pin_mux(i2s0,i2s1 or i2s2).
# If you has some problem on it ,you can refer to tlv320aic31_readme.txt include in audio sample. 
i2s_pin_mux()
{
	# pin_mux with GPIO1 
	#himm 0x200f007c 0x3;		    # i2s_bclk_tx
	#himm 0x200f0080 0x3;		    # i2s_sd_tx
	#himm 0x200f0084 0x3;		    # i2s_mclk
	#himm 0x200f0088 0x3;		    # i2s_ws_tx
	#himm 0x200f008c 0x3;		    # i2s_ws_rx
	#himm 0x200f0090 0x3;		    # i2s_bclk_rx
	#himm 0x200f0094 0x3;		    # i2s_sd_rx

	# pin_mux with UART1 
	himm 0x200f00bc 0x2;		    # i2s_sd_tx
	himm 0x200f00c0 0x2;		    # i2s_ws_tx
	himm 0x200f00c4 0x2;		    # i2s_mclk
	himm 0x200f00c8 0x2;		    # i2s_sd_rx
	himm 0x200f00d0 0x2;		    # i2s_bclk_tx

	# pin_mux with JTAG
	#himm 0x200f00d4 0x3;		    # i2s_mclk
	#himm 0x200f00d8 0x3;		    # i2s_ws_tx
	#himm 0x200f00dc 0x3;		    # i2s_sd_tx
	#himm 0x200f00e0 0x3;		    # i2s_sd_rx
	#himm 0x200f00e4 0x3;		    # i2s_bclk_tx
}


#i2c0_pin_mux;
#i2c2_pin_mux
vicap_pin_mux;
#i2s_pin_mux;
#vo_bt656_mode;
