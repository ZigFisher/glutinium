#!/bin/sh
#
# Simple script for generate vtund.conf
# 2012.07.27 v0.1
#
# Manual: http://vtun.sourceforge.net/vtund.conf

UPDATE=`date +%G%m%d-%H%M`
CONFDIR="/etc"
PROFILE="vtund.dat"
CONFIG="vtund.conf"


mkdir -p /var/lock/vtund

clear
rm -f $CONFDIR/$CONFIG
( echo "options {"
  echo "  syslog daemon;"
  echo "  timeout 60;"
  echo "  ip /bin/ip;"
  echo "}"
  echo "default {"
  echo "  type tun;"
  echo "  proto tcp;"
  echo "  persist yes;"
  echo "  keepalive yes;"
  echo "  compress no;"
  echo "  encrypt no;"
  echo "  stat no;"
  echo "  speed 512:512;"
  echo "  multi killold;"
  echo "}"
) >>$CONFDIR/$CONFIG
#
for host in \
  `cat $CONFDIR/$PROFILE | grep ^~ | awk '{print $2}' | tr '\n' ' '`
    do
      #echo "Create config for $host ..."
      checker=`grep -e "${host} " $CONFDIR/$PROFILE`
      if [ -n "$checker" ]; then
        link=`grep -e "${host} " $CONFDIR/$PROFILE | awk '{print $2}'`
        type=`grep -e "${host} " $CONFDIR/$PROFILE | awk '{print $3}'`
        speed=`grep -e "${host} " $CONFDIR/$PROFILE | awk '{print $4}'`
        password=`grep -e "${host} " $CONFDIR/$PROFILE | awk '{print $5}'`
        servip=`grep -e "${host} " $CONFDIR/$PROFILE | awk '{print $6}'`
        hostip=`grep -e "${host} " $CONFDIR/$PROFILE | awk '{print $7}'`
        descr=`grep -e "${host} " $CONFDIR/$PROFILE | awk '{print $8}'`
        #
        ( echo "#"
          echo "### $descr ###"
          echo "#"
          echo "$link {"
          if [ "$type" = "tun" ]; then
             echo "  type tun;"
             echo "  speed $speed;"
          else
             echo "  type ether;"
             echo "  speed $speed;"
          fi
          echo "  password $password;"
          echo "  device $link;"
          echo "  up {"
          if [ "$type" = "tun" ]; then
            echo "    ip \"link set %% up multicast off mtu 1500\";"
            echo "    ip \"-family inet addr add $servip peer $hostip dev %%\";"
          else
            echo "    ip \"link set %% up multicast off mtu 1500\";"
            echo "    program \"brctl addif $type %%\";"
            echo "    program \"iptables -I FORWARD -j ACCEPT -o $type -i $type\";"
          fi
          echo "  };"
          echo "  down {"
          if [ "$type" = "tun" ]; then
            echo "    ip \"link set %% down\";"
          else
            echo "    program \"brctl delif $type %%\";"
            echo "    ip \"link set %% down\";"
          fi
          echo "  };"
          echo "}"
        ) >>$CONFDIR/$CONFIG
        #
      fi
    done
echo "All configs created..."
#
sleep 1
echo ""
ls -l /var/lock/vtund/ | awk '{print $9,"-",$7,$6,$8}'
echo "Total active `ls -l /var/lock/vtund/ | wc -l` tunnels"
sleep 3
