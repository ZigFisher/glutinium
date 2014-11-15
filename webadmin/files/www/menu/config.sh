#!/bin/sh

menu() { echo "<li> $@"; echo "<ul>"; }
endmenu() { echo "</ul>"; }

item() { echo "  <li><a href='?page=config&do=$2'>$1</a>"; };

echo '<ul id="treemenu1" class="treeview">'

menu "config"
item "flash save" "flash_save"
item "backup startup" "backup_startup"
item "backup running" "backup_running"
item "restore" "restore"
endmenu

echo "</ul>"

