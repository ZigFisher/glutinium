#!/bin/sh
#
# Export license code

DEV=`cat /proc/cpuinfo | grep machine | cut -d ":" -f 2`
MAC=`ifconfig eth0 | grep HWaddr | awk '{FS=" "; if(NR==1) {print $5}};' | tr 'a-z' 'A-Z'`
XOR=`echo $MAC | awk -F ':' '{print "XOR0(0x"$1"),","XOR1(0x"$2"),","XOR2(0x"$3"),","XOR3(0x"$4"),","XOR4(0x"$5"),","XOR5(0x"$6"),"}'`

clear
echo -e "\n$XOR // $MAC $DEV\n"

