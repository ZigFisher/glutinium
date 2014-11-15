#!/bin/sh

cmd="$FORM_cmd"

echo "<div style='height:22px' align='right'>&nbsp; <img src='/images/loading.gif' alt='Loading...' id='loading' style='display:none' /></div>"
echo "<form onsubmit=\"return runcmd(\$F('ajaxcmd'));\" >"
echo "<span class='code'>[ root@$hostname ~] #</span> <input type=text id=\"ajaxcmd\" name=\"ajaxcmd\" size=45 value='$cmd'>"
echo "<input type=\"button\" value=\"run\" onclick=\"runcmd(\$F('ajaxcmd'));\" /></form>"


echo '<pre name="code" id="code" cols="110" rows="40">'
if [ "$REQUEST_METHOD = POST" -a -n "$cmd" ]; then
       [ -n "$FORM_params" ] && params="$FORM_params"
       echo "[root@$hostname ~]# $cmd"
       cd $HOME
       eval ${cmd}
fi
echo '</pre>'
echo "<script language=\"javascript\">setTimeout(\"cmdfocus()\",500);</script>"

