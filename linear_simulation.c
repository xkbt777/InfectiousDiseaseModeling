#include "linear_simulation.h"

int main(int argc, char* argv[]) {
  int use_rtree = 0, print = 0, use_normal = 0;
  unsigned int seed = time(NULL);
  int opt;
  struct timeval start, end;
  while ((opt = getopt(argc, argv, "rpns:")) != -1) {
    switch (opt) {
      case 'r':
        use_rtree = 1;
        printf("Use r_tree\n");
        break;
      case 'n':
        use_normal = 1;
        printf("Use Normal Distribution on center : %f, %f sigma: %f\n", CENTER_X, CENTER_Y, SIGMA);
        break;
      case 'p':
        print = 1;
        break;
      case 's':
        seed = atoi(optarg);
        break;
      }
  }

  object_t *object_pointer = NULL;
  rectangle_t *rectangle_pointer = NULL;
  gettimeofday(&start, NULL);

  if (use_normal) {
    center_generate(TEST_SIZE, MATRIX_SIZE, CENTER_X, CENTER_Y, SIGMA, seed, &object_pointer, &rectangle_pointer);
  } else {
    random_generate(TEST_SIZE, MATRIX_SIZE, seed, &object_pointer, &rectangle_pointer);
  }

  object_statistic(object_pointer, TEST_SIZE);

  if (print) {
    object_to_file(object_pointer, rectangle_pointer, TEST_SIZE, "object_data", MATRIX_SIZE);
  }

  char buf[BUF_SIZE];

  for (int i = 0; i < ITER_TIME; i++) {
    memset(buf, 0, BUF_SIZE * sizeof(char));
    sprintf(buf, "object_data%d", i);
    printf("%s\n", buf);

    // movement
    for (int j = 0; j < TEST_SIZE; j++) {
      random_object_move(object_pointer + j, rectangle_pointer + j, STEP_SIZE, (float) MATRIX_SIZE);
    }

    // structs construction
    r_tree_t *r_tree = NULL;
    if (use_rtree) {
      r_tree = init_rtree();
      for (int j = 0; j < TEST_SIZE; j++) {
        insert(r_tree->root, object_pointer + j, rectangle_pointer[j]);
      }

      if (print) {
        FILE *file = fopen("tree", "w");
        tree_to_file(r_tree->root, file, 0);
        fclose(file);
      }
    }

    // search and contact
    for (int j = 0; j < TEST_SIZE; j++) {
      if (object_pointer[j].status == INFECTED) {
        object_t **search_object = NULL;

        size_t found;
        if (use_rtree) {
          found = search(r_tree->root, rectangle_pointer[j], &search_object);

        } else {
          found = scan_search(object_pointer, rectangle_pointer, TEST_SIZE, rectangle_pointer[j], &search_object);
        }

        for (int k = 0; k < found; k++) {
          contact(search_object[k], object_pointer + j, i);
        }

        free(search_object);
      }
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
    object_statistic(object_pointer, TEST_SIZE);
  }

  gettimeofday(&end, NULL);
  printf("Total time: %.3lf s\n", time_interval(start, end));
  object_statistic(object_pointer, TEST_SIZE);

  free(object_pointer);
  free(rectangle_pointer);
}