#/bin/bash

#ns cbrgen.tcl -type cbr -nn 10 -seed 5 -mc 10 -rate 2000k
#./setdest -v 1 -n 10 -p 102 -M 1 -t 100 -x 100 -y 100

SIM_TIME=3000
DATA_RATE=0.01
MISBH_TIME=3000

MISBH_TYPE=1

for n in 5 10 15 20 30 50
do
	echo "ns misbh_udp_cntrl.tcl $n ${SIM_TIME} ${DATA_RATE} 0 0 0 0 0 file_${n}_${SIM_TIME}_${DATA_RATE}_0_0_0"
	ns misbh_udp_cntrl.tcl $n ${SIM_TIME} ${DATA_RATE} 0 0 0 0 0 file_${n}_${SIM_TIME}_${DATA_RATE}_0_0_0
	for m in 1
	#for m in 1
	do
		for v in 0.1
		#for v in 0.1 0.6
		#for v in 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9
		#for v in 2 6 12 18 24 30 36 42 48 54 60
		#for v in 0 5 10 15 20 25 30 35
		do
			#for re in 0
			for re in 1
			do
				echo "ns misbh_udp_cntrl.tcl $n ${SIM_TIME} ${DATA_RATE} $m ${MISBH_TYPE} $v ${MISBH_TIME} $re file_${n}_${m}_${v}_${re}"
				ns misbh_udp_cntrl.tcl $n ${SIM_TIME} ${DATA_RATE} $m ${MISBH_TYPE} $v ${MISBH_TIME} $re file_${n}_${m}_${v}_${re}
				#echo "ns misbh_udp_cntrl.tcl $n ${SIM_TIME} ${DATA_RATE} $m ${MISBH_TYPE} $v ${MISBH_TIME} $re file_${n}_${m}_${v}_${re}"
				#ns misbh_udp_cntrl.tcl $n ${SIM_TIME} ${DATA_RATE} $m ${MISBH_TYPE} $v ${MISBH_TIME} $re | tee file_${n}_${m}_${v}_${re}
			done
		done
	done
done
