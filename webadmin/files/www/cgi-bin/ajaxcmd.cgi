#!/usr/bin/haserl
<?
cd /root
cmd="$FORM_cmd"
export PATH=/bin:/sbin:/usr/bin:/usr/sbin
hostname=`uci get system.@system[0].hostname`
echo -n "[root@$hostname $PWD]# "
if [ -n "$cmd" -a "$cmd" != "undefined" ]; then
  echo "$cmd"
  eval "$cmd"
fi
?>
