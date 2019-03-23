#!/usr/bin/haserl
<?
  export PATH=/bin:/sbin:/usr/bin:/usr/sbin
  echo -e "Content-type: text/html\r\n\r\n"
?>

<html>
  <head>
    <link rel="shortcut icon" href="/assets/img/favicon_octonix.ico">
  </head>
  <body>
    <div align=center>
      <img src="/assets/img/logo_<? uci get microbe.webadmin.project ?>.png" width="256">
      <p><b>Uptime Device</b></p>
      <pre><? uptime ?></pre>
      <p><b>Routing Table</b></p>
      <table>
        <tr><td><pre><? route -n ?></pre></td></tr>
      </table>
      <p><b>Network Status</b></p>
      <table>
        <tr><td><pre><? ifconfig br-lan ?></pre></td></tr>
        <tr><td><pre><? ifconfig 3g-wan1 ?></pre></td></tr>
        <tr><td><pre><? ifconfig 3g-wan2 ?></pre></td></tr>
        <tr><td><pre><? ifconfig tun1 ?></pre></td></tr>
        <tr><td><pre><? ifconfig eth1 ?></pre></td></tr>
      </table>
      <p><b>Modem Signal</b></p>
      <? comgt -d /dev/ttyUSB6 sig 2>&1 | tr ':,' ' ' | awk '/Signal/ {print -113+$3*2,"dBm"}' ?>
      <p><b>Ping Quality</b></p>
      <table>
        <tr>
          <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
            <td>
              <input type=hidden name="action" value="ping">
              <input type="text" required name="sense" pattern="^[a-zA-Z0-9-_.]+$" value="<? uci get microbe.pinger.host ?>" placeholder="host or ip" size="25">
            </td>
            <td>
              <input type="radio" required checked="checked" name="iface" value="auto"> auto <br>
              <input type="radio" name="iface" value="3g-wan1"> 3g-wan1 <br>
              <input type="radio" name="iface" value="3g-wan2"> 3g-wan2 <br>
            </td>
            <td>
              <input type="submit" value="Run">
            </td>
          </form>
        </tr>
      </table>
      <p><b>Trace Route</b></p>
      <table>
        <tr>
          <form action="/cgi-bin/update.cgi" method="POST" enctype="multipart/form-data">
            <td>
              <input type=hidden name="action" value="trace">
              <input type="text" required name="sense" pattern="^[a-zA-Z0-9-_.]+$" value="<? uci get microbe.pinger.host ?>" placeholder="host or ip" size="25">
            </td>
            <td>
              <input type="radio" required checked="checked" name="iface" value="auto"> auto <br>
              <input type="radio" name="iface" value="3g-wan1"> 3g-wan1 <br>
              <input type="radio" name="iface" value="3g-wan2"> 3g-wan2 <br>
            </td>
            <td>
              <input type="submit" value="Run">
            </td>
          </form>
        </tr>
      </table>
      <br><br><br>
      <form action="/cgi-bin/index.cgi" method="POST" enctype="multipart/form-data">
        <input type="submit" value="Global Settings">
      </form>
    </div>
  </body>
</html>
