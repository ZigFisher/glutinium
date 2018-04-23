#!/bin/sh
#
# Exec /guard command

echo $(date +%s) >/tmp/guard.lock
echo "Guard alert timeout activated"
