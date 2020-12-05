//
// Created by shawn lv on 2020-12-03.
//

#include "matrix.h"

block_t **matrix_generator(int block_num_per_dim, float matrix_size) {
  block_t **matrix = (block_t **) calloc(block_num_per_dim, sizeof(block_t *));

  for (int i = 0; i < block_num_per_dim; i++) {
    matrix[i] = (block_t *) calloc(block_num_per_dim, sizeof(block_t));
  }

  point_t point;
  point.x = 0.0f;
  point.y = 0.0f;
  matrix_generator_recursive(matrix, 0, 0, point, 0, block_num_per_dim, matrix_size, LEFT_TO_RIGHT);
  return matrix;
}



void matrix_generator_recursive(block_t **matrix, int row, int column, point_t bottom_left, int start_index, int block_index_size, float block_size, int direction) {
  if (block_index_size == 1) {
    block_t block;
    block.rectangle.bottom_left = bottom_left;
    block.rectangle.top_right.x = block.rectangle.bottom_left.x + block_size;
    block.rectangle.top_right.y = block.rectangle.bottom_left.y + block_size;
    block.id = start_index;
    matrix[row][column] = block;
    return;
  }

  int total_index = block_index_size * block_index_size;

  point_t bottom_left_up_half = bottom_left;
  bottom_left_up_half.y += block_size / 2;

  point_t mid_point = bottom_left;
  mid_point.x += block_size / 2;
  mid_point.y += block_size / 2;

  point_t bottom_left_right_half = bottom_left;
  bottom_left_right_half.x += block_size / 2;


  if (direction == LEFT_TO_RIGHT) {
    matrix_generator_recursive(matrix, row, column, bottom_left, start_index, block_index_size / 2, block_size / 2, BOT_TO_TOP);

    matrix_generator_recursive(matrix, row + block_index_size / 2, column,
                               bottom_left_up_half, start_index + total_index / 4, block_index_size / 2,
                               block_size / 2, LEFT_TO_RIGHT);

    matrix_generator_recursive(matrix, row + block_index_size / 2, column + block_index_size / 2,
                               mid_point, start_index + total_index / 2, block_index_size / 2,
                               block_size / 2, LEFT_TO_RIGHT);


    matrix_generator_recursive(matrix, row, column + block_index_size / 2, bottom_left_right_half,
                               start_index + total_index * 3 / 4, block_index_size / 2, block_size / 2, TOP_TO_BOT);
  } else if (direction == BOT_TO_TOP) {
    matrix_generator_recursive(matrix, row, column,
                               bottom_left, start_index, block_index_size / 2, block_size / 2, LEFT_TO_RIGHT);

    matrix_generator_recursive(matrix, row, column + block_index_size / 2, bottom_left_right_half,
                               start_index + total_index / 4, block_index_size / 2, block_size / 2, BOT_TO_TOP);


    matrix_generator_recursive(matrix, row + block_index_size / 2, column + block_index_size / 2,
                               mid_point, start_index + total_index / 2, block_index_size / 2, block_size / 2, BOT_TO_TOP);


    matrix_generator_recursive(matrix, row + block_index_size / 2, column, bottom_left_up_half,
                               start_index + total_index * 3 / 4, block_index_size / 2, block_size / 2, RIGHT_TO_LEFT);
  } else if (direction == RIGHT_TO_LEFT) {
    matrix_generator_recursive(matrix, row + block_index_size / 2, column + block_index_size / 2,
                               mid_point, start_index, block_index_size / 2, block_size / 2,
                               TOP_TO_BOT);

    matrix_generator_recursive(matrix, row, column + block_index_size / 2,
                               bottom_left_right_half, start_index + total_index / 4, block_index_size / 2, block_size / 2, RIGHT_TO_LEFT);

    matrix_generator_recursive(matrix, row, column,
                               bottom_left, start_index + total_index / 2, block_index_size / 2, block_size / 2, RIGHT_TO_LEFT);

    matrix_generator_recursive(matrix, row + block_index_size / 2, column,
                               bottom_left_up_half, start_index + total_index * 3 / 4,
                               block_index_size / 2, block_size / 2, BOT_TO_TOP);
  } else {
    matrix_generator_recursive(matrix, row + block_index_size / 2, column + block_index_size / 2,
                               mid_point, start_index, block_index_size / 2, block_size / 2, RIGHT_TO_LEFT);

    matrix_generator_recursive(matrix, row + block_index_size / 2, column,
                               bottom_left_up_half, start_index + total_index / 4, block_index_size / 2, block_size / 2, TOP_TO_BOT);

    matrix_generator_recursive(matrix, row, column, bottom_left, start_index + total_index / 2,
                               block_index_size / 2, block_size / 2, TOP_TO_BOT);

    matrix_generator_recursive(matrix, row, column + block_index_size / 2,
                               bottom_left_right_half, start_index + total_index * 3 / 4,
                               block_index_size / 2, block_size / 2, LEFT_TO_RIGHT);
  }
}

void get_belonged_block(point_t point, int block_num_per_dim, float matrix_size, int* x, int* y) {
  float block_side_length = matrix_size / block_num_per_dim;
  *x = (int)min(floor(point.y / block_side_length), block_num_per_dim - 1.0);
  *y = (int)min(floor(point.x / block_side_length), block_num_per_dim - 1.0);
}