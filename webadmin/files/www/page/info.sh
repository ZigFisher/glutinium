#!/bin/sh

txt(){
  local str="$1"
  local str2="$2"
  echo -n "<tr><td><b>$str&nbsp;&nbsp;</b></td><td>$str2</td></tr>"
}

cmd(){
  local str="$1"
  local command="$2"
  echo -n "<tr><td><b>$str&nbsp;&nbsp;</b></td><td>"
  $command
  echo -n "</td></tr>"
}

echo "<pre>"
cat /etc/banner
echo "</pre>"

echo "<table>"
txt "Hostname:" "$hostname"
txt "Uptime:" "`uptime | awk -F ',' '{print $1}'`"
txt "Hardware:" "`cat /tmp/sysinfo/model`"
txt "CPU:" "`awk -F ':' '/system type/ {print $2,$3,$4,$5}' /proc/cpuinfo`"
txt "Load average:" "`uptime | awk -F ',' '{print $2","$3","$4}' | awk -F ':' '{print $2}'`"
txt "Total memory:" "`cat /proc/meminfo | grep MemTotal | awk '{print $2 " " $3}'`"
txt "Free memory:" "`cat /proc/meminfo | grep MemFree | awk '{print $2 " " $3}'`"
txt "Cached memory:" "`cat /proc/meminfo | grep ^Cached | awk '{print $2 " " $3}'`"
txt "Buffers:" "$(awk '/Buffers/ {print $2,$3}' /proc/meminfo)"

#[ -x /sbin/cpu ] && cmd "CPU usage:" /sbin/cpu

echo "</table>"

echo "<script language=javascript>if (isIE) setTimeout('alert(\"Microsoft IE - SUXX, please use right browser\");window.location=\"http://www.google.com.ua/search?q=firefox\"',7777);</script>"
