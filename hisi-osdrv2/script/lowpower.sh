#!/bin/sh

#关闭USB PHY的总电源：0x20120078 [12]bit配置为0
himm 0x20120078 0x0c0301a0

#关闭USB 2.0 PHY的时钟：0x200300b8 [7]bit配置0
himm 0x200300b8 0x127

#SAR ADC POWER_DOWN_MODEL（打开后在不使用ADC的时候自动power_down）[14]bit配置1
himm 0x200b0000 0x0000c0ff

#打开PWM
himm 0x20030038 0x2

#关闭IR [0]bit配置0
himm 0x20070000 0x0

#IR 管脚复用成gpio
himm 0x200f0070 0x0

#UART2不使能：0x200A0000 [9][8][0]bit都配置为0
himm 0x200A0030 0x0

#UART2管脚复用成gpio
himm 0x200f00cc 0x0
himm 0x200f00d0 0x0

#关闭SPI0和SPI1
himm 0x200C0004 0x7F00
himm 0x200E0004 0x7F00

#spi0 管脚复用成gpio
himm 0x200f0040 0x0    # 000：GPIO3_3；
himm 0x200f0044 0x0    # 000：GPIO3_4；
himm 0x200f0048 0x0    # 000：GPIO3_5；
himm 0x200f004c 0x0    # 000：GPIO3_6；

#spi1 管脚复用成gpio
himm 0x200f0030 0x0    # 000：GPIO0_3；
himm 0x200f0050 0x0    # 000：GPIO3_7；
himm 0x200f0054 0x0    # 000：GPIO4_0；
himm 0x200f0058 0x0    # 000：GPIO4_1；
himm 0x200f005c 0x0    # 000：GPIO4_2；

#AUDIO CODEC LINE IN 关闭左声道
#himm 0x20050068 0xa8022c2c
#himm 0x2005006c 0xf5035a4a
