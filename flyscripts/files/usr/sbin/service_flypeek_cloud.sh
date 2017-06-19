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
#SURL="http://127.0.0.1:8080/?action=snapshot"
SURL="http://192.168.1.10/webcapture.jpg?command=snap&channel=1"
ICAM="192.168.1.10"
IMAC=`arping -I ${PORT} -c 1 ${ICAM} -q && cat /proc/net/arp | grep "${ICAM} " | awk '{print $4}' | tr -d ':' | tr 'a-z' 'A-Z'`
#
mkdir -p ${TDIR}
wget -q -O ${TDIR}/${IMAC}.jpg ${SURL} && curl -u ${USER}:${PASS} -T ${TDIR}/${IMAC}.jpg ${SERV}/${DATE}.jpg
