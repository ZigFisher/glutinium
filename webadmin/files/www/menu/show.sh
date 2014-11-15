#!/bin/sh

menu() { echo "<li> $@"; echo "<ul>"; }
endmenu() { echo "</ul>"; }

item() { echo "  <li><a href=\"#\" onclick=\"runcmd('$2')\">$1</a>"; };

echo '<ul id="treemenu1" class="treeview">'

menu "system"

menu "log"
item "main log" "/sbin/logread"
item "kernel log" "/bin/dmesg"
endmenu
item "date" "/bin/date"
item "df" "/bin/df -h"
item "free" "/usr/bin/free"
item "ipkg list" "/usr/bin/ipkg list"
item "ipkg list_installed" "/usr/bin/ipkg list_installed"
item "lsmod" "/sbin/lsmod"
item "mount" "/bin/mount"
item "ps" "/bin/ps"
item "sysctl" "/sbin/sysctl -a"
item "uname -a" "/bin/uname -a"
endmenu

menu "network"

item "switch" "cat /proc/sys/net/adm5120sw/status"
item "active interfaces" "/sbin/ifconfig"
item "all interfaces" "/sbin/ifconfig -a"
item "connection tracking" "cat /proc/net/ip_conntrack"
item "interface usage" "/usr/bin/bwm --one"
menu "fw"

i="/usr/sbin/iptables -nvxL"
menu "filter"
item "all" "$i"
item "INPUT" "$i INPUT"
item "OUTPUT" "$i OUTPUT"
item "FORWARD" "$i FORWARD"
item "ACCOUNT-SRC" "$i ACCOUNT-SRC"
item "ACCOUNT-DST" "$i ACCOUNT-DST"
endmenu

i="/usr/sbin/iptables -t nat -nvxL"
menu "nat"
item "all" "$i"
item "PREROUTING" "$i PREROUTING"
item "POSTROUTING" "$i POSTROUTING"
item "OUTPUT" "$i OUTPUT"
endmenu

i="/usr/sbin/iptables -t mangle -nvxL"
menu "mangle"
item "all" "$i"
item "PREROUTING" "$i PREROUTING"
item "INPUT" "$i INPUT"
item "FORWARD" "$i FORWARD"
item "OUTPUT" "$i OUTPUT"
item "POSTROUTING" "$i POSTROUTING"
endmenu

endmenu

menu "ip"
item "addr" "/bin/ip addr"
item "link" "/bin/ip -s link"
item "route" "/bin/ip route"
item "rule" "/bin/ip rule"
item "neigh" "/bin/ip -s neigh"
item "tunnel" "/bin/ip -s tunnel"
item "maddr" "/bin/ip -s maddr"
item "mroute" "/bin/ip -s mroute"
endmenu

menu "netstat"
item "all sockets" "/bin/netstat -an"
item "listening sockets" "/bin/netstat -ln"
item "routing" "/bin/netstat -rn"
endmenu

menu "bridge"
item "show" "/usr/sbin/brctl show"
endmenu

endmenu

menu "software"

if [ -d /etc/bluetooth ]; then
	menu "bluepoint"
	item "devices" "$basedir/cmd/bluepoint_devices"
	item "log" "$basedir/cmd/bluepoint_log"
	endmenu
fi

if [ -x /usr/bin/mpcs ]; then
	menu "mpcs"
	item "status" "$basedir/cmd/mpcs_status"
	item "log" "$basedir/cmd/mpcs_log"
	endmenu
fi


endmenu

echo "</ul>"

