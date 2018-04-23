#!/bin/sh
#
# ZFT Lab. 2018 | http://zftlab.org/pages/2018010700.html
# Telegram Bot - Snapshot


IPC=/mnt/mtd/ipcam.conf
TMP=/var/tmp


if [ -f ${IPC} ]; then
  while read settings
    do local ${settings}
  done < ${IPC}
  #
  stamp=$(date +%Y.%m.%d_%H:%M:%S)
  #
  curl -s -k "http://127.0.0.1/webcapture.jpg?command=snap&channel=1" -o ${TMP}/snap.jpg && \
    curl -s -k -X POST "https://api.telegram.org/bot${telegram_token}/sendPhoto?chat_id=${telegram_group}" -H "Content-Type: multipart/form-data" \
      -F "photo=@/tmp/snap.jpg" -F "caption=${device_name} - ${stamp}" >/dev/null 2>&1 && \
    rm -rf ${TMP}/snap.jpg
  #
fi