//
// Created by shawn lv on 2020-12-03.
//

#ifndef INFECTIOUSDISEASEMODELING_MATRIX_H
#define INFECTIOUSDISEASEMODELING_MATRIX_H

#include "rectangle.h"
#include <stdlib.h>

#define LEFT_TO_RIGHT 0
#define BOT_TO_TOP 1
#define RIGHT_TO_LEFT 2
#define TOP_TO_BOT 3


typedef struct block {
  int id;
  rectangle_t rectangle;
}block_t;

block_t **matrix_generator(int block_num_per_dim, float matrix_size);
void matrix_generator_recursive(block_t **offset, int row, int column, point_t bottom_left, int start_index, int block_index_size, float block_size, int direction);
#endif //INFECTIOUSDISEASEMODELING_MATRIX_H
