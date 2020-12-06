#include "rtree_test.h"
#include <stdio.h>

int main() {
  object_t *object_pointer = NULL;
  rectangle_t *rectangle_pointer = NULL;

  random_generate(TEST_SIZE, MATRIX_SIZE, time(NULL), &object_pointer, &rectangle_pointer);

  object_to_file(object_pointer, rectangle_pointer, TEST_SIZE, "objects", MATRIX_SIZE);

  for (int i = 0; i < 10; i++) {
    printf("Object status: %d, left: %f, bottom: %f\n", object_pointer[i].status, rectangle_pointer[i].bottom_left.x, rectangle_pointer[i].bottom_left.y);
  }

  r_tree_t *r_tree = init_rtree();

  for (int i = 0; i < TEST_SIZE; i++) {
    insert(r_tree->root, object_pointer + i, rectangle_pointer[i]);
  }

  FILE *file = fopen("tree", "w");

  fprintf(file, "%d\n", MATRIX_SIZE);

  tree_to_file(r_tree->root, file, 0);

  fclose(file);

  delete(r_tree->root, object_pointer, rectangle_pointer[0]);
  delete(r_tree->root, object_pointer + 3, rectangle_pointer[3]);
  delete(r_tree->root, object_pointer + 5, rectangle_pointer[5]);
  delete(r_tree->root, object_pointer + 7, rectangle_pointer[7]);
  delete(r_tree->root, object_pointer + 9, rectangle_pointer[9]);

  object_t **search_object = NULL;

  size_t found = scan_search(object_pointer, rectangle_pointer, 10, init(2, 2, 5, 5), &search_object);

  for (int i = 0; i < found; i++) {
    printf("Object id: %d\n", search_object[i]->id);
  }

  free(search_object);

  found = search(r_tree->root, init(2, 2, 5, 5), &search_object);

  for (int i = 0; i < found; i++) {
    printf("Object id: %d\n", search_object[i]->id);
  }
  free(search_object);
  free_rtree(r_tree);

  free(object_pointer);
  free(rectangle_pointer);

}