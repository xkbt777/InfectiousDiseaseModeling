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

#define TEST_SIZE 200
#define MATRIX_SIZE 64
#define ITER_TIME 1
#define REALLOCATE_TIME 1
#define STEP_SIZE 1.0f

int static_gather(object_t *dst, object_t *src, rectangle_t *rectangle_dst, rectangle_t *rectangle_src, rectangle_t target, int rank_id);

#endif //INFECTIOUSDISEASEMODELING_MPI_SIMULATION_H
