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
      <? echo "Not Found" ?>
      <p><b>Ping Quality</b></p>
      <table>
        <tr><td><pre><? ping -c 3 -W 1 $(uci get microbe.pinger.checked) ?></pre></td></tr>
      </table>
      <p><b>Network Status</b></p>
      <table>
        <tr><td><pre><? ifconfig br-lan ?></pre></td></tr>
        <tr><td><pre><? ifconfig 3g-wan1 ?></pre></td></tr>
        <tr><td><pre><? ifconfig 3g-wan2 ?></pre></td></tr>
        <tr><td><pre><? ifconfig tun1 ?></pre></td></tr>
      </table>
    </div>
    <script language=javascript>setTimeout('window.location=\"/cgi-bin/monitor.cgi\"',10000);</script>
  </body>
</html>
