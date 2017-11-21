!/bin/sh
#
# Scan home for sitena alarm

DELAY='10'

if [ "`usbgpio statusin | awk -F ' ' '/LED 3/ {print $4}'`" = "1" ] ; then
  #
  usbgpio on 4 && logger -t sirena "Sirena ON".....
  sleep ${DELAY}
  usbgpio off 4 && logger -t sirena "Sirena OFF"
  #
fi


#
# Alarm Buzzer Test
#usbgpio mode 3 1; (while true ; do sleep 1; /usr/sbin/sirena.sh >/dev/null 2>&1; done) &
