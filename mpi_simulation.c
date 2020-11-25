//
// Created by shawn lv on 2020-11-25.
//

#include <mpi.h>
#include "mpi_simulation.h"

int static_gather(int *indexes, rectangle_t *rectangles, rectangle_t target, int rank_id) {
  int size = 0;

  int left_mask = 1;
  int right_mask = 1;
  int top_mask = 0;
  int bottom_mask = 1;

  if (rank_id == 0) {
    top_mask = 1;
  }

  for (int i = 0; i < TEST_SIZE; i++) {
    if (if_cover_point(target, mid_point(rectangles[i]), left_mask, bottom_mask, right_mask, top_mask)) {
      indexes[size] = i;
      size += 1;
    }
  }
  return size;
}

int main(int argc, char* argv[]) {

  unsigned int seed = time(NULL);
  int opt;
  int use_rtree = 0;
  while ((opt = getopt(argc, argv, "rs:")) != -1) {
    switch (opt) {
      case 'r':
        use_rtree = 1;
        printf("Use r_tree\n");
        break;
      case 's':
        seed = atoi(optarg);
        break;
    }
  }

  double startTime, endTime;

  // Initialize the MPI environment
  MPI_Init(NULL, NULL);

  // Get rank id
  int rank_id;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank_id);

  // Get total number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  startTime = MPI_Wtime();

  int buf[BUF_SIZE];

  // communicate configuration
  if (rank_id == 0) {
    buf[0] = seed;
    buf[1] = use_rtree;
    MPI_Bcast(buf, 2, MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Bcast(buf, 2, MPI_INT, 0, MPI_COMM_WORLD);
  }

  seed = (unsigned int) buf[0];
  use_rtree = buf[1];

  object_t *objects = NULL;
  rectangle_t *rectangles = NULL;

  random_generate(TEST_SIZE, MATRIX_SIZE, seed, &objects, &rectangles);

  int indexes[TEST_SIZE];

  rectangle_t self_area;
  self_area.bottom_left.x = 0.0f;
  self_area.bottom_left.y = MATRIX_SIZE / world_size * rank_id;

  self_area.top_right.x = (float) MATRIX_SIZE;
  self_area.top_right.y = MATRIX_SIZE / world_size * (rank_id + 1);

  int size = static_gather(indexes, rectangles, self_area, rank_id);

  if (rank_id == 0) {
    for (int i = 1; i < world_size; i++) {
      memset(buf, 0, BUF_SIZE * sizeof(int));
      MPI_Recv(buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      size += buf[0];
    }
  } else {
    memset(buf, 0, BUF_SIZE * sizeof(int));
    buf[0] = size;
    MPI_Send(buf, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  endTime = MPI_Wtime();

  MPI_Finalize();
  printf("Total time used: %f for proc: %d, size: %d\n", endTime - startTime, rank_id, size);
  return 0;

}