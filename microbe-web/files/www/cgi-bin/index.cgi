#!/usr/bin/haserl
<?
  export PATH=/bin:/sbin:/usr/bin:/usr/sbin
  echo -e "Content-type: text/html\r\n\r\n"
?>

<html>
  <body>
    <div align=center>
      <img src="/assets/img/logo_<? uci get microbe.webadmin.project ?>.png" width="256">
      <p><b>Device Name</b></p>
      <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="hostname">
        <input type="text" required name="sense" pattern="^[a-zA-Z0-9-_.]+$" value="<? uci get system.@system[0].hostname ?>" placeholder="DeviceName" size="25">
        <input type="submit" value="Save">
      </form>
      <p><b>Interface Password</b></p>
      <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="password">
        <input type="password" required name="sense" pattern="^[a-zA-Z0-9]+$" value="<? uci get microbe.webadmin.password ?>" placeholder="You3Pass5Word" size="25">
        <input type="submit" value="Save">
      </form>
      <p><b>IP Address</b></p>
      <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="ipaddr">
        <input type="text" required name="sense" pattern="\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}" value="<? uci get network.lan.ipaddr ?>" placeholder="192.168.1.1" size="25">
        <input type="submit" value="Save">
      </form>
      <p><b>IP Netmask</b></p>
      <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="netmask">
        <input type="text" required name="sense" pattern="\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}" value="<? uci get network.lan.netmask ?>" placeholder="255.255.255.0" size="25">
        <input type="submit" value="Save">
      </form>
      <p><b>SNMP Community</b></p>
      <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="community">
        <input type="text" required name="sense" pattern="^[a-zA-Z0-9-_.]+$" value="<? uci get snmpd.default.community ?>" placeholder="community" size="25">
        <input type="submit" value="Save">
      </form>
      <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="priority">
        <select size="3" required multiple name="sense">
          <option disabled>Current: <? uci get microbe.pinger.priority ?></option>
          <option value="3g-wan1,3g-wan2">Variant-1: 3g-wan1,3g-wan2</option>
          <option value="3g-wan2,3g-wan1">Variant-2: 3g-wan2,3g-wan1</option>
        </select>
        <input type="submit" value="Save">
      </form>
      <p><b>Checked Host</b></p>
      <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="checked">
        <input type="text" required name="sense" pattern="^[a-zA-Z0-9-.]+$" value="<? uci get microbe.pinger.checked ?>" placeholder="facebook.com" size="25">
        <input type="submit" value="Save">
      </form>   
      <p><b>OpenVPN Server</b></p>
      <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="remote">
        <input type="text" required name="sense" pattern="^[a-zA-Z0-9-.]+$" value="<? uci get openvpn.vpn1.remote ?>" placeholder="ovpn.server.net" size="25">
        <input type="submit" value="Save">
      </form>
      <p><b>Certificate authority</b></p>
      <form action="/cgi-bin/upload.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="ca">
        <input type="file" required name="upfile">
        <input type="submit" value="Upload">
      </form>
      <p><b>Local certificate</b></p>
      <form action="/cgi-bin/upload.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="cert">
        <input type="file" required name="upfile">
        <input type="submit" value="Upload">
      </form>
      <p><b>Local private key</b></p>
      <form action="/cgi-bin/upload.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="key">
        <input type="file" required name="upfile">
        <input type="submit" value="Upload">
      </form>
      <br>
      <p><font color="blue">All settings will be applied after rebooting the device !</p>
      <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
        <input type=hidden name="action" value="reboot">
        <!-- input type="submit" value="Reboot Device" -->
        <input type="submit" value="Reboot Device" onclick="return confirm('Do you want to reboot the device ?')">
      </form></p>
    </div>
  </body>
</html>
