# Unit Test

## How to use?

First go to the tests directory and excute the script `unit_test.sh`:
```bash
$ cd tests
$ ./unit_test.sh
```

The log message will be like:

```log
a - src/core.o
a - src/unify.o
a - lib/per_cpu.o
a - tests/test_watchpoint.o

 ------------------------------ unit test start ------------------------------ 

 ============================== detect ============================== 

 [1655330692817227] tests/test_watchpoint.c:128:test_dummy(): Hello world!
./unit_test.sh: line 27: 206834 Aborted                 (core dumped) ./watchpoint 2> detect.log

 detect subsystem test failed: 1 error(s) 
     [1655330692817227] INFO: tests/test_watchpoint.c+128:test_dummy(): Hello world!
     [1655330692817300] ERROR: tests/test_watchpoint.c:131:test_dummy(): Here is the error.
     [1655330692817303] BUG: tests/test_watchpoint.c:142:main(): test_dummy() failed

 ============================== detect ============================== 

```

## Establish the unit test

You need to define the flag in the file you want to test and include the `unit_test.h` header at the bottom of the file.
Following are the example for the detect part of `src/core.c`:

```cpp
/* src/core.c */

/* detect function define here ... */

#define __UT_DETECT
#include <unit_test.h>
/* bottom of the file */
```

After that, you should include your test suit file (which will be in the tests directory) in `unit_test.h` header.
```cpp
/* include/unit_test.h */

#if defined(__UT_DETECT)
#include "../tests/test_watchpoint.c"
#endif
```

For the test suit file, you need to design your testing.
You can call the function in the test suit file and check the state is correct.
It also provides the I/O function to let you print the information and let the script know the error happens.
The script will count the " ERROR:" string in stderr.
If the count is bigger than zero, the test fail.

```cpp
/* tests/test_watchpoint.c */

#include "printut.h"

#undef __FILE_NAME__
#define __FILE_NAME__ "tests/test_watchpoint.c"

static int test_dummy(void)
{
	// It will print with the prefix " INFO:"
	pr_info("Hello world!\n");

	// It will print with the prefix " ERROR:"
	pr_err("Here is the error.\n");

	// stop execute when the condition is true in BUG_ON()
	//BUG_ON(1);

	// return 0 if test success
	return -1;
}

int main(void)
{
	UNIT_BUG_ON(test_dummy());

	return 0;
}
```

Also, you need to write your unit test script function in `unit_test.sh`:

```bash
function detect {
	# Print unit test name
	print_unit_name "detect"

	# Compile
	local obj="$DIR/src/core.o"
	gcc -o watchpoint test_watchpoint.o $obj -fsanitize=thread -lpthread

	# Execute and pipe the stderr to log file
	./watchpoint 2> detect.log

	# Check " ERROR:" string and the count
	local error_count=$(cat detect.log | egrep -c " ERROR:")
	if [ $error_count -gt 0 ]; then

		# Test failed
		echo ""
		echo " detect subsystem test failed: $error_count error(s) "
		for_each_line "    " detect.log

		# Remove the generated file
		rm -f detect.log
		rm -f watchpoint
		make -C $DIR clean quiet=1 --no-print-directory
		print_unit_name "detect"

		# exit 1 will general error to shell
		exit 1
	fi

	# Remove the generated file
	rm -f detect.log
	rm -f watchpoint
	print_unit_name "detect"
}

...

# unit test function call here
detect				# Add the unit test function

...
```
