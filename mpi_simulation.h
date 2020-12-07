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
#define MATRIX_SIZE 250.0f
#define ITER_TIME 100
#define RECONSTRUCTION_ITER 20
#define STEP_SIZE 1.0f
#define CENTER_X 125.0f
#define CENTER_Y 125.0f
#define SIGMA 25.0f

int static_gather(object_t *dst, object_t *src, rectangle_t *rectangle_dst, rectangle_t *rectangle_src, rectangle_t target, int rank_id);

#endif //INFECTIOUSDISEASEMODELING_MPI_SIMULATION_H
