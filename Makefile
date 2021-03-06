EXECS = rtree_test matrix_test linear_simulation mpi_simulation costz_dynamic_mpi
GCC ?= gcc
MPICC ?= mpicc
CFLAG = -lm -g -Wall -std=c99 -O3

all: rectangle.o r_tree.o object.o util.o matrix.o ${EXECS}

rectangle.o: structs/rectangle.c structs/rectangle.h
	${GCC} -c structs/rectangle.c ${CFLAG}

r_tree.o: structs/r_tree.c structs/r_tree.h structs/rectangle.c
	${GCC} -c structs/r_tree.c structs/rectangle.c ${CFLAG}

object.o: structs/object.c structs/object.h structs/rectangle.c
	${GCC} -c structs/object.c structs/rectangle.c ${CFLAG}

util.o: structs/util.c structs/util.h
	${GCC} -c structs/util.c ${CFLAG}

matrix.o: structs/matrix.c structs/matrix.h
	${GCC} -c structs/matrix.c ${CFLAG}

rtree_test: tests/rtree_test.c tests/rtree_test.h
	${GCC} -o rtree_test tests/rtree_test.c r_tree.o object.o rectangle.o ${CFLAG}

matrix_test: tests/matrix_test.c tests/matrix_test.h
	${GCC} -o matrix_test tests/matrix_test.c rectangle.o matrix.o ${CFLAG}

linear_simulation: linear_simulation.c linear_simulation.h
	${GCC} -o linear_simulation linear_simulation.c r_tree.o object.o rectangle.o util.o ${CFLAG}

mpi_simulation: mpi_simulation.c mpi_simulation.h
	${MPICC} -o mpi_simulation mpi_simulation.c r_tree.o object.o rectangle.o util.o ${CFLAG}

costz_dynamic_mpi: costz_dynamic_mpi.c costz_dynamic_mpi.h
	${MPICC} -o costz_dynamic_mpi costz_dynamic_mpi.c r_tree.o object.o rectangle.o util.o matrix.o ${CFLAG}

clean:
	rm -f *.o ${EXECS} object_data* tree matrix assignment