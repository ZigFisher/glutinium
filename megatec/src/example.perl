#!/usr/bin/perl
#
# Supported commands: F, I, Q1
#
# InVolt FaultVolt OutVolt Current Freq UBatt UTemp NA
#
$port = "/dev/ttyUSB0";
system "stty 2400 ixon -echo < $port";
open(COM, "+>$port") or die "Can't open $port";
select(COM);
$| = 1;
#
printf("Q1\r");
sleep(1);
sysread(COM, my ($line), 50);
printf(stderr $line);
#
close(COM);

