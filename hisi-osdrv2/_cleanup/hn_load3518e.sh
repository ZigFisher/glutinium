#!/bin/sh

####################Variables Definition##########################

insert_sns()
{
	case $SNS_TYPE in
		ar0130|9m034)
			himm 0x200f0040 0x2;    			# I2C0_SCL
			himm 0x200f0044 0x2;    			# I2C0_SDA
			
			#cmos pinmux
			himm 0x200f007c 0x1;    			# VI_DATA13
			himm 0x200f0080 0x1;    			# VI_DATA10
			himm 0x200f0084 0x1;    			# VI_DATA12
			himm 0x200f0088 0x1;    			# VI_DATA11
			himm 0x200f008c 0x2;    			# VI_VS
			himm 0x200f0090 0x2;    			# VI_HS
			himm 0x200f0094 0x1;    			# VI_DATA9
			
			himm 0x2003002c 0xb4001;			# sensor unreset, clk 27MHz, VI 99MHz
			;;
		ar0230)
			himm 0x200f0040 0x2;    			# I2C0_SCL
			himm 0x200f0044 0x2;    			# I2C0_SDA
			
			himm 0x2003002c 0xb4001;			# sensor unreset, clk 27MHz, VI 99MHz
			;;
		imx222)
			himm 0x200f0040 0x1    				# SPI0_SCLK
			himm 0x200f0044 0x1    				# SPI0_SDO
			himm 0x200f0048 0x1    				# SPI0_SDI
			himm 0x200f004c 0x1    				# SPI0_CSN
			
			#cmos pinmux
			himm 0x200f007c 0x1;    			# VI_DATA13
			himm 0x200f0080 0x1;    			# VI_DATA10
			himm 0x200f0084 0x1;    			# VI_DATA12
			himm 0x200f0088 0x1;    			# VI_DATA11
			himm 0x200f008c 0x2;    			# VI_VS
			himm 0x200f0090 0x2;    			# VI_HS
			himm 0x200f0094 0x1;    			# VI_DATA9

			himm 0x2003002c 0x94001;			# sensor unreset, clk 37.125MHz, VI 99MHz
			
			insmod extdrv/sensor_spi.ko;
			;;
		ov9712)
			himm 0x200f0040 0x2;    			# I2C0_SCL
			himm 0x200f0044 0x2;    			# I2C0_SDA
			
			#cmos pinmux
			himm 0x200f007c 0x1;    			# VI_DATA13
			himm 0x200f0080 0x1;    			# VI_DATA10
			himm 0x200f0084 0x1;    			# VI_DATA12
			himm 0x200f0088 0x1;    			# VI_DATA11
			himm 0x200f008c 0x2;    			# VI_VS
			himm 0x200f0090 0x2;    			# VI_HS
			himm 0x200f0094 0x1;    			# VI_DATA9
			
			himm 0x2003002c 0xc4001;			# sensor unreset, clk 24MHz, VI 99MHz
			;;
			
		ov9752)
			himm 0x200f0040 0x2;    			# I2C0_SCL
			himm 0x200f0044 0x2;    			# I2C0_SDA
			
			himm 0x2003002c 0xc4001;			# sensor unreset, clk 24MHz, VI 99MHz
			;;
		mn34220)
			himm 0x200f0040 0x2;    			# I2C0_SCL
			himm 0x200f0044 0x2;    			# I2C0_SDA
			
			himm 0x2003002c 0xc4001;			# sensor unreset, clk 24MHz, VI 99MHz
			;;
		mn34222)
			himm 0x200f0040 0x2;    			# I2C0_SCL
			himm 0x200f0044 0x2;    			# I2C0_SDA
			
			himm 0x2003002c 0x94001;			# sensor unreset, clk 37.125MHz, VI 99MHz
			;;

		ar0330)
			himm 0x200f0040 0x2;    			# I2C0_SCL
			himm 0x200f0044 0x2;    			# I2C0_SDA
			
			himm 0x2003002c 0xc4007;			# sensor unreset, clk 24MHz, VI 99MHz
			himm 0x20680024 0x0;
			himm 0x20681010 0x11;
			himm 0x20681014 0x3210;
			himm 0x20681030 0x1;
			;;
		ov4682)
			himm 0x200f0040 0x2;    			# I2C0_SCL
			himm 0x200f0044 0x2;    			# I2C0_SDA
			
			himm 0x2003002c 0xc4001;			# sensor unreset, clk 24MHz, VI 99MHz
			;;
		bt1120)
			himm 0x200f0008 0x4;    			# VI_VS
			himm 0x200f000c 0x4;    			# VI_HS
			himm 0x200f007c 0x1;    			# VI_DATA13
			himm 0x200f0080 0x1;    			# VI_DATA10
			himm 0x200f0084 0x1;    			# VI_DATA12
			himm 0x200f0088 0x1;    			# VI_DATA11
			himm 0x200f008c 0x1;    			# VI_DATA15
			himm 0x200f0090 0x1;    			# VI_DATA14
			himm 0x200f0094 0x1;    			# VI_DATA9
			
			himm 0x2003002c 0x94003;			# sensor unreset, clk 24MHz, VI 99MHz
			;;

		*)
			echo "xxxx Invalid sensor type $SNS_TYPE xxxx"
			report_error;;
	esac
}

remove_sns()
{
	rmmod ssp &> /dev/null
	rmmod sensor_spi &> /dev/null

}

sys_restore()
{
	####################################################
	pinmux_hi3518e.sh -net > /dev/null
	clkcfg_hi3518e.sh > /dev/null

	# system configuration
	sysctl_hi3518e.sh $b_arg_online  > /dev/null
	insert_sns;
}

######################parse arg###################################
b_arg_sensor=0
b_arg_online=1
b_arg_restore=0

for arg in $@
do
	if [ $b_arg_sensor -eq 1 ] ; then
		b_arg_sensor=0
		SNS_TYPE=$arg;
	fi

	case $arg in
		"-sensor")
			b_arg_sensor=1;
			;;
		"-restore")
			b_arg_restore=1;
			;;
		"-offline")
			b_arg_online=0;
			;;
	esac
done
#######################parse arg end########################

#######################Action###############################
if [ $b_arg_restore -eq 1 ]; then	
	sys_restore;
fi

