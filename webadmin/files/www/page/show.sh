#!/bin/sh

cmd="$FORM_cmd"
[ -n "$FORM_params" ] && params="$FORM_params"
echo "<div style='height:22px' align='right'>&nbsp; <img src='/images/loading.gif' alt='Loading...' id='loading' style='display:none' /></div>"
echo '<pre name="code" id="code" cols="110" rows="40">'
[ -n "${cmd}" ] && ${cmd}
echo '</pre>'

echo "<div class='code'>"
echo "Autorefresh: <input type='checkbox' id='autocmd' onclick=\"autocmd();\"'>"
echo "</div>"
