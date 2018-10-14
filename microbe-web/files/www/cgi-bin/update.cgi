#!/usr/bin/haserl
<?
  export PATH=/bin:/sbin:/usr/bin:/usr/sbin
  action=$FORM_action
  sense=$FORM_sense
  #
  echo "Content-type: text/html"
  echo "<html><body>"
  echo "Probe change ${action} to ${sense}" | logger -t microbe-web
  case $action in
    hostname)
      uci set system.@system[0].hostname=${sense} && uci commit system && ok=1
      ;;
    password)
      uci set microbe.webadmin.password=${sense} && uci commit microbe && ok=1
      ;;
    ipaddr)
      uci set network.lan.ipaddr=${sense} && uci commit network && ok=1
      ;;
    netmask)
      uci set network.lan.netmask=${sense} && uci commit network && ok=1
      ;;
    priority)
      uci set microbe.pinger.priority=${sense} && uci commit microbe && ok=1
      ;;
    checked)
      uci set microbe.pinger.checked=${sense} && uci commit microbe && ok=1
      ;;
    remote)
      uci set openvpn.vpn1.remote=${sense} && uci commit openvpn && ok=1
      ;;
  esac
  if [ $ok ]; then
    echo "<br><br><br><br><br><center><h1>We try to update...</h1></center>"
  fi
  echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',1000);</script>"
  echo "</body>"
  echo "</html>"
?>
