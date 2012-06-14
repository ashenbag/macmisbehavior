#/bin/bash
SIM_TIME=6000
DATA_RATE=0.01
MISBH_TIME=6000
MISBH_TYPE=1
for n in 5 10 15 20
do
	echo "ns misbh_udp_cntrl.tcl $n ${SIM_TIME} ${DATA_RATE} 0 0 0 0 0 file_${n}_${SIM_TIME}_${DATA_RATE}_0_0_0"
	ns misbh_udp_cntrl.tcl $n ${SIM_TIME} ${DATA_RATE} 0 0 0 0 0 file_${n}_${SIM_TIME}_${DATA_RATE}_0_0_0
	for m in 1 2
	do
		for v in 0.1
		do
			for re in 0 1
			do
				echo "ns misbh_udp_cntrl.tcl $n ${SIM_TIME} ${DATA_RATE} $m ${MISBH_TYPE} $v ${MISBH_TIME} $re file_${n}_${m}_${v}_${re}"
				ns misbh_udp_cntrl.tcl $n ${SIM_TIME} ${DATA_RATE} $m ${MISBH_TYPE} $v ${MISBH_TIME} $re file_${n}_${m}_${v}_${re}
			done
		done
	done
done
