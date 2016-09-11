#!/bin/sh

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
}

proto_vtun_setup() {
	local interface="$1"

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


	case "$service" in
		cdma|evdo)
			chat="/etc/chatscripts/evdo.chat"
		;;
		*)
			chat="/etc/chatscripts/3g.chat"
			cardinfo=$(gcom -d "$device" -s /etc/gcom/getcardinfo.gcom)
			
			if echo "$cardinfo" | grep -q Novatel; then
				case "$service" in
					umts_only) CODE=2;;
					gprs_only) CODE=1;;
					*) CODE=0;;
				esac
				export MODE="AT\$NWRAT=${CODE},2"
			elif echo "$cardinfo" | grep -qi huawei; then
				case "$service" in
					umts_only) CODE="14,2";;
					gprs_only) CODE="13,1";;
					*) CODE="2,2";;
				esac
				export MODE="AT^SYSCFG=${CODE},3FFFFFFF,2,4"
			fi

		;;
	esac


	if [ -z "$server" ]; then
		proto_notify_error "$interface" *server* is not set
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


	echo $server $port $transport $timeout $name $password $mode $shaper $hub $ipaddr | logger -t setup

	echo "vtund -n -f /tmp/$interface.conf $name $server -P $port" | logger -t run.vtund

	sleep 10

	return 0
}

proto_vtun_teardown() {
	proto_kill_command "$interface"
	echo "proto_vtun_teardown" | logger -t teardown
}

[ -z "NOT_INCLUDED" ] || add_protocol vtun

