//
// Created by shawn lv on 2020-11-21.
//


#include "insert_test.h"
#include <stdio.h>

int main() {
  object_t *object_pointer = NULL;
  rectangle_t *rectangle_pointer = NULL;

  randomGenerate(TEST_SIZE, MATRIX_SIZE, time(NULL), &object_pointer, &rectangle_pointer, 0.1);

  object_to_file(object_pointer, rectangle_pointer, TEST_SIZE, "objects", MATRIX_SIZE);

  for (int i = 0; i < 10; i++) {
    printf("Object status: %zu, left: %f, bottom: %f\n", object_pointer[i].status, rectangle_pointer[i].bottom_left.x, rectangle_pointer[i].bottom_left.y);
  }

  r_tree_t *r_tree = init_rtree();

  for (int i = 0; i < TEST_SIZE; i++) {
    insert(r_tree->root, object_pointer + i, rectangle_pointer[i]);
  }

  FILE *file = fopen("tree", "w");

  fprintf(file, "%d\n", MATRIX_SIZE);

  tree_to_file(r_tree->root, file, 0);

  fclose(file);

  object_t **search_object = NULL;

  size_t found = scanSearch(object_pointer, rectangle_pointer, 10, init(2, 2, 5, 5), &search_object);

  for (int i = 0; i < found; i++) {
    printf("Object id: %ld\n", search_object[i]->id);
  }

  free(search_object);

  found = search(r_tree->root, init(2, 2, 5, 5), &search_object);

  for (int i = 0; i < found; i++) {
    printf("Object id: %ld\n", search_object[i]->id);
  }
  free(search_object);
  // free_rtree(r_tree);

  free(object_pointer);
  free(rectangle_pointer);

}