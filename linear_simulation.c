#include "linear_simulation.h"

int main(int argc, char* argv[]) {
  int use_rtree = 0, print = 0;
  int opt;
  struct timeval start, end;
  while ((opt = getopt(argc, argv, "r")) != -1) {
    switch (opt) {
      case 'r':
        use_rtree = 1;
        printf("Use r_tree\n");
        break;
      case 'p':
        print = 1;
        break;
      }
  }

  object_t *object_pointer = NULL;
  rectangle_t *rectangle_pointer = NULL;

  random_generate(TEST_SIZE, MATRIX_SIZE, time(NULL), &object_pointer, &rectangle_pointer);

  if (print) {
    object_to_file(object_pointer, rectangle_pointer, TEST_SIZE, "object_data", MATRIX_SIZE);
  }

  char buf[BUF_SIZE];

  gettimeofday(&start, NULL);

  for (int i = 0; i < ITER_TIME; i++) {
    memset(buf, 0, BUF_SIZE * sizeof(char));
    sprintf(buf, "object_data%d", i);

    // movement
    for (int j = 0; j < TEST_SIZE; j++) {
      random_object_move(object_pointer + j, rectangle_pointer + j, STEP_SIZE, (float) MATRIX_SIZE);
    }

    // structs construction
    r_tree_t *r_tree = NULL;
    if (use_rtree) {
      r_tree = init_rtree();
      for (int i = 0; i < TEST_SIZE; i++) {
        insert(r_tree->root, object_pointer + i, rectangle_pointer[i]);
      }
    }

    // search and contact
    for (int j = 0; j < TEST_SIZE; j++) {
      object_t **search_object = NULL;

      size_t found;
      if (use_rtree) {
        found = search(r_tree->root, rectangle_pointer[j], &search_object);
      } else {
        found = scan_search(object_pointer, rectangle_pointer, TEST_SIZE, rectangle_pointer[j], &search_object);
      }

      for (int k = 0; k < found; k++) {
        contact(object_pointer + j, search_object[k], i);
      }

      free(search_object);
    }

    // status update
    status_update(object_pointer, TEST_SIZE, i);

    // print to file
    if (print) {
      object_to_file(object_pointer, rectangle_pointer, TEST_SIZE, buf, MATRIX_SIZE);
    }

    if (use_rtree) {
      free_rtree(r_tree);
    }
  }

  free(object_pointer);
  free(rectangle_pointer);

  gettimeofday(&end, NULL);
  printf("Total time: %.3lf s\n", time_interval(start, end));
}