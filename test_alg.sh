#!/bin/bash

#	test_all.sh
#	2019-09-26	Markku-Juhani O. Saarinen <mjos@pqshield.com>

# Command line ?

if [ ! -n "$1" ]
then
	# note; randomizes order with shuf
	TARGETS=`du -a pqm4 | grep -E "\/api.h" | grep crypto | \
				sed 's/\/api.h//g' | colrm 1 8 | shuf` 
else
	TARGETS=$@
fi

mkdir -p log

echo $TARGETS

for alg in $TARGETS
do
	log=log/`echo $alg | tr '/' '_'`.`date +"%Y%m%d%H%M%S"`
	rm -rf BUILD $log

	echo path $alg
	echo ==== $log ====
	make clean
	make PQALG=$alg

	if [ -f "BUILD/pqps.hex" ]; then
		st-flash --format ihex write BUILD/pqps.hex
		if [ $? -eq 0 ]; then
			time timeout 1000 ./psctrl.py | tee $log
		fi
	fi
done

