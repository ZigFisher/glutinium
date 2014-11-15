#!/bin/sh

confirmed="${FORM_confirmed}"

case "$FORM_do" in
	flash_save)
		if [ $REQUEST_METHOD = POST -a "x${confirmed}x" = x1x ]; then
			echo "<h3 align=center>Saving...</h3>"
			echo "<pre>"
			/sbin/flash save
			echo "</pre>"
		else
			echo "<div align=center><h3>Do you really want to save?</h3>"
			echo "<form action='$SCRIPT_NAME' method='POST'>"
			echo "<input type=hidden name='confirmed' value='1'>"
			echo "<input type=hidden name='do' value='flash_save'>"
			echo '<input type=hidden name="page" value="'$page'">'
			echo "<input type=submit value='Yes'>"
			echo "<input type=reset value='No'>"
			echo "</form></div>"
		fi
		;;
	backup_startup)
		echo "<div align=center><form action='/cgi-bin/config.cgi' method='POST'>"
		echo "<input type=hidden name='action' value='backup_startup'>"
		echo "<input type=submit value='Backup'>"
		echo "</form></div>"
		;;
	backup_running)
		echo "<div align=center><form action='/cgi-bin/config.cgi' method='POST'>"
		echo "<input type=hidden name='action' value='backup_running'>"
		echo "<input type=submit value='Backup'>"
		echo "</form></div>"
		;;
	restore)
		echo "<div align=center><form action='/cgi-bin/config.cgi' method='POST' enctype='multipart/form-data'>"
		echo "<input type=hidden name='action' value='restore'>"
		echo "<input type=file name=uploadfile>"
		echo "<input type=submit value='Restore'>"
		echo "</form></div>"
		;;
esac

