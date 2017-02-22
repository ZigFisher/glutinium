#!/bin/sh
append DRIVERS "ralink"

reload_ralink() {
	local vif
	for vif in ra0 ra1 ra2 ra3 ra4 ra5 ra6 ra7 wds0 wds1 wds2 wds3 apcli0; do
	ifconfig $vif down 2>/dev/null
	set_wifi_down $vif
	done
	#rmmod rt2860v2_ap
	#insmod rt2860v2_ap
}

scan_ralink() {
	local device="$1"
	local ifname="$1"

	ralink_phy_if=0
	
#	reload_ralink

	local i=-1
	
	while grep -qs "^ *ra$((++i)):" /proc/net/dev; do
	let ralink_phy_if+=1
	done

	ralink_ap_num=0
	ralink_wds_num=0
	
	config_get vifs "$device" vifs
	
	for vif in $vifs; do
		config_get_bool disabled "$vif" disabled 0
		[ $disabled -eq 0 ] || continue

		config_get mode "$vif" mode
		case "$mode" in
			adhoc)
				echo "ralink only support ap+wds+sta!"
			;;
			ap)
				ralink_ap_num=$(($ralink_ap_num + 1))
				apmode=1
				ralink_ap_vif="${ralink_ap_vif:+$ralink_ap_vif }$vif"
			;;
			wds)
				config_get wds_addr "$vif" bssid
				[ -z "$wds_addr" ] || {
					wds_addr=$(echo "$wds_addr" | tr 'A-F' 'a-f')
					append ralink_wds "$wds_addr"
				}
			;;
			monitor)
			;;
			*) echo "$device($vif): Invalid mode";;
		esac
	done
}


ralink_prepare_config() {

	local ssid_num=0 apcli_num=0 mode disabled
	local device=$1
	config_get channel $device channel
	config_get hwmode $device mode
	config_get wmm $device wmm
	config_get txpower $device txpower
	config_get ht $device ht
	config_get country $device country
	config_get macpolicy $device macpolicy
	config_get maclist $device maclist
	ra_maclist="${maclist// /;};"
	config_get_bool greenap $device greenap 0
	config_get_bool antdiv "$device" diversity
	config_get frag "$device" frag 2346
	config_get rts "$device" rts 2347
	config_get distance "$device" distance
	config_get hidessid "$device" hidden 0
	config_get vifs "$device" vifs

	[ "$channel" != "auto" ] && {
	[ ${channel:-0} -ge 1 -a ${channel:-0} -le 11 ] && countryregion=0
	[ ${channel:-0} -ge 12 -a ${channel:-0} -le 13 ] && countryregion=1
	[ ${channel:-0} -eq 14 ] && countryregion=31
	}

for vif in $vifs; do
	config_get_bool disabled "$vif" disabled 0
	config_get mode "$vif" mode 0
	
	[ "$disabled" == "1" ]&& {
	set_wifi_down $vif
	continue
	}
	
	[ "$hidessid" == "0" ] && {
	config_get hidessid $vif hidden 0
	}
	
	[ "$mode" == "sta" ] || [ "$mode" == "wds" ] && {
	continue
	}

	let ssid_num+=1 #SSID Number
	case $ssid_num in
	1)
		config_get ssid1 "$vif" ssid
		;;
	2)
		config_get ssid2 "$vif" ssid
		;;
	3)
		config_get ssid3 "$vif" ssid
		;;
	4)
		config_get ssid4 "$vif" ssid
		;;
	*)
		echo "Only support 4 MSSID!"
		;;
	esac
done

	HT=1
	HT_CE=1

    if [ "$ht" = "20" ]; then
      HT=0 
    elif [ "$ht" = "20+40" ]; then
      HT=1 
      HT_CE=1
    elif [ "$ht" = "40" ] ; then
      HT=1 
      HT_CE=0
    else
    echo "ht config error!use default!!!"
      HT=0
      HT_CE=1
    fi
	
	EXTCHA=1
	
	[ "$channel" != "auto" ] && [ "$channel" -lt "5" ] && EXTCHA=0

    [ "$channel" == "auto" ] && {
        channel=11
        AutoChannelSelect=2 
    }

    case "$macpolicy" in
	allow|2)
	ra_macfilter=1;
	;;
	deny|1)
	ra_macfilter=2;
	;;
	*|disable|none|0)
	ra_macfilter=0;
	;;
    esac
					
	cat > /tmp/RT2860.dat<<EOF
#The word of "Default" must not be removed
Default
CountryRegion=${countryregion:-0}
CountryRegionABand=7
CountryCode=${country:-US}
BssidNum=${ssid_num:-1}
SSID1=${ssid1:-Wrtnode}
SSID2=
SSID3=
SSID4=
WirelessMode=${hwmode:-9}
TxRate=0
Channel=${channel:-11}
BasicRate=15
BeaconPeriod=100
DtimPeriod=1
TxPower=${txpower:-100}
DisableOLBC=0
BGProtection=0

axStaNum=0
TxPreamble=0
RTSThreshold=${rts:-2347}
FragThreshold=${frag:-2346}
TxBurst=1
PktAggregate=0
TurboRate=0
WmmCapable=${wmm:-0}
APSDCapable=0
DLSCapable=0
APAifsn=3;7;1;1
APCwmin=4;4;3;2
APCwmax=6;10;4;3
APTxop=0;0;94;47
APACM=0;0;0;0
BSSAifsn=3;7;2;2
BSSCwmin=4;4;3;2
BSSCwmax=10;10;4;3
BSSTxop=0;0;94;47
BSSACM=0;0;0;0
AckPolicy=0;0;0;0
NoForwarding=0
NoForwardingBTNBSSID=0
HideSSID=0
StationKeepAlive=0
ShortSlot=1
AutoChannelSelect=${AutoChannelSelect:-0}
IEEE8021X=0
IEEE80211H=0
CSPeriod=10
WirelessEvent=0
IdsEnable=0
AuthFloodThreshold=32
AssocReqFloodThreshold=32
ReassocReqFloodThreshold=32
ProbeReqFloodThreshold=32
DisassocFloodThreshold=32
DeauthFloodThreshold=32
EapReqFooldThreshold=32
PreAuth=0
AuthMode=OPEN
EncrypType=NONE
RekeyInterval=0
RekeyMethod=DISABLE
PMKCachePeriod=10
WPAPSK1=
WPAPSK2=
WPAPSK3=
WPAPSK4=
DefaultKeyID=1
Key1Type=0
Key1Str1=
Key1Str2=
Key1Str3=
Key1Str4=
Key2Type=0
Key2Str1=
Key2Str2=
Key2Str3=
Key2Str4=
Key3Type=0
Key3Str1=
Key3Str2=
Key3Str3=
Key3Str4=
Key4Type=0
Key4Str1=
Key4Str2=
Key4Str3=
Key4Str4=
HSCounter=0
AccessPolicy1=0
AccessControlList1=
AccessPolicy2=0
AccessControlList2=
AccessPolicy3=0
AccessControlList3=
TxBurst=0
WdsEnable=0
WdsEncrypType=NONE
WdsList=
WdsKey=
RADIUS_Server=
RADIUS_Port=1812
RADIUS_Key=ralink
own_ip_addr=
EAPifname=br-lan
PreAuthifname=br-lan
HT_HTC=0
HT_RDG=0
HT_EXTCHA=${EXTCHA}
HT_LinkAdapt=0
HT_OpMode=0
HT_MpduDensity=5
HT_BW=${HT:-0}
HT_AutoBA=1
HT_AMSDU=0
HT_BAWinSize=64
HT_GI=1
HT_MCS=33
HT_BSSCoexistence=${HT_CE:-1}

eshId=MESH

eshAutoLink=1

eshAuthMode=OPEN

eshEncrypType=NONE

eshWPAKEY=

eshDefaultkey=1

eshWEPKEY=
WscManufacturer=
WscModelName=
WscDeviceName=
WscModelNumber=
WscSerialNumber=
RadioOn=1
PMFMFPC=0
PMFMFPR=0
PMFSHA256=0
AccessPolicy0=${ra_macfilter:-0}
AccessControlList0=${ra_maclist:-0}
EOF

}
disable_ralink() {
	local vif
	for vif in ra0 ra1 ra2 ra3 ra4 ra5 ra6 ra7 wds0 wds1 wds2 wds3 apcli0; do
	ifconfig $vif down 2>/dev/null
	set_wifi_down $vif
	done
	
	true
}

enable_ralink_wps_pbc() {
#iwpriv $1 set WscConfMode=7 
#iwpriv $1 set WscConfStatus=2
#iwpriv $1 set WscMode=2
#iwpriv $1 set WscV2Support=1
echo "ok"
}

ralink_start_vif() {
	local vif="$1"
	local ifname="$2"

	local net_cfg
	net_cfg="$(find_net_config "$vif")"
	[ -z "$net_cfg" ] || start_net "$ifname" "$net_cfg"

	set_wifi_up "$vif" "$ifname"
}

enable_ralink() {
	
	local device="$1" dmode if_num=-1;
	
	[ -f /etc/Wireless/RT2860/RT2860.dat ] || {
	mkdir -p /etc/Wireless/RT2860/ 2>/dev/null
	ln -s /tmp/RT2860.dat /etc/Wireless/RT2860/RT2860.dat 2>/dev/null
	}	
	
	reload_ralink
	config_get_bool disabled "$device" disabled 0	
	if [ "$disabled" = "1" ] ;then
	return
	fi
	
	ralink_prepare_config $device
	config_get dmode $device mode
	config_get vifs "$device" vifs
	config_get maxassoc $device maxassoc 0
	config_get channel $device channel 0
	
	local ralink_vifs ralink_ap_vifs ralink_wds_vifs
	
	for vif in $vifs; do
		config_get mode "$vif" mode
		[ "$mode" == "ap" ] && {
			ralink_ap_vifs="$ralink_ap_vifs $vif"
		}
		[ "$mode" == "wds" ] && {
			ralink_wds_vifs="$ralink_ap_vifs $vif"
		}
	done
	
	ralink_vifs="$ralink_vifs $ralink_ap_vifs $ralink_wds_vifs "
	
#	for vif in $vifs; do
	for vif in $ralink_vifs; do
		config_get_bool disabled $vif disabled 0
		[ "$disabled" == "1" ] && {
			continue
		}
		
		local ifname encryption key ssid mode
		
#		config_get ifname $vif device			
		config_get encryption $vif encryption
		config_get key $vif key
		config_get ssid $vif ssid
		config_get mode $vif mode
		
		local wps pin
		config_get wps $vif wps 
		config_get pin $vif pin
		
		config_get isolate $vif isolate 0
		config_get hidessid $vif hidden 0
		config_get doth $vif doth 0

#		config_get hidessid $vif hidden 0	

		[ "$mode" != "sta" ] && [ "$mode" != "wds" ] && {
		let if_num+=1
		
		ifname="ra$if_num"
		}
		

		[ "$mode" == "ap" ] && {
			[ "$key" = "" -a "$vif" = "private" ] && {
				logger "no key set serial"
				key="AAAAAAAAAA"
			}
			[ "$dmode" == "6" ] && wpa_crypto="aes"
			ifconfig $ifname up
			echo "#Encryption" >/tmp/wifi_encryption_${ifname}.dat
			iwpriv $ifname set "SSID=${ssid}"
			case "$encryption" in
				wpa*|psk*|WPA*|Mixed|mixed)
					local enc
					case "$encryption" in
						Mixed|mixed|psk+psk2|psk-mixed)
							enc=WPAPSKWPA2PSK
							;;
						WPA2*|wpa2*|psk2*)
							enc=WPA2PSK
							;;
						WPA*|WPA1*|wpa*|wpa1*|psk*)
							enc=WPAPSK
							;;
					esac
					local crypto="TKIPAES"
					case "$encryption" in
					*tkip+aes*|*tkip+ccmp*|*aes+tkip*|*ccmp+tkip*)
						crypto="TKIPAES"
						;;
					*aes*|*ccmp*)
						crypto="AES"
						;;
					*tkip*) 
						crypto="TKIP"
						echo Warring!!! TKIP not support in 802.11n 40Mhz!!!
						;;
					*)
						crypto="TKIPAES"
						;;	
					esac
					echo "$enc" >>/tmp/wifi_encryption_${ifname}.dat
					echo "$crypto" >>/tmp/wifi_encryption_${ifname}.dat
					iwpriv $ifname set AuthMode=$enc
					iwpriv $ifname set EncrypType=$crypto
					iwpriv $ifname set IEEE8021X=0
					iwpriv $ifname set "SSID=${ssid}"
					iwpriv $ifname set "WPAPSK=${key}"
					iwpriv $ifname set DefaultKeyID=1
					iwpriv $ifname set "SSID=${ssid}"
					;;
					
				WEP|wep|wep-open|wep-shared)
					echo "WEP" >>/tmp/wifi_encryption_${ifname}.dat
					iwpriv $ifname set AuthMode=WEPAUTO
					iwpriv $ifname set EncrypType=WEP
					iwpriv $ifname set IEEE8021X=0
					for idx in 1 2 3 4; do
						config_get keyn $vif key${idx}
						[ -n "$keyn" ] && iwpriv $ifname set "Key${idx}=${keyn}"
					done
					iwpriv $ifname set DefaultKeyID=${key}
					iwpriv $ifname set "SSID=${ssid}"
					echo 
					#iwpriv $ifname set WscConfMode=0
					;;
				none|open)
					echo "NONE" >>/tmp/wifi_encryption_${ifname}.dat
					iwpriv $ifname set AuthMode=OPEN
					iwpriv $ifname set EncrypType=NONE
					#iwpriv $ifname set WscConfMode=0
					;;
			esac
                        iwpriv $ifname set HideSSID="$hidessid"
						
						
		config_get ApCliEnable "$vif" ApCliEnable 0
		config_get ApCliSsid "$vif" ApCliSsid
		config_get ApCliBssid "$vif" ApCliBssid 0
		config_get ApCliAuthMode "$vif" ApCliAuthMode
		config_get ApCliEncrypType "$vif" ApCliEncrypType
		config_get ApCliPassWord "$vif" ApCliPassWord
		ifconfig apcli0 down
		[ "$ApCliEnable" != "0" ] && {
		ifconfig apcli0 up
		
		iwpriv ra0 set SiteSurvey=1 

		sleep 1
			
		apcli_channel=0
		now_rssi=0
		tmp_rssi=0
		OUTPUT=`iwpriv ra0 get_site_survey | grep '^[0-9]' | fgrep "$ApCliSsid"`
		while read line
		do
			tmp_ssid2=`echo "${line:4:33}" | sed 's/[[:space:]]*$//'`
			tmp_rssi=`echo "${line:80:3}" | sed 's/[[:space:]]*$//'`
			if [ "$ApCliSsid"x = "$tmp_ssid2"x ]; then
				if [ $tmp_rssi -gt $now_rssi ]; then
				apcli_channel=`echo $line | awk '{print $1}'`	
				now_rssi=$tmp_rssi			
				fi
			fi
		done <<EOF
$OUTPUT
EOF
		if [ "$apcli_channel" -gt 0 ]; then
			channel="$apcli_channel"
		fi

		
		echo "setting apcli"

		iwpriv apcli0 set ApCliEnable=0

		case "$ApCliAuthMode" in
			none*|NONE*)
				iwpriv apcli0 set ApCliAuthMode=OPEN 
				iwpriv apcli0 set ApCliEncrypType=NONE 
				;;
			WEP|wep|*wep*|*WEP*)
				iwpriv apcli0 set ApCliAuthMode=OPEN
				iwpriv apcli0 set ApCliEncrypType=WEP
				iwpriv apcli0 set ApCliDefaultKeyID=1
				iwpriv apcli0 set ApCliKey1="$ApCliPassWord"
				;;
			WPA*|wpa*)
				iwpriv apcli0 set ApCliAuthMode="$ApCliAuthMode"
				iwpriv apcli0 set ApCliEncrypType="$ApCliEncrypType"
				iwpriv apcli0 set ApCliWPAPSK="$ApCliPassWord"
				;;
		esac

		iwpriv apcli0 set ApCliSsid="$ApCliSsid"

		[ "$ApCliBssid" != "0" ] && {
		iwpriv apcli0 set ApCliBssid="$ApCliBssid"
		echo "APCli use bssid connect."
		}

		}
		
		iwpriv apcli0 set ApCliEnable=$ApCliEnable						
		}
		
		if [ $disabled == 1 ]; then
		 iwpriv $ifname set RadioOn=0
		 set_wifi_down $ifname
		else
		 iwpriv $ifname set RadioOn=1
		fi
		
		[ $isolate == "1" ]&&{
			iwpriv $ifname set NoForwarding=1
		}
		
		[ $doth == "1" ]&&{
			iwpriv $ifname set IEEE80211H=1
		}	
		
		ifconfig "$ifname" up
		if [ "$mode" == "sta" ];then {
			net_cfg="$(find_net_config "$vif")"
			[ -z "$net_cfg" ] || {
					ralink_start_vif "$vif" "$ifname"

			}
		}
		else
		{
			local net_cfg bridge
			net_cfg="$(find_net_config "$vif")"
			[ -z "$net_cfg" ]||{
				bridge="$(bridge_interface "$net_cfg")"
				config_set "$vif" bridge "$bridge"
				ralink_start_vif "$vif" "$ifname"
				#Fix bridge problem
				[ -z `brctl show |grep $ifname` ] && {
				brctl addif $(bridge_interface "$net_cfg") $ifname
				}
				
			}



		}
		fi;
		
		set_wifi_up "$vif" "$ifname"
	done
	
	[ "$channel" != "auto" ] && iwpriv $device set Channel=$channel
	iwpriv $device set MaxStaNum=$maxassoc
}

#获取MAC地址
rt2860v2_get_mac() {
#	/lib/functions.sh
	factory_part=$(find_mtd_part $1)
	dd bs=1 skip=4 count=6 if=$factory_part 2>/dev/null | /usr/sbin/maccalc bin2mac	
}

detect_ralink() {
	local i=-1
	cd /sys/module/
	[ -d mt76x8 ] || return
	while grep -qs "^ *ra$((++i)):" /proc/net/dev; do
		config_get type ra${i} type
		[ "$type" = ralink ] && continue
		
	[ -f /etc/Wireless/RT2860/RT2860.dat ] || {
	mkdir -p /etc/Wireless/RT2860/ 2>/dev/null
	ln -s /tmp/RT2860.dat /etc/Wireless/RT2860/RT2860.dat 2>/dev/null
	}
	
	mach=$(cat /proc/cpuinfo | grep machine | awk '{ print $3}')

	if [ "$mach"x = "WRTnode2R"x ] ;then
		name="2R"
	else
		name="2P"
	fi
		cat <<EOF
config wifi-device  ra${i}
	option type     ralink
	option mode 	9
	option channel  auto
	option txpower 100
	option ht 	20
	option country US
	
# REMOVE THIS LINE TO ENABLE WIFI:
	option disabled 0	
	
config wifi-iface
	option device   ra${i}
	option network	lan
	option mode     ap
	option ssid     WRTnode${name}_${i#0}$(cat /sys/class/net/eth0/address|awk -F ":" '{print $5""$6 }'| tr a-z A-Z)
	option encryption psk2
	option key 12345678
	option ApCliEnable '1'
	option ApCliSsid 'aAP'
	option ApCliAuthMode 'WPA2PSK'
	option ApCliEncrypType 'AES'
	option ApCliPassWord '87654321'
	
EOF

	done
	
}


