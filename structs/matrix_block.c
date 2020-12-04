//
// Created by 张新悦 on 12/3/20.
//

#include "matrix_block.h"

#include <math.h>

block_t **matrix_generator(int block_num_per_dim, float matrix_size) {

}

void get_belonged_block(point_t point, int block_num_per_dim, float matrix_size, int* x, int* y) {
  float block_side_length = matrix_size / block_num_per_dim;
  *x = block_num_per_dim - 1 - (int)floor(point.y / block_side_length);
  *y = (int)floor(point.x / block_side_length);
}
