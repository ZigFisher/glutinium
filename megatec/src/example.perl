#!/usr/bin/perl
#
# Supported commands: F, I, Q1
#
# Command Q1: InVolt FaultVolt OutVolt Current Freq UBatt UTemp Status
#
$port = "/dev/ttyUSB0";
system "stty -F $port 2400 ixon -echo";
open(COM, "+>$port") or die "Can't open $port";
select(COM);
$| = 1;
printf("Q1\r");
sleep(1);
sysread(COM, my ($line), 60);
printf(stderr $line);
close(COM);

