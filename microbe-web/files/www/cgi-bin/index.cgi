#!/usr/bin/haserl
<?
  export PATH=/bin:/sbin:/usr/bin:/usr/sbin
  #
  #IPC=/mnt/mtd/ipcam.conf
  #
  #if [ -f ${IPC} ]; then
  # while read settings
  #    do local ${settings}
  # done < ${IPC}
  #fi
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
  <head>
    <meta http-equiv="pragma" content="no-cache"/>
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />
    <link rel="stylesheet" href="/assets/css/base.css" type="text/css"/>
    <link rel="shortcut icon" type="image/png" href="/assets/img/favicon_<? uci get microbe.webadmin.project ?>.png" />
    <script src="/assets/js/jquery-1.7.2.min.js" type="text/javascript"></script>
    <title>Octonix test</title>
  </head>

  <body>

    <div class="b-base-logo"><img src="/assets/img/logo_<? uci get microbe.webadmin.project ?>.png" width="256" alt="<? uci get microbe.webadmin.project ?>"/></div>

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
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? uci get system.@system[0].hostname ?>" name="system.@system[0].hostname" id="system.@system[0].hostname" class="in" maxlength="40" size="30"/>&nbsp;</td>
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

  <tr><td><br/></td></tr>

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
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? uci get network.lan.ipaddr ?>" name="network.lan.ipaddr" id="network.lan.ipaddr" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Netmask&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? uci get network.lan.netmask ?>" name="network.lan.netmask" id="network.lan.netmask" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Gateway&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? uci get network.lan.gateway ?>" name="network.lan.gateway" id="network.lan.gateway" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;DNS servers&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? uci get network.lan.dns ?>" name="network.lan.dns" id="network.lan.dns" class="in" maxlength="40" size="30"/>&nbsp;</td>
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

  <tr><td><br/></td></tr>

  <tr><td>
    <fieldset class="b-fieldset-inline">
      <legend>OpenVPN settings</legend>
      <br/>
      <form name="personalization" id="PersonalizationForm" method="post" action="/cgi-bin/index.cgi">
        <table border="0" cellpadding="0" cellspacing="0" align="center">
          <tr>
            <td>
              <table border="1" bordercolordark="#ffffff" bordercolorlight="#c0c0c0" cellpadding="0" cellspacing="0" align="center">
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Remote server&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? uci get openvpn.vpn1.remote ?>" name="openvpn.vpn1.remote" id="openvpn.vpn1.remote" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Certificate authority&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? uci get openvpn.vpn1.ca ?>" name="openvpn.vpn1.ca" id="openvpn.vpn1.ca" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Local certificate&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? uci get openvpn.vpn1.cert ?>" name="openvpn.vpn1.cert" id="openvpn.vpn1.cert" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Local private key&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? uci get openvpn.vpn1.key ?>" name="openvpn.vpn1.key" id="openvpn.vpn1.key" class="in" maxlength="40" size="30"/>&nbsp;</td>
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

</table>

</body>
</html>
