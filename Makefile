PWD := $(CURDIR)

nr_cpu=1

INC=$(PWD)/include
INC_PARAMS=$(INC:%=-I%)

CC ?= gcc
CFLAGS:=-g
CFLAGS+=-Wall
CFLAGS+=-O1
CFLAGS+=-D'UCSAN_NR_CPU=$(nr_cpu)'

SRC:=src/core.c
LIB:=lib/per_cpu.c

OBJ=$(SRC:.c=.o)
OBJ+=$(LIB:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) $(INC_PARAMS) -c $< -o $@

all: static
	rm -f src/*.o
	rm -f lib/*.o

static: $(OBJ)
	ar crsv ucsan.a $(OBJ)

clean:
	rm -f ucsan.a

indent:
	clang-format -i include/ucsan/*.[ch]
	clang-format -i src/*.[ch]
	clang-format -i lib/*.[ch]
