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
	gcc -o watchpoint $obj -fsanitize=thread -lpthread

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

function unify {
	# Print unit test name
	print_unit_name "unify"

	# Compile
	local obj="$DIR/src/unify.o"
	gcc -o unify $obj -fsanitize=thread -lpthread -rdynamic

	# Execute and pipe the stderr to log file
	./unify 2> unify.log

	# Check " ERROR:" string and the count
	report_log "unify subsystem" unify.log
	if [[ $? -eq 1 ]]; then

		# Remove the generated file
		rm -f report.log
		rm -f ucsan_report.log
		rm -f unify
		print_unit_name "unify"
		make -C $DIR clean quiet=1 --no-print-directory

		# exit 1 will general errors
		exit 1
	fi

	# Remove the generated file
	rm -f unify.log
	rm -f ucsan_report.log
	rm -f unify
	print_unit_name "unify"
}

function data_race {
	# Print unit test name
	print_unit_name "data race"

	make -C $DIR clean quiet=1 --no-print-directory
	make -C $DIR quiet=1 --no-print-directory

	# Compile
	local lib="$DIR/libucsan.a"
	gcc -c test_data_race.c -fsanitize=thread
	gcc -o data_race test_data_race.o $lib -lpthread -rdynamic

	# Execute and pipe the stderr to log file
	./data_race 2> data_race.log

	# Check " ERROR:" string and the count
	report_log "data race" data_race.log
	if [[ $? -eq 1 ]]; then

		# Remove the generated file
		rm -f data_race.log
		rm -f ucsan_report.log
		rm -f data_race
		print_unit_name "data race"
		make -C $DIR clean quiet=1 --no-print-directory

		# exit 1 will general errors
		exit 1
	fi

	# Remove the generated file
	rm -f data_race.log
	rm -f ucsan_report.log
	rm -f data_race
	print_unit_name "data race"
}

make -C $DIR all quiet=1 test=1 --no-print-directory
echo ""
echo " ------------------------------ unit test start ------------------------------ "

# unit test function call here
detect
unify
data_race

echo " ------------------------------ unit test end -------------------------------- "
echo ""
make -C $DIR clean quiet=1 --no-print-directory
