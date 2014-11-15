#!/bin/sh

menu() { echo "<li> $@"; echo "<ul>"; }
endmenu() { echo "</ul>"; }

item() { echo "  <li><a href=\"javascript:cmdset('$2')\">$1</a>"; };

echo '<ul id="treemenu1" class="treeview">'

menu "system"
menu "ipkg"
item "ipkg update" "/usr/bin/ipkg update"
item "ipkg list" "/usr/bin/ipkg list"
item "ipkg install" "/usr/bin/ipkg install PACKAGE"
endmenu
item "flash save" "/sbin/flash save"
item "helpdesk" "/sbin/helpdesk"
item "nslookup" "/usr/bin/nslookup "
item "reboot" "/sbin/reboot"
item "upgrade" "/sbin/upgrade URL"
endmenu

menu "network"
item "arping" "/usr/bin/arping -I eth0 -c 3 127.0.0.1"
item "etherdump" "/usr/bin/etherdump --time 3 -i eth0"
item "ping" "/bin/ping -c 3 -s 100 127.0.0.1"
item "traceroute" "/usr/bin/traceroute -n 127.0.0.1"
[ -x /usr/sbin/tcpdump ] && item "tcpdump" "/usr/sbin/tcpdump -i eth0 -n            & pid=\${!}; sleep 5; kill \$pid "
ifaces=`cat /etc/network/interfaces | grep ^iface | awk '{print $2}'`

menu "ifaces"
item "test: ifup -an" "/sbin/ifup -a -n"
for i in $ifaces; do 
	menu "$i"
	item "ifup $i" "/sbin/ifup -v $i"
	item "ifdown $i" "/sbin/ifdown -v $i"
	item "restart $i" "/sbin/ifdown -v $i; /sbin/ifup -v $i"
	endmenu
done
endmenu

menu "fw"
item "restart fw" "/etc/network/fw"
menu "iptables"
item "iptables -nvxL" "/usr/sbin/iptables -nvxL"
item "disable fw" "/usr/sbin/iptables -P INPUT ACCEPT; /usr/sbin/iptables -P FORWARD ACCEPT; /usr/sbin/iptables -F; /usr/sbin/iptables -X"
endmenu

endmenu

menu "bridge"
item "brctl addbr" "/usr/sbin/brctl addbr BRIDGE"
item "brctl addif" "/usr/sbin/brctl addif BRIDGE DEV"
item "brctl showmacs" "/usr/sbin/brctl showmacs BRIDGE"
item "brctl delif" "/usr/sbin/brctl delif BRIDGE DEV"
item "brctl delbr" "/usr/sbin/brctl delbr BRIDGE"
endmenu
menu "ip"
menu "addr"
item "add" "/bin/ip addr add IPADDR/MASK dev IFACE"
item "del" "/bin/ip addr del IPADDR/MASK dev IFACE"
item "list" "/bin/ip addr list"
endmenu
menu "link"
item "list" "/bin/ip link list"
item "set up" "/bin/ip link set DEV up"
item "set down" "/bin/ip link set DEV down"
item "set mtu" "/bin/ip link set DEV mtu 1400"
endmenu
menu "route"
item "add gw" "/bin/ip route add NET/MASK via GW"
item "add dev" "/bin/ip route add NET/MASK dev DEV"
item "list" "/bin/ip route list"
item "list table" "/bin/ip route list table TABLE"
item "del" "/bin/ip route del NET/MASK"
endmenu
menu "rule"
item "add from" "/bin/ip rule add from NET/MASK table TABLE"
item "add dev" "/bin/ip rule add dev DEV table TABLE"
item "list" "/bin/ip rule list"
endmenu
menu "neigh"
item "flush" "/bin/ip neigh flush dev DEV"
item "list" "/bin/ip neigh list"
endmenu
endmenu

menu "tc"
item "action" "/usr/sbin/tc -s action"
item "class" "/usr/sbin/tc -s class"
item "filter" "/usr/sbin/tc -s filter"
item "qdisc" "/usr/sbin/tc -s qdisc"
endmenu

endmenu

menu "software"

if [ -d /etc/bluetooth ]; then
	menu "bluepoint"
	#item "devices" "bluepoint_devices"
	#item "log" "bluepoint_log"
	endmenu
fi

if [ -x /usr/bin/mpcs ]; then
	menu "mpcs"
	#item "status" "mpcs_status"
	#item "log" "mpcs_log"
	endmenu
fi


endmenu

echo "</ul>"

