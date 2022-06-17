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

# name log
function report_log() {
	local name=$1
	local log=$2

	local error_count=$(cat $log | egrep -c " ERROR:")
	local have_bug=$(cat $log | egrep -c " BUG:")
	if [ $error_count -gt 0 ] || [ $have_bug -gt 0 ]; then
		echo ""
		echo " $name test failed: $error_count error(s) "
		for_each_line "    " $log

		return 1
	fi

	return 0
}

# unit test function define here

function detect {
	# Print unit test name
	print_unit_name "detect"

	# Compile
	local obj="$DIR/src/core.o"
	gcc -o watchpoint test_watchpoint.o $obj -fsanitize=thread -lpthread

	# Execute and pipe the stderr to log file
	./watchpoint 2> detect.log

	# Check " ERROR:" string and the count
	report_log "detect subsystem" detect.log
	if [[ $? -eq 1 ]]; then

		# Remove the generated file
		rm -f detect.log
		rm -f watchpoint
		print_unit_name "detect"
		make -C $DIR clean quiet=1 --no-print-directory

		# exit 1 will general errors
		exit 1
	fi

	# Remove the generated file
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
