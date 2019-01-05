#!/bin/sh
#based on https://dev.openwrt.org/browser/trunk/package/mac80211/files/lib/wifi/mac80211.sh?rev=16627
append DRIVERS "wext"

scan_wext() {
	local device="$1"
	local adhoc sta monitor

	config_get vifs "$device" vifs
	for vif in $vifs; do

		config_get ifname "$vif" ifname
		config_set "$vif" ifname "${ifname:-$device}"

		config_get mode "$vif" mode
		case "$mode" in
			adhoc|sta|monitor)
				append $mode "$vif"
			;;
			*) echo "$device($vif): Invalid mode, ignored."; continue;;
		esac
	done

	config_set "$device" vifs "${adhoc:+$adhoc }${sta:+$sta }${monitor:+$monitor }"
}


disable_wext() (
	local device="$1"

	set_wifi_down "$device"
	# kill all running hostapd and wpa_supplicant processes that
	# are running on wext vifs
	for pid in `pidof hostapd wpa_supplicant`; do
		grep "$device" /proc/$pid/cmdline >/dev/null && \
			kill $pid
	done

	include /lib/network
	cd /proc/sys/net
	for dev in *; do
		grep "$device" "$dev/%parent" >/dev/null 2>/dev/null && {
			ifconfig "$dev" down
			unbridge "$dev"
		}
	done
	return 0
)

enable_wext() {
	local device="$1"
	config_get channel "$device" channel
	config_get vifs "$device" vifs
	config_get txpower "$device" txpower

	local first=1
	#wifi_fixup_hwmode "$device" "g"
	#iwconfig needs the interface to be up
	ifconfig "$device" up
	for vif in $vifs; do
		config_get enc "$vif" encryption
		config_get eap_type "$vif" eap_type
		config_get mode "$vif" mode

		config_set "$vif" ifname "$device"

		[ "$first" = 1 ] && {
			# only need to change freq band and channel on the first vif
			iwconfig "$device" channel "$channel" >/dev/null 2>/dev/null
			if [ "$mode" = adhoc ]; then
				iwlist "$ifname" scan >/dev/null 2>/dev/null
				sleep 1
				iwconfig "$device" mode ad-hoc >/dev/null 2>/dev/null
			fi
			iwconfig "$device" channel "$channel" >/dev/null 2>/dev/null
		}
		if [ "$mode" = sta ]; then
			iwconfig "$device" mode managed >/dev/null 2>/dev/null
		else
			iwconfig "$device" mode $mode >/dev/null 2>/dev/null
		fi

		wpa=
		case "$enc" in
			WEP|wep)
				for idx in 1 2 3 4; do
					config_get key "$vif" "key${idx}"
					iwconfig "$device" enc "[$idx]" "${key:-off}"
				done
				config_get key "$vif" key
				key="${key:-1}"
				case "$key" in
					[1234]) iwconfig "$device" enc "[$key]";;
					*) iwconfig "$device" enc "$key";;
				esac
			;;
			PSK|psk|PSK2|psk2)
				config_get key "$vif" key
			;;
		esac

		case "$mode" in
			adhoc)
				config_get addr "$vif" bssid
				[ -z "$addr" ] || {
					iwconfig "$ifname" ap "$addr"
				}
			;;
		esac
		config_get ssid "$vif" ssid

		config_get vif_txpower "$vif" txpower
		# use vif_txpower (from wifi-iface) to override txpower (from
		# wifi-device) if the latter doesn't exist
		txpower="${txpower:-$vif_txpower}"
		[ -z "$txpower" ] || iwconfig "$device" txpower "${txpower%%.*}"

		config_get frag "$vif" frag
		if [ -n "$frag" ]; then
			iwconfig "$device" frag "${frag%%.*}"
		fi

		config_get rts "$vif" rts
		if [ -n "$rts" ]; then
			iwconfig "$device" rts "${rts%%.*}"
		fi

		#ifconfig "$device" up
		iwconfig "$device" channel "$channel" >/dev/null 2>/dev/null

		local net_cfg bridge
		net_cfg="$(find_net_config "$vif")"
		[ -z "$net_cfg" ] || {
			bridge="$(bridge_interface "$net_cfg")"
			config_set "$vif" bridge "$bridge"
			start_net "$device" "$net_cfg"
		}
		iwconfig "$device" essid "$ssid"
		set_wifi_up "$vif" "$device"
		case "$mode" in
			sta)
				if eval "type wpa_supplicant_setup_vif" 2>/dev/null >/dev/null; then
					wpa_supplicant_setup_vif "$vif" wext || {
						echo "enable_wext($device): Failed to set up wpa_supplicant for interface $ifname" >&2
						# make sure this wifi interface won't accidentally stay open without encryption
						#ifconfig "$device" down
						continue
					}
				fi
			;;
		esac
		first=0
	done
}


detect_wext() {
	cd /sys/class/net
	for dev in $(ls -d ra* 2>&-); do
		config_get type "$dev" type
		[ "$type" = wext ] && return
		cat <<EOF
config wifi-device  $dev
	option type     wext

	# REMOVE THIS LINE TO ENABLE WIFI:
	option disabled 1

config wifi-iface
	option device	$dev
	option network	lan
	option mode	sta
	option ssid	OpenWrt
	option encryption none
EOF
	done
}
