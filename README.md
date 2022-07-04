# ucsan

The User Concurrency Sanitizer (UCSAN) is the watchpoint-based data race detector.
It uses the [thread sanitizer](https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual) interface in [Clang](https://releases.llvm.org/3.8.0/tools/clang/docs/ThreadSanitizer.html) (v3.2+) and [GCC](https://gcc.gnu.org/gcc-4.8/changes.html) (v4.8+) to determine whether the variable data race.

Currently, it only will check the non-volatile type of variable access.
In other words, all the variables with thread-safety operations can declare as volatile types to avoid data race checking.

## Get start

## Build

Build this project with following commands:

```bash
$ make                  # Generate static library: libucsan.a
$ make clean            # Delete generated files
```

### Makefile Parameter

* `nr_cpu` : number of cpu.
* `nr_wp` : number of watchpoint slot.
* `CC` : compiler, gcc or clang.

## How to use?

Add the static library `libucsan.a` to your project:

```bash
$ gcc -c main.c -fsanitize=thread
$ gcc -o main main.o -L/path/to/libucsan/ -lucsan -rdynamic -pthread
```

Or you can move the `libucsan.a` to your project directory. Then:

```bash
$ gcc -c main.c -fsanitize=thread
$ gcc -o main main.o libucsan.a -rdynamic -pthread
```
