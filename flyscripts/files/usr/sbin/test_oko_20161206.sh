
# Push data to FlyPeek service

usbgpio mode 7 1
(while true ; do sleep 1; /usr/sbin/oko.sh >/dev/null 2>&1; done) &

**********************************************************************************

#!/bin/sh
#
# Create snapshot via radar search

if [ "`usbgpio statusin | awk -F ' ' '/LED 7/ {print $4}'`" = "1" ] ; then
  logger -t service_flypeek "Radar is active !"
  # /usr/sbin/service_flypeek_usb.sh
  /usr/sbin/service_flypeek_ipc.sh
fi
