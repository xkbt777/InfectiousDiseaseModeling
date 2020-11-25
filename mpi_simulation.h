//
// Created by shawn lv on 2020-11-25.
//

#ifndef INFECTIOUSDISEASEMODELING_MPI_SIMULATION_H
#define INFECTIOUSDISEASEMODELING_MPI_SIMULATION_H

#include "structs/util.h"
#include "structs/rectangle.h"
#include "structs/object.h"
#include "structs/r_tree.h"

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#define TEST_SIZE 10000
#define MATRIX_SIZE 256
#define ITER_TIME 10
#define STEP_SIZE 1.0f

int static_gather(int *indexes, rectangle_t *rectangles, rectangle_t target, int rank_id);

#endif //INFECTIOUSDISEASEMODELING_MPI_SIMULATION_H
