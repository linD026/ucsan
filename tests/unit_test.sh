#!/usr/bin/env bash

DIR="$(pwd)/.."
INCLUDE="$DIR/include"

IFS='
'

# prefix file
function for_each_line() {
	args=("$@")
	TEMP="${args[1]}"
	file=`cat $TEMP`
	for line in $file; do
		echo "$1$line"
	done
}

function print_unit_name() {
	echo ""
	echo " ============================== $1 ============================== "
	echo ""
}

# unit test function define here

function detect {
	print_unit_name "detect"
	local obj="$DIR/src/core.o"
	gcc -o watchpoint test_watchpoint.o $obj -fsanitize=thread -lpthread
	./watchpoint 2> detect.log
	local error_count=$(cat detect.log | egrep -c " ERROR:")
	if [ $error_count -gt 0 ]; then
		echo ""
		echo " detect subsystem test failed: $error_count error(s) "
		for_each_line "    " detect.log
		rm -f detect.log
		rm -f watchpoint
		make -C $DIR clean quiet=1 --no-print-directory
		print_unit_name "detect"
		exit 1
	fi
	rm -f detect.log
	rm -f watchpoint
	print_unit_name "detect"
}

make -C $DIR all quiet=1 test=1 --no-print-directory
echo ""
echo " ------------------------------ unit test start ------------------------------ "

# unit test function call here
detect

echo " ------------------------------ unit test end -------------------------------- "
echo ""
make -C $DIR clean quiet=1 --no-print-directory
