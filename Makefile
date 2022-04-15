PWD := $(CURDIR)

nr_cpu=1

INC=$(PWD)/include
INC_PARAMS=$(INC:%=-I%)

CC := gcc
CFLAGS+=-Wall
CFLAGS+=-O1
CFLAGS+=-fPIC
CFLAGS+=-D'UCSAN_NR_CPU=$(nr_cpu)'

ifeq ($(CC), gcc)
CFLAGS+=-D'CONFIG_GCC=y'
endif

SRC:=src/core.c
SRC+=src/tsan.c
LIB:=lib/per_cpu.c

OBJ=$(SRC:.c=.o)
OBJ+=$(LIB:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) $(INC_PARAMS) -fPIC -c $< -o $@

all: static

static: $(OBJ)
	ar crsv libucsan.a $(OBJ)
	ranlib libucsan.a

clean:
	rm -f libucsan.a
	rm -f src/*.o
	rm -f lib/*.o
	rm -f tests/*.o

indent:
	clang-format -i include/ucsan/*.[ch]
	clang-format -i src/*.[ch]
	clang-format -i lib/*.[ch]
