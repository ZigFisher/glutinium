#!/bin/sh
#
# Exec /system command

echo "Uptime:"
uptime
echo ""
echo "Memory:"
free | awk '/Mem/ {print "Total: "$2" |","Used: "$3" |","Free: "$4}'
