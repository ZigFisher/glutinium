#!/usr/bin/haserl
<?
  export PATH=/bin:/sbin:/usr/bin:/usr/sbin
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
  <head>
    <meta http-equiv="pragma" content="no-cache"/>
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />
    <link rel="stylesheet" href="/assets/css/base.css" type="text/css"/>
    <link rel="shortcut icon" type="image/vnd.microsoft.icon" href="/assets/img/favicon.ico" />
    <script src="/assets/js/jquery-1.7.2.min.js" type="text/javascript"></script>
    <title>Octonix test</title>
  </head>

  <body>

    <div class="b-base-logo"><img src="/assets/img/octonix.jpg" width="199" height="180" alt="Octonix"/></div>


<table border="0" cellpadding="0" cellspacing="0" align="center">

  <tr><td>
    <fieldset class="b-fieldset-inline">
      <legend>Base settings</legend>
      <br/>
      <form name="personalization" id="PersonalizationForm" method="post" action="/cgi-bin/index.cgi">
        <table border="0" cellpadding="0" cellspacing="0" align="center">
          <tr>
            <td>
              <table border="1" bordercolordark="#ffffff" bordercolorlight="#c0c0c0" cellpadding="0" cellspacing="0" align="center">
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Device name&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder=" <? uci get system.@system[0].hostname ?>" name="system.@system[0].hostname" id="system.@system[0].hostname" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
              </table>
            </td>
          </tr>
          <tr>
            <td align="center" height="60">
              <input type="submit" class="butt" name="apply" id="PersonalizationForm_apply" value=" Apply "/>
            </td>
          </tr>
        </table>
      </form>
    </fieldset>
  </td></tr>

  <tr><td><br/></tr></td>

  <tr><td>
    <fieldset class="b-fieldset-inline">
      <legend>Network settings</legend>
      <br/>
      <form name="personalization" id="PersonalizationForm" method="post" action="/cgi-bin/index.cgi">
        <table border="0" cellpadding="0" cellspacing="0" align="center">
          <tr>
            <td>
              <table border="1" bordercolordark="#ffffff" bordercolorlight="#c0c0c0" cellpadding="0" cellspacing="0" align="center">
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;IP address&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder=" <? uci get network.lan.ipaddr ?>" name="network.lan.ipaddr" id="network.lan.ipaddr" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Netmask&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder=" <? uci get network.lan.netmask ?>" name="network.lan.netmask" id="network.lan.netmask" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Gateway&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder=" <? uci get network.lan.gateway ?>" name="network.lan.gateway" id="network.lan.gateway" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;DNS servers&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder=" <? uci get network.lan.dns ?>" name="network.lan.dns" id="network.lan.dns" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
              </table>
            </td>
          </tr>
          <tr>
            <td align="center" height="60">
              <input type="submit" class="butt" name="apply" id="PersonalizationForm_apply" value=" Apply "/>
            </td>
          </tr>
        </table>
      </form>
      <table style="margin: auto; width: auto;">
        <tr>
          <td valign="top"><font class="text2"><em>Notice</em></font>&nbsp;: </td>
          <td>Аварийный адрес 192.168.254.254/255.255.250.0 всегда прописан в устройстве !</td>
        </tr>
      </table>
    </fieldset>
  </td></tr>

  <tr><td><br/></tr></td>

  <tr><td>
    <fieldset class="b-fieldset-inline">
      <legend>Wi-Fi settings (optional)</legend>
      <br/>
      <form name="personalization" id="PersonalizationForm" method="post" action="/cgi-bin/index.cgi">
        <table border="0" cellpadding="0" cellspacing="0" align="center">
          <tr>
            <td>
              <table border="1" bordercolordark="#ffffff" bordercolorlight="#c0c0c0" cellpadding="0" cellspacing="0" align="center">
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;SSID&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder=" <? uci get wireless.@wifi-iface[0].ssid ?>" name="wireless.@wifi-iface[0].ssid" id="wireless.@wifi-iface[0].ssid" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Passphrase&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder=" <? uci get wireless.@wifi-iface[0].key ?>" name="wireless.@wifi-iface[0].key" id="wireless.@wifi-iface[0].key" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
              </table>
            </td>
          </tr>
          <tr>
            <td align="center" height="60">
              <input type="submit" class="butt" name="apply" id="PersonalizationForm_apply" value=" Apply "/>
            </td>
          </tr>
        </table>
      </form>
      <table style="margin: auto; width: auto;">
        <tr>
          <td valign="top"><font class="text2"><em>Notice</em></font>&nbsp;: </td>
          <td>Поддерживаются только точки доступа в режиме WPA2-PSK TKIP/AES !</td>
        </tr>
      </table>
    </fieldset>
  </td></tr>

  <tr><td><br/></tr></td>

  <tr><td>
    <fieldset class="b-fieldset-inline">
      <legend>Telegram settings</legend>
      <br/>
      <form name="personalization" id="PersonalizationForm" method="post" action="/cgi-bin/index.cgi">
        <table border="0" cellpadding="0" cellspacing="0" align="center">
          <tr>
            <td>
              <table border="1" bordercolordark="#ffffff" bordercolorlight="#c0c0c0" cellpadding="0" cellspacing="0" align="center">
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Bot Token&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder=" <? uci get telegram.bot.token ?>" name="telegram.bot.token" id="telegram.bot.token" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Group ID&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder=" <? uci get telegram.bot.rupor ?>" name="telegram.bot.rupor" id="telegram.bot.rupor" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;IPCam URL&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder=" <? uci get telegram.bot.ipcam ?>" name="telegram.bot.ipcam" id="telegram.bot.ipcam" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Relay GPIO&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder=" <? uci get telegram.bot.relay ?>" name="telegram.bot.relay" id="telegram.bot.relay" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
              </table>
            </td>
          </tr>
          <tr>
            <td align="center" height="60">
              <input type="submit" class="butt" name="apply" id="PersonalizationForm_apply" value=" Apply "/>
            </td>
          </tr>
        </table>
      </form>
    </fieldset>
  </td></tr>

  <tr><td><br/></tr></td>


  <tr><td>
    <fieldset class="b-fieldset-inline">
      <legend>Security</legend>
      <br/>
      <form name="personalization" id="PersonalizationForm" method="post" action="/cgi-bin/index.cgi">
        <table border="0" cellpadding="0" cellspacing="0" align="center">
          <tr>
            <td>
              <table border="1" bordercolordark="#ffffff" bordercolorlight="#c0c0c0" cellpadding="0" cellspacing="0" align="center">
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Admin password&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder=" <? uci get telegram.bot.password ?> "name="telegram.bot.password" id="telegram.bot.password" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
              </table>
            </td>
          </tr>
          <tr>
            <td align="center" height="60">
              <input type="submit" class="butt" name="apply" id="PersonalizationForm_apply" value=" Apply "/>
            </td>
          </tr>
        </table>
      </form>
      <table style="margin: auto; width: auto;">
        <tr>
          <td valign="top"><font class="text2"><em>Notice</em></font>&nbsp;: </td>
          <td>Можно написать аннотацию и чешую в столбик:
            <ul>
              <li>Bitrate: 256 Kbit/sec.</li>
            </ul>
            Измени пароль, дурик !
          </td>
        </tr>
      </table>
    </fieldset>
  </td></tr>

</table>

</body>
</html>
