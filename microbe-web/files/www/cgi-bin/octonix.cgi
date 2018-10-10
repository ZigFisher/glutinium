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
    <link rel="shortcut icon" type="image/vnd.microsoft.icon" href="/assets/img/favicon.ico" />
    <script src="/assets/js/jquery-1.7.2.min.js" type="text/javascript"></script>
    <title>Octonix test</title>
  </head>

  <body>

    <!-- div class="b-base-logo"><img src="/assets/img/octonix.jpg" width="199" height="180" alt="Octonix"/></div -->

    <div class="b-base-logo"><h1>MixNet experimental project</h1></div>

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
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? echo ${device_name} ?>" name="device_name" id="device_name" class="in" maxlength="40" size="30"/>&nbsp;</td>
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
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? ifconfig eth0 | tr ':' ' ' | awk '/Mask/ {print $3}' ?>" name="lan_ipaddr" id="lan_ipaddr" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Netmask&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? ifconfig eth0 | tr ':' ' ' | awk '/Mask/ {print $7}' ?>" name="lan_netmask" id="lan_netmask" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Gateway&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? ip r | awk '/default/ {print $3}' ?>" name="lan_gateway" id="lan_gateway" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;DNS servers&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? awk '{print $2}' /etc/resolv.conf | tr '\n' ' ' ?>" name="lan_dns" id="lan_dns" class="in" maxlength="40" size="30"/>&nbsp;</td>
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
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? echo ${wifi_ssid} ?>" name="wifi_ssid" id="wifi_ssid" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Certificate authority&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? echo ${wifi_ssid} ?>" name="wifi_ssid" id="wifi_ssid" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Local certificate&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? echo ${wifi_ssid} ?>" name="wifi_ssid" id="wifi_ssid" class="in" maxlength="40" size="30"/>&nbsp;</td>
                </tr>
                <tr>
                  <td bgcolor="#BAC4DD">&nbsp;Local private key&nbsp;</td>
                  <td bgcolor="#E1EBF0">&nbsp;<input type="text" placeholder="<? echo ${wifi_type} ?>" name="wifi_pass" id="wifi_pass" class="in" maxlength="40" size="30"/>&nbsp;</td>
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
