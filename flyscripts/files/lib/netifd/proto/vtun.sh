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
  local findip=`find /bin /sbin /usr/bin /usr/sbin -name ip`

  json_get_var server server
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


  if [ -z "$server" ]; then
    proto_notify_error "$interface" *server* is not defined
    proto_block_restart "$interface"
    return 1
  fi

  if [ -z "$port" ]; then
    port="5000"
    echo "Set port = $port" | logger -t auto-set
  fi

  if [ -z "$transport" ]; then
    transport="tcp"
    echo "Set transport = $transport" | logger -t auto-set
  fi

  if [ -z "$timeout" ]; then
    timeout="60"
    echo "Set timeout = $timeout" | logger -t auto-set
  fi

  if [ -z "$name" ]; then
    proto_notify_error "$interface" *name* is not defined
    proto_block_restart "$interface"
    return 1
  fi

  if [ -z "$password" ]; then
    proto_notify_error "$interface" *password* is not defined
    proto_block_restart "$interface"
    return 1
  fi

  if [ -z "$mode" ]; then
    mode="tap"
    echo "Set mode = $mode" | logger -t auto-set
  fi

  if [ -z "$shaper" ]; then
    shaper="512:512"
    echo "Set shaper = $shaper" | logger -t auto-set
  fi

  case "$mode" in
    tun)
      if [ -z "$hub" ]; then
        proto_notify_error "$interface" *hub* is not defined
        proto_block_restart "$interface"
        return 1
      fi
      #
      if [ -z "$ipaddr" ]; then
        proto_notify_error "$interface" *ipaddr* is not defined
        proto_block_restart "$interface"
        return 1
      fi
    ;;
    ether)
      if [ -z "$bridge" ]; then
        proto_notify_error "$interface" *bridge* is not defined
        proto_block_restart "$interface"
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
    echo "  type $mode;"
    echo "  proto $transport;"
    echo "  speed $speed;"
    echo "  persist yes;"
    echo "  keepalive yes;"
    echo "  compress no;"
    echo "  encrypt no;"
    echo "  stat no;"
    echo "  multi killold;"
    echo "}"
  ) >/tmp/$interface.conf




  echo "$server $port $transport $timeout $mode $shaper    $name $password $hub $ipaddr $bridge" | logger -t setup

  echo "vtund -n -f /tmp/$interface.conf $name $server -P $port" | logger -t run.vtund
  sleep 10
  return 0
}

proto_vtun_teardown() {
  proto_kill_command "$interface"
  echo "proto_vtun_teardown" | logger -t teardown
}

[ -z "NOT_INCLUDED" ] || add_protocol vtun
