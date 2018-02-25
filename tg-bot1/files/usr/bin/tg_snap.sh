#!/bin/sh
#
# Exec /snap command

ipcam=$(uci get telegram.bot.ipcam)
rupor=$(uci get telegram.bot.rupor)
stamp=$(date +%Y.%m.%d_%H:%M:%S)
sysid=$(uci get system.@system[0].hostname)
token=$(uci get telegram.bot.token)


curl -s -k ${ipcam} -o /tmp/snap.jpg && \
  curl -s -k -X POST "https://api.telegram.org/bot${token}/sendPhoto?chat_id=${rupor}" -H "Content-Type: multipart/form-data" \
    -F "photo=@/tmp/snap.jpg" -F "caption=${sysid} - ${stamp}" >/dev/null 2>&1 && \
  rm -rf /tmp/snap.jpg
