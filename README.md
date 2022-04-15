# ucsan

## Get start

## Build

Build this project with following commands:

```bash
$ make                  # Generate static library: libtsan.a
$ make clean            # Delete germerated files
```

### Makefile Option

* `nr_cpu` : number of cpu.
* `CC` : compiler, gcc or clang.

## How to use?

Add the static library `libtsan.a` to your project:

```bash
$ gcc -o main main.c -L/path/to/libtsan.a -ltsan -fsanitize=thread
```

Or you can move the `libtsan.a` to your project directory. Then:

```bash
$ gcc -o main main.c libtsan.a -fsanitize=thread
```
