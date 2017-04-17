#!/bin/sh

CLOUD="http://monitor.flymon.net:81/"

NAME=`uci get system.@system[0].hostname`
MACID=`ifconfig eth0 | grep HWaddr | awk '{FS=" "; if(NR==1) {print $5}};' | tr -d ':' | tr 'a-z' 'A-Z'`
WANIP=`ifconfig br-lan | grep 'Mask:' | tr ':' ' ' | awk '{print $3}' | head -n 1`
MEMORY=`free | awk '/Mem:/ {print $4}' | egrep '^[-+]?[0-9]*\.?[0-9]+$'`
UPTIME=`cat /proc/uptime | cut -d '.' -f 1 | egrep '^[-+]?[0-9]*\.?[0-9]+$'`

httping -q -G -c 1 -t 5 -g "$CLOUD?host=$NAME&mac=$MACID&wanip=$WANIP&freemem=$MEMORY&uptime=$UPTIME"
