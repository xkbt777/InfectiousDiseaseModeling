EXECS = insert_test linear_simulation
GCC ?= gcc
CFLAG = -lm -g -Wall -std=c99 -O3

all: rectangle.o r_tree.o object.o ${EXECS}

rectangle.o: structs/rectangle.c structs/rectangle.h
	${GCC} -c structs/rectangle.c ${CFLAG}

r_tree.o: structs/r_tree.c structs/r_tree.h structs/rectangle.c
	${GCC} -c structs/r_tree.c structs/rectangle.c ${CFLAG}

object.o: structs/object.c structs/object.h structs/rectangle.c
	${GCC} -c structs/object.c structs/rectangle.c ${CFLAG}

insert_test: tests/insert_test.c tests/insert_test.h
	${GCC} -o insert_test tests/insert_test.c r_tree.o object.o rectangle.o ${CFLAG}

linear_simulation: linear_simulation.c linear_simulation.h
	${GCC} -o linear_simulation linear_simulation.c r_tree.o object.o rectangle.o ${CFLAG}

clean:
	rm -f *.o ${EXECS} object_data* tree