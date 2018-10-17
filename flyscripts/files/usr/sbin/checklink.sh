#!/bin/sh
#
# BackUp ver.20170217
#
export pinger="$(uci get microbe.pinger.checked)"
export normal="$(uci get microbe.pinger.priority | awk -F ',' '{print $1}')"
export backup="$(uci get microbe.pinger.priority | awk -F ',' '{print $2}')"
#
( \
  while sleep 5s; do
    curent=`ip r | awk -F ' ' '/default/ {print $3}'`
    # logger -t routing "Current: ${curent}"
    # Use this string for ETH interfaces. Also use IP GW addresses and "via" in routing rules:
    # ip r replace ${pinger} via ${normal}   
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
