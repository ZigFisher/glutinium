#!/bin/sh
#
# option method   'radar/period'                                               # Работа по срабатыванию ИК/СВЧ датчика или по временному интервалу
# option period   '5m'                                                         # Установка временного интервала
# option radar    'gpio7'                                                      # Установка контрольного GPIO при работе в режиме Radar
# option service  'yandex/4share'                                              # Сервис отправки данных по протоколу WebDav, поддерживаются Yandex.com и 4share.com
# option login    'myname'                                                     # Имя пользователя на сервисе
# option password 'mypass'                                                     # Пароль на сервисе
# option folder   'device'                                                     # Название папки на сервисе, если указан device, то будет использовано имя устройства
# option snapshot 'http://192.168.1.10/webcapture.jpg?command=snap&channel=1'  # Где брать снапшот с IP камеры
# option snapshot 'http://127.0.0.1:8080/?action=snapshot'                     # Где брать снапшот с USB камеры
# option sms...
# option buffer...
#
DATE=`date '+%Y%m%d%H%M%S'`
HOST=`uci get system.@system[0].hostname`
TDIR="/tmp/webcam"
#
#IMAC=`arping -I ${PORT} -c 1 ${ICAM} -q && cat /proc/net/arp | grep "${ICAM} " | awk '{print $4}' | tr -d ':' | tr 'a-z' 'A-Z'`
IMAC=`ifconfig eth0 | grep HWaddr | awk '{FS=" "; if(NR==1) {print $5}};' | tr -d ':' | tr 'a-z' 'A-Z'`
#
SURL="http://192.168.1.10/webcapture.jpg?command=snap&channel=1"
#
#SERV="https://webdav.yandex.ru/${HOST}"
SERV="https://webdav.4shared.com/FlyPeek/${HOST}"
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
