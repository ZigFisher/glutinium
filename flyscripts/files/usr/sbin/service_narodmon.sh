#!/bin/sh
#
# Simple sender script to narodmon.ru service via HTTP GET or TCP
#
#
DEV_NAME="SolarPanel"
DEV_LAT="45.387705" # Default infocom.ua
DEV_LNG="35.760440"
DEV_INT="wlan0"
DEV_MAC="647002F98752" # Atheros
#DEV_MAC=`ifconfig $DEV_INT | grep HWaddr | awk '{FS=" "; if(NR==1) {print $5}};' | tr -d ':'`
#
#
#TEMP_ID="$DEV_MAC"01
#TEMP_DATA=`bmp085 | grep -e "Temperature" | awk '{print $3}'`
#
#PRES_ID="$DEV_MAC"02
#PRES_DATA=`bmp085 | grep -e "Pressure" | awk '{print $3}'`
#
#BRIG_ID="$DEV_MAC"03
#BRIG_DATA=`bh1750 | grep -e "Brightness" | awk '{print $3}'`
#
VOLT_ID="$DEV_MAC"04
VOLT_1=`ina219 -b 0 -w -v | awk '{print $1}'`
VOLT_DATA=`awk 'BEGIN{print '"$VOLT_1"'/'"1000"'}'`
#
CURR_ID="$DEV_MAC"05
CURR_DATA=`ina219 -b 0 -w -c | awk '{print $1}'`
#
BATT_ID="$DEV_MAC"11
BATT_DATA=`ds1621  | awk '{print $1}'`
#BATT_DATA=`lm75 79 | grep -e "Temperature" | awk '{print $3}'`
#
TFRX_ID="$DEV_MAC"21
TFRX_DATA=`ifconfig $DEV_INT | grep bytes | tr ':' ' ' | awk '{print $3}'`
#
TFTX_ID="$DEV_MAC"22
TFTX_DATA=`ifconfig $DEV_INT | grep bytes | tr ':' ' ' | awk '{print $3}'`
#
STRING_HTTP="ID=$DEV_MAC&name=$DEV_NAME&lat=$DEV_LAT&lng=$DEV_LNG&$VOLT_ID=$VOLT_DATA&$CURR_ID=$CURR_DATA&$TFRX_ID=$TFRX_DATA&$TFTX_ID=$TFTX_DATA"
#STRING_HTTP="ID=$DEV_MAC&name=$DEV_NAME&lat=$DEV_LAT&lng=$DEV_LNG&$TEMP_ID=$TEMP_DATA&$PRES_ID=$PRES_DATA&$BRIG_ID=$BRIG_DATA&$BATT_ID=$BATT_DATA&$TFRX_ID=$TFRX_DATA&$TFTX_ID=$TFTX_DATA"
#STRING_TCP="#$DEV_MAC\n#name#$DEV_NAME\n#$TEMP_ID#$TEMP_DATA\n#$PRES_ID#$PRES_DATA\n#$BRIG_ID#$BRIG_DATA\n#$BATT_ID#$BATT_DATA\n#$TFRX_ID#$TFRX_DATA\n#$TFTX_ID#$TFTX_DATA\n##"
#
# Example logging for debug
#echo $STRING_HTTP | logger -t narodmon ; echo $STRING_TCP | logger -t narodmon
#
# Send data via HTTP GET request and write system log
httping -q -c 1 -t 5 -g http://narodmon.ru/post.php?$STRING_HTTP && (echo $STRING_HTTP | logger -t narodmon)
#httping -q -c 1 -t 5 -g "http://api.thingspeak.com/update?api_key=0VKZROPPIY1NQ4S5&field1=$VOLT_1&field2=$CURR_DATA&field3=$BATT_DATA"
#
# Send data via TCP with nc
#echo -e $STRING_TCP | nc narodmon.ru 8283 && (echo $STRING_TCP | logger -t narodmon)

