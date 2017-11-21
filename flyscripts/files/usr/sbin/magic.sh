!/bin/sh -x

IPCAM='http://192.168.1.10/webcapture.jpg?command=snap&channel=1'
TOKEN='My:Token'
RUPOR='@AlarmNauki'
STAMP=`date +%Y%m%d%H%M%S`

curl -s -k ${IPCAM} -o /tmp/${STAMP}.jpg && \
  curl -s -k -X POST "https://api.telegram.org/bot${TOKEN}/sendPhoto?chat_id=${RUPOR}" -H "Content-Type: multipart/form-data" -F "photo=@/tmp/${STAMP}.jpg" -F "caption=Test: ${STAMP}" && rm -f /tmp/${STAMP}.jpg 

