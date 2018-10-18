#!/usr/bin/haserl
<?
  export PATH=/bin:/sbin:/usr/bin:/usr/sbin
  action=$FORM_action
  sense=$FORM_sense
  iface=$FORM_iface
  project=$(uci get microbe.webadmin.project)
  #
  echo "Content-type: text/html"
  echo
  echo "<html><body><div align=center>"
  echo "<img src=\"/assets/img/logo_${project}.png\" width=\"256\">"
  echo
  echo "Probe change ${action} to ${sense} on ${iface}" | logger -t microbe-web
  echo
  case $action in
    hostname)
      echo "<br><br><br><br><br><center><h1>We try to update...</h1></center>"
      uci set system.@system[0].hostname=${sense} && uci commit system
      echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',1000);</script>"
      ;;
    password)
      echo "<br><br><br><br><br><center><h1>We try to update...</h1></center>"
      uci set microbe.webadmin.password=${sense} && uci commit microbe
      echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',1000);</script>"
      ;;
    ipaddr)
      echo "<br><br><br><br><br><center><h1>We try to update...</h1></center>"
      uci set network.lan.ipaddr=${sense} && uci commit network
      echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',1000);</script>"
      ;;
    netmask)
      echo "<br><br><br><br><br><center><h1>We try to update...</h1></center>"
      uci set network.lan.netmask=${sense} && uci commit network
      echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',1000);</script>"
      ;;
    community)
      echo "<br><br><br><br><br><center><h1>We try to update...</h1></center>"
      uci set snmpd.default.community=${sense} && uci commit snmpd
      echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',1000);</script>"
      ;;
    priority)
      echo "<br><br><br><br><br><center><h1>We try to update...</h1></center>"
      uci set microbe.pinger.priority=${sense} && uci commit microbe
      echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',1000);</script>"
      ;;
    checked)
      echo "<br><br><br><br><br><center><h1>We try to update...</h1></center>"
      uci set microbe.pinger.checked=${sense} && uci commit microbe
      echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',1000);</script>"
      ;;
    remote)
      echo "<br><br><br><br><br><center><h1>We try to update...</h1></center>"
      uci set openvpn.vpn1.remote=${sense} && uci commit openvpn
      echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',1000);</script>"
      ;;
    reboot)
      echo "<br><br><br><br><br><center><h1>We try to reboot...</h1><p>Please wait 60 sec.</p></center>"
      echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',60000);</script>"
      reboot
      ;;
    trace)
      echo "<p><b>Trace Route</b></p>"
      echo "<table><tr><td><pre>"
      if [ ${iface} = "auto" ]; then
        traceroute ${sense}
      else
        traceroute -i ${iface} ${sense}
      fi
      echo "</pre><td><tr><table>"
      echo "<p><form action=\"/cgi-bin/monitor.cgi\" method=\"POST\" enctype=\"multipart/form-data\"><input type=\"submit\" value=\"Monitor Tool\"></form>"
      ;;
    ping)
      echo "<p><b>Ping Quality</b></p>"
      echo "<table><tr><td><pre>"
      if [ ${iface} = "auto" ]; then
        ping -c 15 -s 1500 ${sense}
      else
        ping -c 15 -s 1500 -I ${iface} ${sense}
      fi
      echo "</pre><td><tr><table>"
      echo "<p><form action=\"/cgi-bin/monitor.cgi\" method=\"POST\" enctype=\"multipart/form-data\"><input type=\"submit\" value=\"Monitor Tool\"></form>"
      ;;
  esac
  echo
  echo "</div></body></html>"
?>
