#!/bin/sh
#
# Push data to FlyPeek service
#
SERVER="172.19.24.1" # or vbox3.flymon.net
LOGIN="www-data"
PASSW="webcam2015"
WCDIR="/tmp/webcam"
#
DEV_INT="eth0"
DEV_MAC=`ifconfig $DEV_INT | grep HWaddr | awk '{FS=" "; if(NR==1) {print $5}};' | tr -d ':' | tr 'a-z' 'A-Z' `
#
mkdir -p $WCDIR
#fswebcam --input 0 --fps 2 --skip 10 --resolution 640x480 --no-title --no-banner --save $WCDIR/snapshot.jpg --quiet
wget -q -O $WCDIR/$DEV_MAC.jpg "http://127.0.0.1:8080/?action=snapshot"
ftpput -u $LOGIN -p $PASSW $SERVER $DEV_MAC.jpg $WCDIR/$DEV_MAC.jpg && logger -t service_flypeek "Send snapshot from $DEV_MAC device"
