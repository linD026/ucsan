# ucsan

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
