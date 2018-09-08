#!/bin/sh
#
NAME=$(uci get system.@system[0].hostname)
DATE=$(date "+%G.%m.%d %H:%M:%S")
NET1=$(awk '/3g-wan1:/ {print $1,",",$2,",",$10}' /proc/net/dev)
NET2=$(awk '/3g-wan2:/ {print $1,",",$2,",",$10}' /proc/net/dev)
#
if [ ! -n "${NET1}" ] ; then NET1="3g-wan1: , 0 , 0" ; fi
if [ ! -n "${NET2}" ] ; then NET2="3g-wan2: , 0 , 0" ; fi
#
echo "${NAME} , ${DATE} , ${NET1} , ${NET2}"
#
