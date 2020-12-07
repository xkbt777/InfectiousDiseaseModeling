//
// Created by shawn lv on 2020-12-03.
//

#ifndef INFECTIOUSDISEASEMODELING_COSTZ_DYNAMIC_MPI_H
#define INFECTIOUSDISEASEMODELING_COSTZ_DYNAMIC_MPI_H

#include "structs/util.h"
#include "structs/rectangle.h"
#include "structs/object.h"
#include "structs/r_tree.h"
#include "structs/matrix.h"

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#define TEST_SIZE 10000
#define MATRIX_SIZE 250.0f
#define ITER_TIME 100
#define RECONSTRUCTION_ITER 20
#define STEP_SIZE 1.0f
#define BLOCK_NUM_PER_DIM 8
#define CENTER_X 125.0f
#define CENTER_Y 125.0f
#define SIGMA 25.0f

void found_all_adjacent(block_t **matrix_map, int *block_assignment, int rank_id, int world_size, int *rec_num, rectangle_t ***ghost_zones);

#endif
