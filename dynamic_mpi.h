//
// Created by shawn lv on 2020-12-03.
//

#ifndef INFECTIOUSDISEASEMODELING_DYNAMIC_MPI_H
#define INFECTIOUSDISEASEMODELING_DYNAMIC_MPI_H

#include "structs/util.h"
#include "structs/rectangle.h"
#include "structs/object.h"
#include "structs/r_tree.h"

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#define TEST_SIZE 1000
#define MATRIX_SIZE 100
#define ITER_TIME 100
#define STEP_SIZE 1.0f
#define BLOCK_NUM_PER_DIM 8

typedef struct block {
  int id;
  rectangle_t rectangle;
}block_t;

block_t **matrix_generator(int block_num_per_dim, float matrix_size);

#endif
