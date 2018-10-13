#!/usr/bin/haserl
<?
  export PATH=/bin:/sbin:/usr/bin:/usr/sbin
  echo -e "Content-type: text/html\r\n\r\n"
?>

<html>
  <body>
    <div align=center>
      <img src="/assets/img/logo_mixnet.png" width="256">
      <p><b>Device Name:</b>
      <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="hostname">
        <input type="text" name="sense" value="<? uci get system.@system[0].hostname ?>" placeholder="DeviceName" size="20">
        <input type="submit" value="Save">
      </form>
      <p><b>IP Address:</b>
      <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="ipaddr">
        <input type="text" name="sense" value="<? uci get network.lan.ipaddr ?>" placeholder="192.168.1.1" size="20">
        <input type="submit" value="Save">
      </form>
      <p><b>IP Netmask:</b>
      <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="netmask">
        <input type="text" name="sense" value="<? uci get network.lan.netmask ?>" placeholder="255.255.255.0" size="20">
        <input type="submit" value="Save">
      </form>
      <p><b>Modem Priority:</b>
      <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="priority">
        <input list="modem" name="sense" value="<? uci get microbe.webadmin.priority ?>" placeholder="3g-wanX" size="20"><datalist id="modem"><option>3g-wan1</option><option>3g-wan2</option></datalist>
        <input type="submit" value="Save">
      </form>
    </div>
  </body>
</html>
