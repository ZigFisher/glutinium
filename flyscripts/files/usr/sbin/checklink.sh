#!/bin/sh
#
# BackUp ver.20170217
#
export pinger="8.8.8.8"
export normal="3g-wan1"
export backup="3g-wan2"
#
( \
  while sleep 5s; do
    curent=`ip r | awk -F ' ' '/default/ {print $3}'`
    logger -t routing "Current: ${curent}"
    if ping -q -c 6 -I 3g-wan1 ${pinger} ; then
      [ "${curent}" != ${normal} ] && ip route replace default dev ${normal} && logger -t routing "Now link via normal channel"
      [ "${curent}" != ${normal} ] && kill -s HUP `pidof vtund`
      [ "${curent}" != ${normal} ] && kill -s HUP `pidof openvpn`
      [ "${curent}" != ${normal} ] && /etc/init.d/ipsec restart
    else
      [ "${curent}" != ${backup} ] && ip route replace default dev ${backup} && logger -t routing "Now link via backup channel"
      [ "${curent}" != ${backup} ] && kill -s HUP `pidof vtund`
      [ "${curent}" != ${backup} ] && kill -s HUP `pidof openvpn`
      [ "${curent}" != ${backup} ] && /etc/init.d/ipsec restart
    fi
  done
)
