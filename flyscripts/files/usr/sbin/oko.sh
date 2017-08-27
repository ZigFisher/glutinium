#!/bin/sh
#
# Scan home via Radar detector
# Grab snapshot from connected IPCam
# Send Message and Photo via Telegram

IPCAM='http://192.168.1.10/webcapture.jpg?command=snap&channel=1'
TOKEN='YOU:TOKEN'
RUPOR='YouChannel_-ID_or_@Name'
ALARM='RadarDetect:'
YPASS='user:pass'
SYSID=`uci get system.@system[0].hostname`
DELAY='60'
SLEEP='1'
LIMIT='3'


if [ "`usbgpio statusin | awk -F ' ' '/LED 7/ {print $4}'`" = "1" ] ; then
  STAMP=`date +%Y%m%d%H%M%S`
  FILES=`ls -l /tmp/*.jpg 2>/dev/null | wc -l`
  logger -t oko "${ALARM}"
  #
  last=`cat /tmp/.delay`
  curr=`date +%s`
  [ $(($curr-$last)) -gt ${DELAY} ] && \
    curl -s -k -X POST "https://api.telegram.org/bot${TOKEN}/sendMessage?chat_id=${RUPOR}&text=${SYSID}_${ALARM}_${STAMP}" && date +%s >/tmp/.delay
  #
  # Send photo to Telegram channel
  if [ "${FILES}" -lt "${LIMIT}" ]; then
    ( curl -s -k ${IPCAM} -o /tmp/${STAMP}.jpg && \
      curl -s -k -X POST "https://api.telegram.org/bot${TOKEN}/sendPhoto?chat_id=${RUPOR}" -H "Content-Type: multipart/form-data" -F "photo=@/tmp/${STAMP}.jpg" -F "caption=${SYSID} TimeStamp: ${STAMP}" && \
      logger -t oko "TX to Telegram OK" && \
      rm -f /tmp/${STAMP}.jpg \
    ) &
   else
    logger -t oko "Limit for files buffer"
  fi
  #
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
