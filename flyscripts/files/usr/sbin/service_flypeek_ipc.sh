#!/bin/sh
#
# Push data to FlyPeek service
#
SERVER="172.19.24.1" # or vbox3.flymon.net
LOGIN="www-data"
PASSW="webcam2015"
WCDIR="/tmp/webcam"
#
IPCAM="172.17.32.121"
#
DEV_INT="br-lan"
DEV_MAC=`arping -I $DEV_INT -c 1 $IPCAM | awk '/reply/ {print 0$5}' | tr -d '[]:' | tr 'a-z' 'A-Z'`
#
mkdir -p $WCDIR
wget -q -O $WCDIR/$DEV_MAC.jpg "http://$IPCAM/webcapture.jpg?command=snap&channel=1"
ftpput -u $LOGIN -p $PASSW $SERVER $DEV_MAC.jpg $WCDIR/$DEV_MAC.jpg && logger -t service_flypeek "Send snapshot from $DEV_MAC device"
