PWD := $(CURDIR)

nr_cpu=1
# The number of watchpoint slot
nr_wp=64

INC=$(PWD)/include
INC_PARAMS=$(INC:%=-I%)

CC := gcc
CFLAGS+=-Wall
CFLAGS+=-O1
# Position Independent Code suitable for use in a shared library.
CFLAGS+=-fPIC
CFLAGS+=-rdynamic
CFLAGS+=-D'UCSAN_NR_CPU=$(nr_cpu)'
CFLAGS+=-D'UCSAN_NR_WATCHPOINT'=$(nr_wp)

ifeq ($(CC), gcc)
CFLAGS+=-D'CONFIG_GCC=y'
endif

SRC:=src/core.c
SRC+=src/unify.c
LIB:=lib/per_cpu.c
TEST:=tests/test_watchpoint.c
TEST+=tests/test_unify.c

OBJ=$(SRC:.c=.o)
OBJ+=$(LIB:.c=.o)

ifeq ($(test), 1)
CFLAGS+=-lpthread
OBJ+=$(TEST:.c=.o)
endif

%.o: %.c
	$(CC) $(CFLAGS) $(INC_PARAMS) -c $< -o $@

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
	clang-format -i include/uapi/*.[ch]
	clang-format -i include/*.[ch]
	clang-format -i src/*.[ch]
	clang-format -i lib/*.[ch]
	clang-format -i tests/*.[ch]

ifeq ($(quiet), 1)
.SILENT:
endif
