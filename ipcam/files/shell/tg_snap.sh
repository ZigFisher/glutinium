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
  if [ ${socks5_enable} = 1 ]; then
    curl_options="-s -k --socks5-hostname ${socks5_server}:${socks5_port} --proxy-user ${socks5_login}:${socks5_password}"
  else
    curl_options="-s -k"
  fi
  #
  stamp=$(date +%Y.%m.%d_%H:%M:%S)
  PASSW=$(cat /mnt/mtd/Config/Account1 | jsonfilter -e '@.Users[@.Name="admin"].Password')
  #
  curl -s -k "http://127.0.0.1/webcapture.jpg?command=snap&channel=1&user=admin&password=${PASSW}" -o ${TMP}/snap.jpg && \
    curl ${curl_options} -X POST "https://api.telegram.org/bot${telegram_token}/sendPhoto?chat_id=${telegram_group}" -H "Content-Type: multipart/form-data" \
      -F "photo=@/tmp/snap.jpg" -F "caption=${device_name} - ${stamp}" >/dev/null 2>&1 && \
    rm -rf ${TMP}/snap.jpg
  #
fi