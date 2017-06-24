#!/bin/sh
#
# Scan home via Radar detector
# Send SMS via AlphaSMS service
# Create snapshot from IPCam

SERVICE="https://alphasms.ua/api/http.php?version=http&command=send"
LOGIN="+380670000000"
PASSWORD="mypass"
FROM="Test"
CLIENT="+380500000000"
MESSAGE="AlarmVision"
DELAY="60"


if [ "`usbgpio statusin | awk -F ' ' '/LED 7/ {print $4}'`" = "1" ] ; then
  logger -t flypeek "RadarDetect !"
  #
  last=`cat /tmp/.smsdelay`
  curr=`date +%s`
  [ $(($curr-$last)) -gt ${DELAY} ] && \
    curl -k "${SERVICE}&login=${LOGIN}&password=${PASSWORD}&from=${FROM}&to=${CLIENT}&message=${MESSAGE}" | logger -t alphasms && date +%s >/tmp/.smsdelay
  #
  /usr/sbin/service_flypeek.sh
fi
