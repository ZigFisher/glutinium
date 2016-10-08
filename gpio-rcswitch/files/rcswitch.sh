#!/bin/sh

##
# Bash client for rcswitch-kmod
#
# Copyright (c) 2014 Stefan Wendler
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
##


SYS_KERNEL_RCSWITCH_COMMAND="/sys/kernel/rcswitch/command"

die()
{
	msg="$@"

	echo "ERROR: ${msg}"
	exit 1
}

switch_state()
{
	address=$1
	channel=$2
	state=$3

	test -f ${SYS_KERNEL_RCSWITCH_COMMAND} || die "${SYS_KERNEL_RCSWITCH_COMMAND} not found. Kernel module loaded?" 

	echo "${address}${channel}${state}" > ${SYS_KERNEL_RCSWITCH_COMMAND}
}

if [ "X$1" == "X-h" ]
then
	echo ""
	echo "Usage: $0 <address> <channel> <state>"
	echo "	address	- 5-Bit address - e.g. 11111"
	echo "	channel	- Channel A, B, C or D, alternatively 1, 2, 3 or 4"
	echo "	state	- 1 - ON, 0 - OFF"
	echo ""
	echo "Example:"
	echo "	$0 11111 A 1"
	echo ""
	exit 0
fi

address=$1
channel=$2
state=$3

test "X${address}" == "X" && die "First  parameter needs to be address (e. g. 11111)"
test "X${channel}" == "X" && die "Second parameter needs to be channel (e. g. A or 1)"
test "X${state}"   == "X" && die "Third  parameter needs to be state   (e. g. 1 or 0)"

switch_state ${address} ${channel} ${state}

