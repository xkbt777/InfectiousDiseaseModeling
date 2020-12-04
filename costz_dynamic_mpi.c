//
// Created by shawn lv on 2020-12-03.
//

#include <mpi.h>
#include "costz_dynamic_mpi.h"

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

  // communicate configuration
  int buf[2];
  if (rank_id == 0) {
    buf[0] = seed;
    buf[1] = use_rtree;
    MPI_Bcast(buf, 2, MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Bcast(buf, 2, MPI_INT, 0, MPI_COMM_WORLD);
  }

  seed = (unsigned int) buf[0];
  use_rtree = buf[1];

  // random generate use same seed
  object_t *objects = NULL;
  rectangle_t *rectangles = NULL;
  random_generate(TEST_SIZE, MATRIX_SIZE, seed, &objects, &rectangles);

  // count objects
  block_t** matrix_map = matrix_generator(BLOCK_NUM_PER_DIM, MATRIX_SIZE);
  long total_block = BLOCK_NUM_PER_DIM * BLOCK_NUM_PER_DIM;
  int count_size = total_block / world_size;
  int start_block_idx = rank_id * count_size;
  int buffer_size = rank_id == 0 ? total_block : count_size + 1;
  int count_buffer[buffer_size];
  memset(count_buffer, 0, buffer_size * sizeof(int));

  for (size_t i = 0; i < TEST_SIZE; i++) {
    int block_x, block_y;
    get_belonged_block(mid_point(rectangles[i]), BLOCK_NUM_PER_DIM, MATRIX_SIZE, &block_x, &block_y);
    int block_idx = matrix_map[block_x][block_y].id;
    if (block_idx >= start_block_idx && block_idx < start_block_idx + count_size) {
//      printf("block %.2f % .2f %.2f %.2f contains point %.2f %.2f is %d\n",
//              matrix_map[block_x][block_y].rectangle.bottom_left.x,
//              matrix_map[block_x][block_y].rectangle.bottom_left.y,
//              matrix_map[block_x][block_y].rectangle.top_right.x,
//              matrix_map[block_x][block_y].rectangle.top_right.y,
//              mid_point(rectangles[i]).x, mid_point(rectangles[i]).y,
//              if_cover_point(matrix_map[block_x][block_y].rectangle, mid_point(rectangles[i]), 1, 1, 1, 1));
      count_buffer[block_idx - start_block_idx]++;
    }
  }

  if (rank_id == 0) {
    int recv_buffer[count_size + 1];
    for (size_t i = 1; i < world_size; i++) {
      MPI_Status status;
      MPI_Recv(recv_buffer, count_size + 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      int start = recv_buffer[count_size];
      for (size_t j = 0; j < count_size; j++) {
        count_buffer[start + j] = recv_buffer[j];
      }
    }

    int obj_sum = 0;
    for (int i = 0; i < total_block; i++) {
      printf("Block %d has %d objects\n", i, count_buffer[i]);
      obj_sum += count_buffer[i];
    }
    printf("Sum: %d\n", obj_sum);
  } else {
    count_buffer[count_size] = start_block_idx;
    MPI_Send(count_buffer, buffer_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  // assign and broadcast
  int block_assignment[total_block];
  if (rank_id == 0) {
    int assign_worker_id = 0;
    size_t assign_i = 0;
    int assigned_size = 0;
    int assign_target = TEST_SIZE / world_size;
    while (assign_i < total_block && assign_worker_id < world_size - 1) {
      assigned_size += count_buffer[assign_i];
      block_assignment[assign_i] = assign_worker_id;
      printf("Block %zu is assigned to worker %d\n", assign_i, assign_worker_id);
      assign_i++;
      if (assigned_size >= assign_target || assign_i >= total_block
          || (assigned_size + count_buffer[assign_i] >= assign_target
              && assigned_size + count_buffer[assign_i] - assign_target >= assign_target - assigned_size)) {
        printf("Assign %d objects to worker %d\n", assigned_size, assign_worker_id);
        assign_worker_id++;
        assigned_size = 0;
      }
    }
    while (assign_i < total_block) {
      assigned_size += count_buffer[assign_i];
      block_assignment[assign_i] = assign_worker_id;
      printf("Block %zu is assigned to worker %d\n", assign_i, assign_worker_id);
      assign_i++;
    }
    printf("Assign %d objects to worker %d\n", assigned_size, assign_worker_id);
//    memset(block_assignment + assign_i, assign_worker_id, (total_block - assign_i) * sizeof(int));
    MPI_Bcast(block_assignment, total_block, MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Bcast(block_assignment, total_block, MPI_INT, 0, MPI_COMM_WORLD);
  }

  endTime = MPI_Wtime();

  MPI_Finalize();

  if (rank_id == 0) {
    for (int i = 0; i < total_block; i++) {
      printf("%d", block_assignment[i]);
    }
    printf("\n");
  }
  return 0;
}