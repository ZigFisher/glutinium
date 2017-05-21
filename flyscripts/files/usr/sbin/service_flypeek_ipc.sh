#!/bin/sh
#
# Push data to FlyPeek service
#
DATE=`date '+%Y%m%d%H%M%S'`
SERVER="172.19.24.1" # or vbox3.flymon.net
LOGIN="www-data"
PASSW="webcam2015"
WCDIR="/tmp/webcam"
#
IPCAM="192.168.1.10"
#
DEV_INT="br-lan"
DEV_MAC=`arping -I $DEV_INT -c 1 $IPCAM -q && cat /proc/net/arp | grep "$IPCAM " | awk '{print $4}' | tr -d ':' | tr 'a-z' 'A-Z'`
#
mkdir -p $WCDIR
wget -q -O $WCDIR/$DEV_MAC.jpg "http://$IPCAM/webcapture.jpg?command=snap&channel=1"
#ftpput -u $LOGIN -p $PASSW $SERVER mouse/$DATE.jpg $WCDIR/$DEV_MAC.jpg && logger -t service_flypeek "Send snapshot from $DEV_MAC device"
curl -u Octonix:mypass -T $WCDIR/$DEV_MAC.jpg https://webdav.4shared.com/vbox4628/$DATE.jpg