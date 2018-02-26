#!/bin/sh
#
# Scan home for sirena alarm

delay='5'

if [ "`usbgpio statusin | awk -F ' ' '/LED 8/ {print $4}'`" = "1" ] ; then
  #
  logger -t sirena "Door open"
  #
  if [ -f /tmp/sirena.run ]; then
    logger -t "Sirena trigger"
  else
    (oko1.sh Door open && tg_snap.sh && tg_snap.sh && tg_snap.sh) &
    usbgpio on 4 && logger -t sirena "Sirena on"
    sleep ${delay}
    usbgpio off 4 && logger -t sirena "Sirena off"
    echo "Trigger" >/tmp/sirena.run
  fi
  #
else
  if [ -f /tmp/sirena.run ]; then
    (oko1.sh Door closed && tg_snap.sh) &
    rm -f /tmp/sirena.run
  fi
  #logger -t sirena "Door closed"
  echo "Waiting"
fi

#
# Alarm Buzzer Test
#usbgpio mode 3 1; (while true ; do sleep 2; /usr/sbin/sirena.sh >/dev/null 2>&1; done) &
