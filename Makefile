EXECS = rtree object
GCC ?= gcc
CFLAG = -lm -g -Wall -std=c99 -O3

all: util.o ${EXECS}

util.o: util.c util.h
	${GCC} -c util.c ${CFLAG}

rtree: r_tree.c r_tree.h
	${GCC} -o rtree r_tree.c util.o ${CFLAG}

object: object.c object.h
	${GCC} -o object object.c util.o ${CFLAG}

clean:
	rm -f *.o ${EXECS}