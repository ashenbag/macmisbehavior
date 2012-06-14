 Mac/802_11 set BeaconInterval_	       0.1		;# 100ms	
 Mac/802_11 set ScanType_	PASSIVE
 Mac/802_11 set ProbeDelay_	0.0001		;# 0.1 ms
 Mac/802_11 set MaxChannelTime_ 0.011		;# 11 ms
 Mac/802_11 set MinChannelTime_ 0.005		; # 5 ms
 Mac/802_11 set ChannelTime_ 0.12		;# 120 ms
#Following are the values used by all nodes. They are overwritten (configured) by the TCL script 
 Mac/802_11 set MisbhType_ 0			;# No misbehavior
 Mac/802_11 set MisbhValue_ 0.0			;# No misbehavior and so value does not matter
 Mac/802_11 set MisbhTime_ 0			;# Zero misbehavior time
 Mac/802_11 set MisbhReact_ 0			;# No misbehavior reaction
