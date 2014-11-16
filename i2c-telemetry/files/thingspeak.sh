#!/bin/sh
#
# Simple script for push data to ThingSpeak service
#
#
APIKEY="INSERT-YOU-KEY"
CLOUDS="http://api.thingspeak.com/update?api_key=$APIKEY"
#
FIELD1=`lm75 79 | awk '{print $3}'`
#
wget -q -O - "$CLOUDS&field1=$FIELD1" && logger -t thingspeak "$CLOUDS&field1=$FIELD1"
#
