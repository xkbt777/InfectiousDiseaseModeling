//
// Created by 张新悦 on 12/3/20.
//

#ifndef INFECTIOUSDISEASEMODELING_MATRIX_BLOCK_H
#define INFECTIOUSDISEASEMODELING_MATRIX_BLOCK_H

#include "rectangle.h"

typedef struct block {
  int id;
  rectangle_t rectangle;
} block_t;

block_t **matrix_generator(int block_num_per_dim, float matrix_size);
void get_belonged_block(point_t point, int block_num_per_dim, float matrix_size, int* x, int* y);

#endif //INFECTIOUSDISEASEMODELING_MATRIX_BLOCK_H
