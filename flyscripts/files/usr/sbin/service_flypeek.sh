#!/bin/sh
#
DATE=`date '+%Y%m%d%H%M%S'`
HOST=`uci get system.@system[0].hostname`
TDIR="/tmp/webcam"
#
#IMAC=`arping -I ${PORT} -c 1 ${ICAM} -q && cat /proc/net/arp | grep "${ICAM} " | awk '{print $4}' | tr -d ':' | tr 'a-z' 'A-Z'`
IMAC=`ifconfig eth0 | grep HWaddr | awk '{FS=" "; if(NR==1) {print $5}};' | tr -d ':' | tr 'a-z' 'A-Z'`
#
#SURL="http://127.0.0.1:8080/?action=snapshot"
SURL="http://192.168.1.10/webcapture.jpg?command=snap&channel=1"
#
#SERV="https://webdav.yandex.ru/${HOST}"
SERV="https://webdav.4shared.com/${HOST}"
#
USER="octonix"
PASS="mypass"
#
#
#fswebcam --input 0 --fps 2 --skip 10 --resolution 640x480 --no-title --no-banner --save ${TDIR}/${IMAC}.jpg --quiet
#ftpput -u ${USER} -p ${PASS} ${SERV} mouse/${DATE}.jpg ${TDIR}/${IMAC}.jpg
#
mkdir -p ${TDIR}
wget -q -O ${TDIR}/${IMAC}.jpg ${SURL} && curl -k -u ${USER}:${PASS} -T ${TDIR}/${IMAC}.jpg ${SERV}/${DATE}.jpg && logger -t flypeek "Send snapshot from ${IMAC} device"
