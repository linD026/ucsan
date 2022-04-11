PWD := $(CURDIR)

INC=$(PWD)/include
INC_PARAMS=$(INC:%=-I%)

CC ?= gcc
CFLAG:=-g 
CFLAG+=-Wall
CFLAG+=-O1

#SRC:=core.c
#LIB:=per_cpu.c
SRC:=src/*.c
LIB:=lib/*.c

OBJ=$(SRC:.c=.o)
OBJ+=$(LIB:.c=.o)

%.o: %.c
	$(CC) $(CFLAG) $(INC_PARAMS) -c $< -o $@

all: static
	rm -f src/*.o
	rm -f lib/*.o
	rm -f *.o

static: $(OBJ)
	ar crsv ucsan.a $(OBJ)

clean:
	rm -f ucsan.a

indent:
	clang-format -i include/ucsan/*.[ch]
	clang-format -i src/*.[ch]
	clang-format -i lib/*.[ch]
