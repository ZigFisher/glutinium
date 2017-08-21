#!/bin/sh
#
# Scan home via Radar detector
# Send Message via Telegram
# Create snapshot from IPCam
# Send Photo via Telegram

IPCAM='http://172.28.158.102/webcapture.jpg?command=snap&channel=1'
TOKEN='367706172:AAHqXLtPAUExiF_CUPe42SvrK9b7csi953A'
RUPOR='@Bricket1'
ALARM='RadarDetect!'
MEDIA='Content-Type: multipart/form-data'
DELAY='60'
SLEEP='1'


if [ "`usbgpio statusin | awk -F ' ' '/LED 7/ {print $4}'`" = "1" ] ; then
  logger -t flypeek "${ALARM}"
  #
  last=`cat /tmp/.delay`
  curr=`date +%s`
  [ $(($curr-$last)) -gt ${DELAY} ] && \
    curl -s -k -X POST "https://api.telegram.org/bot${TOKEN}/sendMessage?chat_id=${RUPOR}&text=${ALARM}" && date +%s >/tmp/.delay
  #
  ( STAMP=`date +%Y%m%d%H%M%S` && \
    curl -s -k ${IPCAM} -o /tmp/${STAMP}.jpg && \
    curl -s -k -X POST "https://api.telegram.org/bot${TOKEN}/sendPhoto?chat_id=${RUPOR}" -H "${MEDIA}" -F "photo=@${PHOTO}" -F "caption=TimeStamp: ${STAMP}" && \
    rm -f /tmp/${STAMP}.jpg \
  ) &
fi
