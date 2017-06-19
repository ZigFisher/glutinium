#!/bin/sh
#
DATE=`date '+%Y%m%d%H%M%S'`
TDIR="/tmp/webcam"
#
#SERV="https://webdav.yandex.ru/Public"
SERV="https://webdav.4shared.com/Public"
USER="octonix"
PASS="mypass"
#
PORT="br-lan"
ICAM="192.168.1.10"
IURL="webcapture.jpg?command=snap&channel=1"
IMAC=`arping -I ${PORT} -c 1 ${ICAM} -q && cat /proc/net/arp | grep "${ICAM} " | awk '{print $4}' | tr -d ':' | tr 'a-z' 'A-Z'`
#
mkdir -p ${TDIR}
wget -q -O ${TDIR}/${IMAC}.jpg "http://${ICAM}/${IURL}" && curl -u ${USER}:${PASS} -T ${TDIR}/${IMAC}.jpg ${SERV}/${DATE}.jpg
