#include "linear_simulation.h"
#include <stdio.h>

int main() {
  object_t *object_pointer = NULL;
  rectangle_t *rectangle_pointer = NULL;

  random_generate(TEST_SIZE, MATRIX_SIZE, time(NULL), &object_pointer, &rectangle_pointer);

  object_to_file(object_pointer, rectangle_pointer, TEST_SIZE, "object_data", MATRIX_SIZE);

  char buf[BUF_SIZE];

  for (int i = 0; i < ITER_TIME; i++) {
    memset(buf, 0, BUF_SIZE * sizeof(char));
    sprintf(buf, "object_data%d", i);

    // movement
    for (int j = 0; j < TEST_SIZE; j++) {
      random_object_move(object_pointer + j, rectangle_pointer + j, STEP_SIZE, (float) MATRIX_SIZE);
    }

    // search and contact
    for (int j = 0; j < TEST_SIZE; j++) {
      object_t **search_object = NULL;

      size_t found = scan_search(object_pointer, rectangle_pointer, TEST_SIZE, rectangle_pointer[j], &search_object);

      for (int k = 0; k < found; k++) {
        contact(object_pointer + j, search_object[k], i);
      }

      free(search_object);
    }

    // status update
    status_update(object_pointer, TEST_SIZE, i);

    // to file
    object_to_file(object_pointer, rectangle_pointer, TEST_SIZE, buf, MATRIX_SIZE);
  }

  free(object_pointer);
  free(rectangle_pointer);

}