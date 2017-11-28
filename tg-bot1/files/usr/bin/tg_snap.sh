#!/bin/sh
#
# Exec /snap command

sysid=`uci get system.@system[0].hostname`
stamp=`date +%Y%m%d%H%M%S`
token=`uci get telegram.bot.token`
rupor=`uci get telegram.bot.rupor`
ipcam="http://10.255.10.247/webcapture.jpg?command=snap&channel=1"


curl -s -k ${ipcam} -o /tmp/snap.jpg && \
  curl -s -k -X POST "https://api.telegram.org/bot${token}/sendPhoto?chat_id=${rupor}" -H "Content-Type: multipart/form-data" \
    -F "photo=@/tmp/snap.jpg" -F "caption=${sysid} - TimeStamp: ${stamp}" >/dev/null 2>&1 && \
  rm -rf /tmp/snap.jpg
