#!/bin/bash

#	test_all.sh
#	2019-09-26	Markku-Juhani O. Saarinen <mjos@pqshield.com>

# Command line ?

if [ ! -n "$1" ]
then
        TARGETS="pqm4/crypto_*/*/* pqm4/mupq/crypto_*/*/* pqm4/mupq/pqclean/crypto_*/*/*"
else
        TARGETS=$@
fi

mkdir -p log
for alg in $TARGETS
do
	log=log/`echo $alg | tr '/' '_'`.txt
	rm -f $log
	echo ==== $alg ====
	make clean
	make PQALG=$alg flash
	time timeout 100 ./psctrl.py | tee $log
done

