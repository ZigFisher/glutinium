#!/bin/sh
#
# Push data to ThingSpeak service
#
#APIKEY="0VKZROPPIY1NQ4S5" # Atheros  647002F98752
#APIKEY="D2SLLTC2HJNHN8U2" # Ralink   647002F98752
#APIKEY="U7CT4LRB4BLO9LP2" # Ralink   000C4370AAD3   Vegetary
APIKEY="Z5UDKB7T087NPEV5" # Ralink   2C67FBD3B38C   Semenovka
CLOUDS="http://api.thingspeak.com/update?api_key=$APIKEY"
#
#FIELD4=`am2321 -r | awk '/Temperature/ {print $2}'; sleep 2`
#FIELD5=`am2321 -r | awk '/Humidity/ {print $2}'`
#FIELD1=`bmp085 | awk '/Temperature/ {print $3}'`
#FIELD2=`bmp085 | awk '/Pressure/ {print $3}'`
#FIELD3=`bh1750 | awk '/Brightness/ {print $3}'`
#FIELD3=`ds1621  | awk '{print $1}'`
#FIELD2=`ina219 -b 0 -w -c | awk '{print $1}'`
#FIELD1=`ina219 -b 0 -w -v | awk '{print $1}'`
#FIELD0=`lm75 79 | awk '{print $3}'`
#
#httping -q -c 1 -t 5 -g
#wget -q -O - "$DEV_URL/?host=$DEV_NAM&mac=$DEV_MAC&volt=$FIELD1&curr=$FIELD2&temp=$FIELD3"
wget -q -O - "$CLOUDS&field1=$FIELD1&field2=$FIELD2&field3=$FIELD3"
logger -t service_thingspeak "Send data: host=$DEV_NAM&mac=$DEV_MAC&field1=$FIELD1&field2=$FIELD2&field3=$FIELD3"
