//
// Created by shawn lv on 2020-12-03.
//
#include "matrix_test.h"

int main() {
  FILE *file = fopen("matrix", "w");

  block_t **matrix = matrix_generator(BLOCK_SIZE, MATRIX_SIZE);

  fprintf(file, "%f\n", MATRIX_SIZE);
  for (int r = 0; r < BLOCK_SIZE; r++) {
    for (int c = 0; c < BLOCK_SIZE; c++) {
      fprintf(file, "%d,%f,%f,%f,%f\n", matrix[r][c].id, matrix[r][c].rectangle.bottom_left.x, matrix[r][c].rectangle.bottom_left.y,
        matrix[r][c].rectangle.top_right.x, matrix[r][c].rectangle.top_right.y);
    }
  }
  fclose(file);
}