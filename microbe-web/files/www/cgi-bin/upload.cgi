#!/usr/bin/haserl --upload-limit=4096 --upload-target=/tmp/ --upload-dir=/tmp/
#
<?
export PATH=/bin:/sbin:/usr/bin:/usr/sbin
action=$FORM_action
upfile=$FORM_upfile
#
case $action in
  ca)
    echo "Content-type: text/html"
    echo
    echo "<html><body>"
    if [ -r $upfile ]; then
      fsize="$(wc -c $upfile | awk '{print $1}')"
      if [ $fsize -gt "500" ]; then
        echo "<center><br><br><h1>Error: file is so big</h1></center>"
      else
        if cp $upfile /root/z1 2>/dev/null; then
          ok=1
        else
          echo "<center><br><br><h1>Error: file not writing to flash</h1></center>"
        fi
      fi
    else
      echo "<center><br><br><h1>Error: file not found</h1></center>"
    fi
    if [ $ok ]; then
      echo "<br><br><br><br><br><center><h1>We try to upload...</h1></center>"
      echo "<script language=javascript>setTimeout('window.location=\"/cgi-bin/index.cgi\"',1000);</script>"
    fi
    echo "</body>"
    echo "</html>"
    rm $uploadfile
  ;;
esac
?>

