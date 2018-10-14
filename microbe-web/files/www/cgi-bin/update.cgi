#!/usr/bin/haserl
<?
  export PATH=/bin:/sbin:/usr/bin:/usr/sbin
  action=$FORM_action
  sense=$FORM_sense
  #
  echo "Content-type: text/html"
  echo
  echo "<html><body>"
  echo
  echo "Probe change ${action} to ${sense}" | logger -t microbe-web
  echo
  case $action in
    hostname)
      uci set system.@system[0].hostname=${sense} && uci commit system
      ;;
    password)
      uci set microbe.webadmin.password=${sense} && uci commit microbe
      ;;
    ipaddr)
      uci set network.lan.ipaddr=${sense} && uci commit network
      ;;
    netmask)
      uci set network.lan.netmask=${sense} && uci commit network
      ;;
    priority)
      uci set microbe.pinger.priority=${sense} && uci commit microbe
      ;;
    checked)
      uci set microbe.pinger.checked=${sense} && uci commit microbe
      ;;
    remote)
      uci set openvpn.vpn1.remote=${sense} && uci commit openvpn
      ;;
  esac
  echo
  echo "<br><br><br><br><br><center><h1>We try to update...</h1></center>"
  echo
  echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',1000);</script>"
  echo
  echo "</body>"
  echo "</html>"
?>
