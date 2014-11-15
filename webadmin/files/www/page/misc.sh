#!/bin/sh

ssh_html1="<object codetype='application/java-archive' classid='java:com.mindbright.application.MindTerm.class' archive='http://flyrouter.net/misc/mindterm.jar'>"
ssh_html2="<param name='protocol' value='ssh2' />
<param name='debug' value='true' />
<param name='server' value='`echo $HTTP_REFERER | cut -f3 -d/`' />
<param name='username' value='root' />
<param name='port' value='22' />
<param name='alive' value='60' />
<param name='80x132-enable' value='true' />
<param name='80x132-toggle' value='true' />
<param name='bg-color' value='black' />
<param name='fg-color' value='white' />
<param name='cursor-color' value='i_green' />
<param name='save-lines' value='1000' />
<param name='geometry' value='132x35' />
<param name='encoding' value='utf-8' />
<param name='copy-select' value='true' />
</object>
"

case "$FORM_do" in
	ssh_*)
	echo "<p><b>Required:</b> Internet access and <a href='http://www.google.com.ua/search?q=java+plug-in'>Java plugin</a></p>"
		echo "$ssh_html1"
		echo "<param name='sepframe' value='true' />"
		echo "$ssh_html2"
		;;
esac


