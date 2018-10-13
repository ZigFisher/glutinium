#!/usr/bin/haserl --upload-limit=4096 --upload-target=/tmp/ --upload-dir=/tmp/
#
<?
export PATH=/bin:/sbin:/usr/bin:/usr/sbin
action=$FORM_action
uploadfile=$FORM_uploadfile
#
case $action in
  upload_ca)
    echo "Content-type: text/html"
    echo
    echo "<html><body bgcolor=black text=lightgray>"
    if [ -r $uploadfile ]; then
      fsize="$(wc -c $uploadfile | awk '{print $1}')"
      if [ $fsize -gt "500" ]; then
        echo "<center><br><br><h1>Error: file is so big</h1></center>"
      else
        if cp $uploadfile /root/z1 2>/dev/null; then
          ok=1
          echo "<center><br><br><h1>Done<br><br>Upload next file and reboot device to take effect</h1></center>"
        else
          echo "<center><br><br><h1>Error: file not writing to flash</h1></center>"
        fi
      fi
    else
      echo "<center><br><br><h1>Error: file not found</h1></center>"
    fi
    if [ $ok ]; then
      echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',1000);</script>"
    fi
    echo "</body>"
    echo "</html>"
    rm $uploadfile
  ;;
esac
?>

