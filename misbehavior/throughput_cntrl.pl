#!/bin/perl
#Program to per node and system Throughput.

use strict;
use warnings;

#Input trace file
my($infile) =$ARGV[0];

#Keep track of variables
my(@x);
my($nodes) = $ARGV[1];
my($misbhnodes) = $ARGV[2];
my($packet_size) = 512;
my($total_tx_count) = 0;
my($total_rx_count) = 0;
my($simulation_time) = 0;
my($system_throughput) = 0;
my($system_pdr) = 0;
my(%hash_table_send) = ();
my(%hash_table_receive) = ();
my($hash_key) = 0;
my($hash_value) = 0;
my($sip) = 0;
my($node_send_count) = 0;
my($node_receive_count) = 0;
my($node_throughput) = 0;
my($result) = 0;
my($sum) = 0;
my($sum_misbh) = 0;
my($sqr_sum) = 0;
my($ratio) = 0;
my($fairness) = 0;


open(DATA,"$infile" ) || die "could't open $infile$!";
while(<DATA>)
{
	@x=split(' ');
	if((defined($x[44])) && ($x[44] eq 'cbr') && ($x[18] eq 'MAC') && ($x[0] eq 's')) {
		$hash_key = $x[30];
		$hash_value = $hash_table_send{"$hash_key"};
		$hash_value++;
		$hash_table_send{"$hash_key"} = $hash_value;
		#Counting send events.
		$total_tx_count++;
	} elsif((defined($x[44])) && ($x[44] eq 'cbr') && ($x[18] eq 'MAC') && ($x[0] eq 'r')) {
		#Source IP is the key for the hash table.
		$hash_key = $x[30];
		$hash_value = $hash_table_receive{"$hash_key"};
		$hash_value++;
		#Per source receive count is maintained.
		$hash_table_receive{"$hash_key"} = $hash_value;
		$total_rx_count++;
	}
}
$simulation_time = $x[2];
#$packet_size = $x[36];

print("\n *** Trace File Analysis output ***\n");
#print("\n Packet Size = $packet_size bytes");
print("\n Total Transmit Count = $total_tx_count");
print("\n Total Receive Count = $total_rx_count");
print("\n Simulation Time = $simulation_time seconds");

if($total_rx_count != 0) {
	$system_throughput = ($total_rx_count) / $simulation_time;
        #print("\n System Throughput = $system_throughput packets per second.");
	$system_throughput = ($total_rx_count * $packet_size * 8) / $simulation_time;
	$system_throughput = $system_throughput / 1000;
	$result = sprintf("%.3f", $system_throughput);
        print("\n System Throughput = $result Kbps");
} else {
	print("\n No packet reached!");
}
print("\n");
for($sip = 1;$sip < $nodes;$sip++) {
	$hash_key = $sip.".0";
        $node_send_count = $hash_table_send{"$hash_key"};
        $node_receive_count = $hash_table_receive{"$hash_key"};
        $node_throughput = ($node_receive_count * $packet_size * 8) / $simulation_time;
        $node_throughput = $node_throughput / 1000;
	$result = sprintf("%.3f", $node_throughput);
        print("\n Node $sip  Send = $node_send_count Receive = $node_receive_count Throughput = $result Kbps");
	
	if($sip < ($nodes - $misbhnodes)) {
		$sum = $sum + $result;
		$sqr_sum = $sqr_sum + ($result * $result);
	} else {
		$sum_misbh = $sum_misbh + $result;
	}
}
print("\n");
if($misbhnodes == 0) {
	$ratio = 0;
} else {
	$ratio = ($sum_misbh/$misbhnodes)/($sum/(($nodes-1) - $misbhnodes));
}
print("\n Misbh/Normal Throughput Ratio = $ratio");

$fairness = ($sum * $sum) / ((($nodes - 1) - $misbhnodes) * $sqr_sum);
print("\n Fairness Index = $fairness");

print("\n");
print("\n");
close DATA;
exit(0);
