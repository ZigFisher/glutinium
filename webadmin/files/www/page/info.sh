#!/bin/sh

txt(){
	local str="$1"
	local str2="$2"
	echo -n "<tr><td><b>$str</b></td><td>$str2</td></tr>"
}

cmd(){
	local str="$1"
	local command="$2"
	echo -n "<tr><td><b>$str</b></td><td>"
	$command
	echo -n "</td></tr>"
}
echo "<pre>"
cat /etc/banner
echo "</pre>"

echo "<table>"
txt "Hostname:" "$hostname"
cpu=`cat /proc/cpuinfo | grep "system type" | cut -d: -f2`
txt "CPU:" "$cpu"
cmd "Uptime:" uptime
[ -x /sbin/cpu ] && cmd "CPU usage:" /sbin/cpu
mem_free=`cat /proc/meminfo | grep MemFree | awk '{print $2 " " $3}'`
mem_total=`cat /proc/meminfo | grep MemTotal | awk '{print $2 " " $3}'`
mem_cached=`cat /proc/meminfo | grep ^Cached | awk '{print $2 " " $3}'`
buffers=`cat /proc/meminfo | grep Buffers | awk '{print $2 " " $3}'`
txt "Total memory:" "$mem_total"
txt "Free memory:" "$mem_free"
txt "Cached memory:" "$mem_cached"
txt "Buffers:" "$buffers"

echo "</table>"

echo "<script language=javascript>if (isIE) setTimeout('alert(\"Microsoft IE - SUXX, please use right browser\");window.location=\"http://www.google.com.ua/search?q=firefox\"',7777);</script>"
