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
$ gcc -o main main.c -L/path/to/libucsan.a -ltsan -fsanitize=thread
```

Or you can move the `libucsan.a` to your project directory. Then:

```bash
$ gcc -o main main.c libucsan.a -fsanitize=thread
```
