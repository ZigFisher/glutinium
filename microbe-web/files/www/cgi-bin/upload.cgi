#!/usr/bin/haserl --upload-limit=4096 --upload-target=/tmp/ --upload-dir=/tmp/
<?
  export PATH=/bin:/sbin:/usr/bin:/usr/sbin
  action=$FORM_action
  upfile=$FORM_upfile
  #
  echo "Content-type: text/html"
  echo
  echo "<html><body>"
  echo
  echo "Probe write ${action} file" | logger -t microbe-web
  echo
  case $action in
    ca)
      if [ -r $upfile ]; then
        fsize="$(wc -c $upfile | awk '{print $1}')"
        if [ $fsize -gt "1500" ]; then
          echo "<br><br><br><br><br><center><h1><font color="red">Error: file is so big !<font></h1></center>"
        else
          if cp $upfile /etc/openvpn/ca.crt 2>/dev/null; then
            ok=1
            rm $upfile
          else
            echo "<br><br><br><br><br><center><h1><font color="red">Error: file not writing to flash !<font></h1></center>"
          fi
        fi
      else
        echo "<br><br><br><br><br><center><h1><font color="red">Error: file not found !<font></h1></center>"
      fi
      if [ $ok ]; then
        echo "<br><br><br><br><br><center><h1>We try to upload...</h1></center>"
      fi
    ;;
    cert)
      if [ -r $upfile ]; then
        fsize="$(wc -c $upfile | awk '{print $1}')"
        if [ $fsize -gt "5000" ]; then
          echo "<br><br><br><br><br><center><h1><font color="red">Error: file is so big !<font></h1></center>"
        else
          if cp $upfile /etc/openvpn/cert.crt 2>/dev/null; then
            ok=1
            rm $upfile
          else
            echo "<br><br><br><br><br><center><h1><font color="red">Error: file not writing to flash !<font></h1></center>"
          fi
        fi
      else
        echo "<br><br><br><br><br><center><h1><font color="red">Error: file not found !<font></h1></center>"
      fi
      if [ $ok ]; then
        echo "<br><br><br><br><br><center><h1>We try to upload...</h1></center>"
      fi
    ;;
    key)
      if [ -r $upfile ]; then
        fsize="$(wc -c $upfile | awk '{print $1}')"
        if [ $fsize -gt "1200" ]; then
          echo "<br><br><br><br><br><center><h1><font color="red">Error: file is so big !<font></h1></center>"
        else
          if cp $upfile /etc/openvpn/cert.key 2>/dev/null; then
            ok=1
            rm $upfile
          else
            echo "<br><br><br><br><br><center><h1><font color="red">Error: file not writing to flash !<font></h1></center>"
          fi
        fi
      else
        echo "<br><br><br><br><br><center><h1><font color="red">Error: file not found !<font></h1></center>"
      fi
      if [ $ok ]; then
        echo "<br><br><br><br><br><center><h1>We try to upload...</h1></center>"
      fi
    ;;
  esac
  echo
  echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',1000);</script>"
  echo
  echo "</body>"
  echo "</html>"
?>
