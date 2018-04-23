#!/bin/sh
#
# Exec /relay command

#STATUS=`cat /sys/class/gpio/gpio20/value`


if [ "${STATUS}" == "1" ]; then
  echo "Relay current ON => turn it to OFF"
  #echo "0" >/sys/class/gpio/gpio20/value
fi

if [ "${STATUS}" == "0" ]; then
  echo "Relay current OFF => turn it to ON"
  #echo "1" >/sys/class/gpio/gpio20/value
fi
