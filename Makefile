EXECS = insert_test
GCC ?= gcc
CFLAG = -lm -g -Wall -std=c99 -O3

all: util.o r_tree.o object.o  ${EXECS}

util.o: util.c util.h
	${GCC} -c util.c ${CFLAG}

r_tree.o: r_tree.c r_tree.h util.c
	${GCC} -c r_tree.c util.c ${CFLAG}

object.o: object.c object.h util.c
	${GCC} -c object.c util.c ${CFLAG}

insert_test: insert_test.c insert_test.h
	${GCC} -o insert_test insert_test.c r_tree.o object.o util.o ${CFLAG}

clean:
	rm -f *.o ${EXECS} objects tree