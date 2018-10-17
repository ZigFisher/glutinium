#!/usr/bin/haserl
<?
  export PATH=/bin:/sbin:/usr/bin:/usr/sbin
  echo -e "Content-type: text/html\r\n\r\n"
?>

<html>
  <body>
    <div align=center>
      <img src="/assets/img/logo_<? uci get microbe.webadmin.project ?>.png" width="256">
      <p><b>Uptime Device</b></p>
      <pre><? uptime ?></pre>
      <p><b>Modem Signal</b></p>
      <? comgt -d /dev/ttyUSB6 sig 2>&1 | tr ':,' ' ' | awk '/Signal/ {print -113+$3*2,"dBm"}' ?>
      <p><b>Ping Quality</b></p>
      <table>
        <tr><td><pre><? ping -c 5 -s 1500 -W 1 $(uci get microbe.pinger.checked) -I $(uci get microbe.pinger.priority) ?></pre></td></tr>
      </table>
      <p><b>Network Status</b></p>
      <table>
        <tr><td><pre><? ifconfig br-lan ?></pre></td></tr>
        <tr><td><pre><? ifconfig 3g-wan1 ?></pre></td></tr>
        <tr><td><pre><? ifconfig 3g-wan2 ?></pre></td></tr>
        <tr><td><pre><? ifconfig tun1 ?></pre></td></tr>
      </table>
    </div>
  </body>
</html>
