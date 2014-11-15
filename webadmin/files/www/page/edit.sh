#!/bin/sh

filename="${FORM_filename:-none}"
text="${FORM_text}"

. /bin/midge_functions

if [ $REQUEST_METHOD = POST -a -n "$filename" -a -n "$FORM_text" ]; then
	echo "<h3>Saving...</h3>"
	pre_edit_file ${filename}
	#[ -r "${filename}" ] && cp "${filename}" "${filename}.bak"
	echo -n "$FORM_text" | dos2unix >"$filename"
fi

if [ -n "$filename" -a "$filename" != "none" ]; then
	echo "<h3>Edit ${filename}</h3>"
	echo '<form action="'$SCRIPT_NAME'" method=POST>'
	echo '<input type=hidden name="filename" value="'$filename'">'
	echo '<input type=hidden name="page" value="'$page'">'
	echo '<textarea name="text" cols="90" rows="30" wrap="off" spellcheck="false">'
	[ -r "${filename}" ] && cat "${filename}"
	echo '</textarea>'
	echo '<input type=submit value=Save>'
	echo '</form>'
fi

