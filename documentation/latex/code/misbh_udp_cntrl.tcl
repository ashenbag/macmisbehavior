# Initialize some parameters
Mac/802_11 set dataRate_ 2Mb
# rts thrshold is in bytes
Mac/802_11 set RTSThreshold_ 128

# ======================================================================
# Define options
# ======================================================================

set val(chan) 	Channel/WirelessChannel;	# channel type
set val(prop) 	Propagation/TwoRayGround;	# radio-propagation model
set val(netif) 	Phy/WirelessPhy;		# network interface type
set val(mac) 	Mac/802_11;			# MAC type
set val(ifq) 	Queue/DropTail/PriQueue;	# interface queue type
set val(ll) 	LL;				# link layer type
set val(ant) 	Antenna/OmniAntenna;		# antenna model
set val(x)      100;				# X dimension of the topography
set val(y)      100;				# Y dimension of the topography
set val(ifqlen) 50;				# max packet in ifq
set val(adhocRouting)   DumbAgent;		# routing protocol
# ======================================
# Main Program
# =======================================
if { $argc != 9 } {
        puts "Wrong no. of cmdline args."
        puts "Usage: ns misbh_udp_cntrl.tcl <nodes> <simulation time> <rate> <misbhnodes> <misbhtype> <misbhvalue> <misbhtime> <misbhreact> <outfile>"
        exit 0
}

set nodes [lindex $argv 0]
set stime [lindex $argv 1]
set rate [lindex $argv 2]
set misbhnodes [lindex $argv 3]
set misbhtype [lindex $argv 4]
set misbhvalue [lindex $argv 5]
set misbhtime [lindex $argv 6]
set misbhreact [lindex $argv 7]
set outfile [lindex $argv 8]

set  $misbhtime [expr {$misbhtime + 200}]

puts "Misbhnodes is $misbhnodes"
puts "Misbhtime is $misbhtime"
puts "Misbhreact is $misbhreact"

# Initialize Global Variables
set cwmin 31
set cwmax 1023
set packetsize 512
set trace_file trace_misbh_udp_cntrl.tr
#set nam_file nam_misbh_udp_cntrl.nam

# create simulator instance
set ns_         [new Simulator]

global defaultRNG
$defaultRNG seed 0

# setup topography object
set topo        [new Topography]

#Deleting Trace file
#exec rm $trace_file
#exec rm $nam_file

# create trace object for ns and nam
set tracefd 	[open $trace_file w]
#set namfd 	[open $nam_file w]

$ns_ trace-all $tracefd
#$ns_ namtrace-all-wireless $namfd $val(x) $val(y)
$ns_ use-newtrace

# define topology
$topo load_flatgrid $val(x) $val(y)

#
# Create God
#
create-god $nodes

# configure node
#

#$val(mac) set bandwidth_ 1.0e6
#$val(mac) set CWMin_ 31
#$val(mac) set CWMax_ 1023

#Node 0 is receiving. So niether misbehavior nor reaction.
$val(mac) set MisbhType_ 0
$val(mac) set MisbhValue_ 0.0
$val(mac) set MisbhTime_ 0
$val(mac) set MisbhReact_ 0

$ns_ node-config -adhocRouting $val(adhocRouting) \
		 -llType $val(ll) \
		 -macType $val(mac) \
		 -ifqType $val(ifq) \
		 -ifqLen $val(ifqlen) \
		 -antType $val(ant) \
		 -propType $val(prop) \
		 -phyType $val(netif) \
		 -channelType $val(chan) \
		 -topoInstance $topo \
		 -agentTrace OFF \
		 -routerTrace OFF \
		 -macTrace ON \
		 -movementTrace OFF 

set node_(0) [$ns_ node]

#Create the specified number of mobilenodes [$nodes] and "attach" them to the channel.
#Here nine nodes are created-nodes 1 through 8
#Nodes can react only. They cannot misbehave.
$val(mac) set MisbhType_ 0
$val(mac) set MisbhValue_ 0
$val(mac) set MisbhTime_ 0
$val(mac) set MisbhReact_ $misbhreact

$ns_ node-config -macType $val(mac)

for {set i 1} {$i < [expr {$nodes-$misbhnodes}] } {incr i} {
	set node_($i) [$ns_ node]
	$node_($i) random-motion 0;# disable random motion
}

#Misbehaving node is has different configuration. After setting configuration parameters, misbehaving node is created.
#Misbehaving node cannot react.
proc set_misbh {misbhtype misbhvalue} {
	global val cwmin cwmax misbhtime
	if {$misbhtype == 0} {
		puts "No Misbehavior."
		puts "CW Min is $cwmin"
		puts "CW Max is $cwmax"
		$val(mac) set CWMin_ $cwmin
		$val(mac) set CWMax_ $cwmax
		$val(mac) set MisbhType_ 0
		$val(mac) set MisbhValue_ 0
		$val(mac) set MisbhReact_ 0
		puts "MisbhValue_ is 0"
	} elseif {$misbhtype == 1} {
		puts "Type 1 Misbehavior. Alpha behavior."
		puts "CW Min is $cwmin"
		puts "CW Max is $cwmax"
		$val(mac) set CWMin_ $cwmin
		$val(mac) set CWMax_ $cwmax
		$val(mac) set MisbhType_ 1
		$val(mac) set MisbhValue_ $misbhvalue
		$val(mac) set MisbhReact_ 0
		puts "MisbhValue_ is $misbhvalue."
	} elseif {$misbhtype == 2} {
		puts "Type 2 Misbehavior. Deterministic behavior."
		puts "CW Min is $cwmin"
		puts "CW Max is $cwmax"
		$val(mac) set CWMin_ $cwmin
		$val(mac) set CWMax_ $cwmax
		$val(mac) set MisbhType_ 2
		$val(mac) set MisbhValue_ $misbhvalue
		$val(mac) set MisbhReact_ 0
		puts "MisbhValue_ is $misbhvalue."
	} elseif {$misbhtype == 3} {
		puts "Type 3 Misbehavior. Beta behavior."
		if {$misbhvalue < 31} {
			set cwmin [expr {$misbhvalue*31}]
		}
		puts "CW Min is $cwmin"
		puts "CW Max is $cwmax"
		$val(mac) set CWMin_ $cwmin
		$val(mac) set CWMax_ $cwmax
		$val(mac) set MisbhType_ 3
		$val(mac) set MisbhValue_ $misbhvalue
		$val(mac) set MisbhReact_ 0
		puts "MisbhValue_ is $misbhvalue."
	} elseif {$misbhtype == 4} {
		puts "Type 4 Misbehavior. Fixed Maximum Contention Window behavior."
		if {$misbhvalue < 31} {
			set cwmin $misbhvalue
		}
		puts "CW Min is $cwmin"
		puts "CW Max is $misbhvalue"
		$val(mac) set CWMin_ $cwmin
		$val(mac) set CWMax_ $misbhvalue
		$val(mac) set MisbhType_ 4
		$val(mac) set MisbhValue_ 0.0
		$val(mac) set MisbhReact_ 0
		puts "MisbhValue_ is $misbhvalue."
	} elseif {$misbhtype == 5} {
		puts "Type 5 Misbehavior. Fixed Contention Window behavior."
		$val(mac) set MisbhType_ 5
		$val(mac) set MisbhValue_ $misbhvalue
		$val(mac) set MisbhReact_ 0
		puts "MisbhValue_ is $misbhvalue."
	} else {
		puts "Invalid Misbehavior Type. Considering it as no misbehavior."
		$val(mac) set MisbhType_ 0
		$val(mac) set MisbhValue_ 0.0
		$val(mac) set MisbhReact_ 0
	}
	$val(mac) set MisbhTime_ $misbhtime
}

#Creating Misbehaving nodes
for {set count 0} {$count < $misbhnodes} {incr count} {
	#If all misbehaving need to use same misbhvalue, multiply count by zero in below line.
	set_misbh $misbhtype [expr {$misbhvalue + ($count * 0)}]
	$ns_ node-config -macType $val(mac)
	set node_($i) [$ns_ node]
	$node_($i) random-motion 0;# disable random motion
	incr i
}

#Provide initial (X,Y, for now Z=0) co-ordinates for mobilenodes
$node_(0) set X_ 50.0
$node_(0) set Y_ 50.0
$node_(0) set Z_ 0.0

##nodes in a circular way
set pi 3.1416
set r 50
set a [expr {2*$pi*0.111111111}]
for {set i 1} {$i < $nodes } {incr i} {
	$node_($i) set X_ [ expr { 50 + ($r * cos($i * $a))}]
	$node_($i) set Y_ [ expr { 50 + ($r * sin($i * $a))}]
	$node_($i) set Z_ 0.0
	$ns_ initial_node_pos $node_($i) 2
}

$ns_ initial_node_pos $node_(0) 2

#traffic between nodes
set sink [new Agent/Null]
$ns_ attach-agent $node_(0) $sink

for {set i 1} {$i < $nodes } {incr i} {
	set udp($i) [new Agent/UDP]
	$udp($i) set class_ 2
	set cbr($i) [new Application/Traffic/CBR]
	$cbr($i) attach-agent $udp($i)
	$cbr($i) set packetSize_ $packetsize
        $cbr($i) set interval_ $rate
        #$cbr($i) set rate_ $rate
        $cbr($i) set random_ false

	$ns_ attach-agent $node_($i) $udp($i)

	$ns_ connect $udp($i) $sink

	$ns_ at 0.0 "$cbr($i) start"
}

# Tell nodes when the simulation ends
for {set i 0} {$i < $nodes } {incr i} {
	$ns_ at $stime "$node_($i) reset";
}
#$ns_ at $stime "puts \"NS Exiting...\" "
$ns_ at $stime "stop"

#Define a 'stop' procedure
proc stop {} {
	#global ns_ tracefd namfd
	global ns_ tracefd nodes trace_file outfile misbhnodes
	#global nam_file
	$ns_ flush-trace
	#close $namfd
	close $tracefd
	#exec nam $nam_file &
	$ns_ halt
	puts "Analyzing trace file..."
	set tmp [ exec perl throughput_cntrl.pl $trace_file $nodes $misbhnodes | tee ./run/$outfile]
	puts stdout $tmp
	
	#Deleting Trace file
	#exec rm $trace_file
	#exec rm $nam_file

	exit 0
}

puts "Starting Simulation..."
$ns_ run
