#!/bin/sh
#
# Push data to Yandex.Disk
#
DRIVE="https://webdav.yandex.ru/Public"
LOGIN="myname"
PASSW="mypass"
SNAPS="http://127.0.0.1:8080/?action=snapshot"
WCDIR="/tmp/webcam"
#
DEV_INT="eth0"
DEV_MAC=`ifconfig $DEV_INT | grep HWaddr | awk '{FS=" "; if(NR==1) {print $5}};' | tr -d ':' | tr 'a-z' 'A-Z' `
#
mkdir -p $WCDIR
wget -q -O $WCDIR/$DEV_MAC.jpg $SNAPS && curl -k -u $LOGIN:$PASSW -T $WCDIR/$DEV_MAC.jpg $DRIVE/$DEV_MAC.jpg