#!/bin/sh

menu() { echo "<li> $@"; echo "<ul>"; }
endmenu() { echo "</ul>"; }

echo '<ul id="treemenu1" class="treeview">'

menu "ssh"
echo "<li><a href=\"?page=$page&do=ssh_ext\">Java SSH</a>";
echo "<li><a href=\"http://kitty.9bis.com/\">Download KiTTy</a>"; 
echo "<li><a href=\"http://the.earth.li/~sgtatham/putty/latest/x86/putty.exe\">Download PuTTY</a>";
endmenu
menu "help"
echo "<li><a href=\"http://flyrouter.net/flyrouter:news\">News</a>"; 
echo "<li><a href=\"http://flyrouter.net/flyrouter:help\">FAQ</a>"; 
endmenu
menu "links"
echo "<li><a href=\"http://flyrouter.net/downloads/software/flyrouter/packages/\">Available packages</a>"; 
echo "<li><a href=\"http://flyrouter.net/downloads/software/flyrouter/upgrade/\">Available firmware</a>"; 
endmenu

echo "</ul>"

