#!/bin/sh
#
# Simple script for generate vtund.conf
# 2012.07.27 v0.1
#
# Manual: http://vtun.sourceforge.net/vtund.conf


[ -n "$INCLUDE_ONLY" ] || {
  NOT_INCLUDED=1
  INCLUDE_ONLY=1

  . ../netifd-proto.sh
  init_proto "$@"
}

proto_vtun_init_config() {
  no_device=1
  available=1
  proto_config_add_string "server"
  proto_config_add_string "backup"
  proto_config_add_string "port"
  proto_config_add_string "transport"
  proto_config_add_string "timeout"
  proto_config_add_string "name"
  proto_config_add_string "password"
  proto_config_add_string "mode"
  proto_config_add_string "shaper"
  proto_config_add_string "hub"
  proto_config_add_string "ipaddr"
  proto_config_add_string "bridge"
}

proto_vtun_setup() {
  local interface="$1"
  local findip=$(find /bin /sbin /usr/bin /usr/sbin -name ip | head -n 1)

  json_get_var server server
  json_get_var backup backup
  json_get_var port port
  json_get_var transport transport
  json_get_var timeout timeout
  json_get_var name name
  json_get_var password password
  json_get_var mode mode
  json_get_var shaper shaper
  json_get_var hub hub
  json_get_var ipaddr ipaddr
  json_get_var bridge bridge



  if [ -z "$backup" ]; then
    backup="none"
    # echo "Set backup = $backup" | logger -t vtun.autoset
  fi

  if [ -z "$port" ]; then
    port="5000"
    # echo "Set port = $port" | logger -t vtun.autoset
  fi

  if [ -z "$transport" ]; then
    transport="tcp"
    # echo "Set transport = $transport" | logger -t vtun.autoset
  fi

  if [ -z "$timeout" ]; then
    timeout="60"
    # echo "Set timeout = $timeout" | logger -t vtun.autoset
  fi

  if [ -z "$shaper" ]; then
    shaper="0:0"
    # echo "Set shaper = $shaper" | logger -t vtun.autoset
  fi

  if [ -z "$server" ]; then
    echo "Warning, on "$interface" *server* is not defined" | logger -t vtun.error
    return 1
  fi

  if [ -z "$name" ]; then
    echo "Warning, on "$interface" *name* is not defined" | logger -t vtun.error
    return 1
  fi

  if [ -z "$password" ]; then
    echo "Warning, on "$interface" *password* is not defined" | logger -t vtun.error
    return 1
  fi

  if [ -z "$mode" ]; then
    echo "Warning, on "$interface" *mode* is not defined" | logger -t vtun.error
  fi


  case "$mode" in
    tun)
      if [ -z "$hub" ]; then
        echo "Warning, on "$interface" *hub* is not defined" | logger -t vtun.error
        return 1
      fi
      #
      if [ -z "$ipaddr" ]; then
        echo "Warning, on "$interface" *ipaddr* is not defined" | logger -t vtun.error
        return 1
      fi
    ;;
    ether)
      if [ -z "$bridge" ]; then
        echo "Warning, on "$interface" *bridge* is not defined" | logger -t vtun.error
        return 1
      fi
    ;;
  esac


  ( echo "options {"
    echo "  syslog daemon;"
    echo "  timeout $timeout;"
    echo "  ip $findip;"
    echo "}"
    echo "default {"
    echo "  proto $transport;"
    echo "  type $mode;"
    echo "  speed $shaper;"
    echo "  persist no;"
    echo "  keepalive 60:5;"
    echo "  compress no;"
    echo "  encrypt no;"
    echo "  stat no;"
    echo "  multi killold;"
    echo "}"
    echo "#"
  for multiname in `echo $name`; do
    echo "$multiname {"
    echo "  password $password;"
    #echo "  device $multiname;"
    echo "  up {"
    if [ "$mode" = "tun" ]; then
      echo "    ip \"link set %% up multicast off mtu 1500\";"
      echo "    ip \"-family inet addr add $ipaddr peer $hub dev %%\";"
    fi
    if [ "$mode" = "ether" ]; then
      if [ "$bridge" = "br-null" ]; then
        if [ -z "$ipaddr" ]; then
          echo "    ip \"link set %% up multicast off mtu 1500\";"
        else
          echo "    ip \"link set %% up multicast off mtu 1500\";"
          echo "    ip \"addr add $ipaddr dev %%\";"
        fi
      else
        echo "    ip \"link set %% up multicast off mtu 1500\";"
        echo "    program \"brctl addif $bridge %%\";"
        echo "    program \"iptables -I FORWARD -j ACCEPT -o $bridge -i $bridge\";"
      fi
    fi
    echo "  };"
    echo "  down {"
    if [ "$mode" = "tun" ]; then
      echo "    ip \"link set %% down\";"
    fi
    if [ "$mode" = "ether" ]; then
      if [ "$bridge" = "br-null" ]; then
        if [ -z "$ipaddr" ]; then
          echo "    ip \"link set %% down\";"
        else
          echo "    ip \"addr add $ipaddr dev %%\";"
          echo "    ip \"link set %% down\";"
        fi
      else
        echo "    program \"brctl delif $bridge %%\";"
        echo "    ip \"link set %% down\";"
      fi
    fi
    echo "  };"
    echo "}"
  done
  ) >/tmp/$interface.conf


  if [ "$server" = "true" ]; then
    mkdir -p /var/lock/vtund
    vtund -n -f /tmp/$interface.conf -s -P $port
  else
    if [ "$backup" = "none" ]; then
      vtund -n -f /tmp/$interface.conf $name $server -P $port
    else
      vtund -n -f /tmp/$interface.conf $name $server -P $port ; vtund -n -f /tmp/$interface.conf $name $backup -P $port
    fi
  fi

  sleep 5

  return 0
}

proto_vtun_teardown() {
  proto_kill_command "$interface"
  echo "proto_vtun_teardown" | logger -t teardown
}

[ -z "NOT_INCLUDED" ] || add_protocol vtun
