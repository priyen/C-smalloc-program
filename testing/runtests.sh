#!/bin/bash

stuid=$1

# This script currently assumes that all the test code and traces plus the 
# student's smalloc.c file are in the current working directory.

# results is a comma separated string of results corresponding to the
# list of tests. 1 means pass, 0 means fail
#compilation,init,t1,t2,t3.1,t3.2,t3.3,t4,t4.1,t4.2,t5,t6,errors,valgrind
results="$stuid"

# compile
echo "*******************************************"
echo "*** Running make"
make  2>&1

if [ $? -eq 0 ] 
then
	echo "    passed"
	results="$results,1"
else
	echo "    failed"
	results="$results,0"
	echo $results > RESULTS.out
	exit
fi

echo "*******************************************"
echo "*** Testing init_mem"
testinit > ti.out
r=`diff ti.out expected/init.out  | head -n 1`
if [ "$r" =  "4c4" -o -z "$r" ] 
then
	results="$results,1"
else
	results="$results,0"
fi
	
# run each test, and compare the output
echo "*******************************************"
echo "*** Running main tests"

traces="1-malloc_one 2-malloc_several 3.1-free_first 3.2-free_middle 3.3-free_last 4-malloc_many 4.1-free_some 4.2-free_all 5-free_mixed 6-different_sizes" 

for tr in $traces
	do
		echo -ne "TEST: $tr \t"
		prefix=`echo $tr | cut -d "-" -f1`
		timeout 20 driver traces/$tr > $prefix.out
		ret1=$?
		timeout 20 driver -p traces/$tr > $prefix-full.out
		ret2=$?
		diff $prefix.out expected/$prefix.out
		ret3=$?
		sum=$(($ret1 + $ret2 + $ret3))
		echo RETURN $sum
		if [ $sum -eq 0 ]
	    then
		    echo "passed"
			results="$results,1"
		else
			echo "failed"
			results="$results,0"
		fi
	done

# check error cases
echo "*******************************************"
echo "*** Running test for errors:"
timeout 20 testerrors
if [ $? -eq 0 ]
then
	results="$results,1"
	echo "passed"
else
	echo "failed"
	results="$results,0"
	echo "passed"
fi

echo "*******************************************"
echo "** Testing for memory leaks"
timeout 20 valgrind driver traces/5-free_mixed >& val.out
if [ $? -eq 0 ]
then
	if grep "All heap blocks were freed" val.out 
		then
			results="$results,1"
			echo "passed"
		else
			results="$results,0"
		fi
else
	results="$results,0"
	echo "failed"
fi

echo $results > RESULTS.out
