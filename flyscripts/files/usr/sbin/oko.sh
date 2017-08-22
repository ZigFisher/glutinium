#!/bin/sh
#
# Scan home via Radar detector
# Grab snapshot from connected IPCam
# Send Message and Photo via Telegram

IPCAM='http://192.168.1.10/webcapture.jpg?command=snap&channel=1'
TOKEN='367706172:AAHqXLtPAUExiF_CUPe42SvrK9b7csi953A'
RUPOR='@Bricket1'
ALARM='RadarDetect!'
YPASS='user:pass'
DELAY='60'
SLEEP='1'


if [ "`usbgpio statusin | awk -F ' ' '/LED 7/ {print $4}'`" = "1" ] ; then
  STAMP=`date +%Y%m%d%H%M%S`
  logger -t oko "${ALARM}"
  #
  last=`cat /tmp/.delay`
  curr=`date +%s`
  [ $(($curr-$last)) -gt ${DELAY} ] && \
    curl -s -k -X POST "https://api.telegram.org/bot${TOKEN}/sendMessage?chat_id=${RUPOR}&text=${ALARM}" && date +%s >/tmp/.delay
  #
  # Send photo to Telegram channel
  ( curl -s -k ${IPCAM} -o /tmp/${STAMP}.jpg && \
    curl -s -k -X POST "https://api.telegram.org/bot${TOKEN}/sendPhoto?chat_id=${RUPOR}" -H "Content-Type: multipart/form-data" -F "photo=@/tmp/${STAMP}.jpg" -F "caption=TimeStamp: ${STAMP}" && \
    logger -t oko "TX to Telegram OK" && \
    rm -f /tmp/${STAMP}.jpg \
  ) &
  # Send photo to Yandex.Disk
  #( curl -s -k ${IPCAM} -o /tmp/${STAMP}.jpg && \
  #  curl -s -k -u "${YPASS}" -T "/tmp/${STAMP}.jpg" https://webdav.yandex.ru/Public/${STAMP}.jpg && \
  #  logger -t oko "TX to Yandex.Disk OK" && \
  #  rm -f /tmp/${STAMP}.jpg \
  #) &
  #
  sleep ${SLEEP}
  #
fi
