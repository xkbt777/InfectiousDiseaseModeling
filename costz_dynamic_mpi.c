//
// Created by shawn lv on 2020-12-03.
//

#include <mpi.h>
#include "costz_dynamic_mpi.h"

int main(int argc, char* argv[]) {

  unsigned int seed = time(NULL);
  int opt;
  int use_rtree = 0, use_normal = 0;
  while ((opt = getopt(argc, argv, "rns:")) != -1) {
    switch (opt) {
      case 'r':
        use_rtree = 1;
        break;
      case 'n':
        use_normal = 1;
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
  int buf[BUF_SIZE];
  if (rank_id == 0) {
    buf[0] = seed;
    buf[1] = use_rtree;
    buf[2] = use_normal;
    MPI_Bcast(buf, 3, MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Bcast(buf, 3, MPI_INT, 0, MPI_COMM_WORLD);
  }

  seed = (unsigned int) buf[0];
  use_rtree = buf[1];
  use_normal = buf[2];

  // random generate use same seed
  object_t *objects = NULL;
  rectangle_t *rectangles = NULL;

  if (use_normal) {
    center_generate(TEST_SIZE, MATRIX_SIZE, CENTER_X, CENTER_Y, SIGMA, seed, &objects, &rectangles);
  } else {
    random_generate(TEST_SIZE, MATRIX_SIZE, seed, &objects, &rectangles);
  }

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
//        printf("Block %d has %d objects\n", start + j, count_buffer[start + j]);
      }
    }
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
//      printf("Block %zu is assigned to worker %d\n", assign_i, assign_worker_id);
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
//      printf("Block %zu is assigned to worker %d\n", assign_i, assign_worker_id);
      assign_i++;
    }
    printf("Assign %d objects to worker %d\n", assigned_size, assign_worker_id);
    MPI_Bcast(block_assignment, total_block, MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Bcast(block_assignment, total_block, MPI_INT, 0, MPI_COMM_WORLD);
  }

  // rtree init
  r_tree_t* r_tree = NULL;
  if (use_rtree) {
    r_tree = init_rtree();
  }

  // get assigned objects
  int start_index = 0;
  int block_size = 0;
  while (block_assignment[start_index] != rank_id) {
    start_index++;
  }
  while (block_assignment[start_index + block_size] == rank_id) {
    block_size++;
  }

  object_t object_buffer[TEST_SIZE];
  rectangle_t rectangle_buffer[TEST_SIZE];
  int size = 0;
  for (size_t i = 0; i < TEST_SIZE; i++) {
    int block_x, block_y;
    get_belonged_block(mid_point(rectangles[i]), BLOCK_NUM_PER_DIM, MATRIX_SIZE, &block_x, &block_y);
    int block_idx = matrix_map[block_x][block_y].id;
    if (block_idx >= start_index && block_idx < start_index + block_size) {
      object_buffer[size] = objects[i];
      rectangle_buffer[size] = rectangles[i];
      if (use_rtree) {
        insert(r_tree->root, object_buffer + size, rectangle_buffer[size]);
      }
      size++;
    }
  }



  for (size_t iter = 0; iter < ITER_TIME; iter++) {
    // move objects
    if (use_rtree) {
      random_move_rtree_objects(r_tree->root, STEP_SIZE, MATRIX_SIZE);
    } else {
      for (size_t i = 0; i < size; i++) {
        random_object_move(object_buffer + i, rectangle_buffer + i, STEP_SIZE, MATRIX_SIZE);
      }
    }

    // pick out outsiders
    object_t send_objs[world_size][size];
    rectangle_t send_rects[world_size][size];
    int send_size[world_size];
    memset(send_size, 0, world_size * sizeof(int));
    if (use_rtree) {
      for (size_t i = 0; i < BLOCK_NUM_PER_DIM; i++) {
        for (size_t j = 0; j < BLOCK_NUM_PER_DIM; j++) {
          if (matrix_map[i][j].id < start_index || matrix_map[i][j].id >= start_index + block_size) {
            int worker = block_assignment[matrix_map[i][j].id];
            object_t* search_obj = NULL;
            rectangle_t* search_rec = NULL;
            size_t search_size = search_with_rect(r_tree->root, matrix_map[i][j].rectangle, &search_obj, &search_rec);
            for (size_t k = 0; k < search_size; k++) {
              send_objs[worker][send_size[worker]] = search_obj[k];
              send_rects[worker][send_size[worker]] = search_rec[k];
              send_size[worker]++;
              delete(r_tree->root, search_obj + k, search_rec[k]);
            }
            free(search_obj);
            free(search_rec);
            size -= search_size;
          }
        }
      }
    } else {
      size_t total_rev = 0;
      size_t* removed_idx = (size_t*) calloc(size, sizeof(size_t));

      for (size_t j = 0; j < size; j++) {
        int block_x, block_y;
        get_belonged_block(mid_point(rectangle_buffer[j]), BLOCK_NUM_PER_DIM, MATRIX_SIZE, &block_x, &block_y);
        int block_idx = matrix_map[block_x][block_y].id;
        if (block_idx < start_index || block_idx >= start_index + block_size) {
          int worker = block_assignment[block_idx];
          send_objs[worker][send_size[worker]] = object_buffer[j];
          send_rects[worker][send_size[worker]] = rectangle_buffer[j];
          send_size[worker]++;
          removed_idx[total_rev++] = j;
        }
      }

      // remove objects and rectangles from the buffer
      int full_pointer = size - 1;
      for (size_t j = 0; j < total_rev; j++) {
        while (full_pointer == removed_idx[total_rev - j] && full_pointer >= 0) {
          full_pointer--;
        }

        if (full_pointer < 0 || full_pointer < removed_idx[j]) {
          break;
        }

        object_buffer[removed_idx[j]] = object_buffer[full_pointer];
        rectangle_buffer[removed_idx[j]] = rectangle_buffer[full_pointer--];
      }
      size -= total_rev;
    }

    // exchange objects
    for (size_t i = 0; i < rank_id; i++) {
      int recv_size;
      MPI_Status status;
      MPI_Recv(rectangle_buffer + size, 4 * TEST_SIZE, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_FLOAT, &recv_size);
      recv_size = recv_size / 4;
      MPI_Recv(object_buffer + size, 4 * recv_size, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);

      if (use_rtree) {
        for (size_t j = 0; j < recv_size; j++) {
          insert(r_tree->root, object_buffer + size + j, rectangle_buffer[size + j]);
        }
      }

      size += recv_size;
    }
    for (size_t j = 0; j < world_size; j++) {
      if (j != rank_id) {
        MPI_Send(send_rects[j], send_size[j] * 4, MPI_FLOAT, j, 0, MPI_COMM_WORLD);
        MPI_Send(send_objs[j], send_size[j] * 4, MPI_UNSIGNED, j, 0, MPI_COMM_WORLD);
      }
    }
    for (size_t i = rank_id + 1; i < world_size; i++) {
      int recv_size;
      MPI_Status status;
      MPI_Recv(rectangle_buffer + size, 4 * TEST_SIZE, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_FLOAT, &recv_size);
      recv_size = recv_size / 4;
      MPI_Recv(object_buffer + size, 4 * recv_size, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);

      if (use_rtree) {
        for (size_t j = 0; j < recv_size; j++) {
          insert(r_tree->root, object_buffer + size + j, rectangle_buffer[size + j]);
        }
      }

      size += recv_size;
    }
  }

  // gather objects
  if (rank_id == 0) {
    if (use_rtree) {
      object_t* all_objects = NULL;
      rectangle_t* all_rectangles = NULL;
      search_with_rect(r_tree->root, r_tree->root->rectangle, &all_objects, &all_rectangles);
      for (size_t i = 0; i < size; i++) {
        object_buffer[i] = all_objects[i];
        rectangle_buffer[i] = all_rectangles[i];
      }
      free(all_objects);
      free(all_rectangles);
    }

    for (int i = 1; i < world_size; i++) {
      int recv_size;
      MPI_Status status;
      MPI_Recv(rectangle_buffer + size, 4 * TEST_SIZE, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_FLOAT, &recv_size);
      recv_size = recv_size / 4;
      MPI_Recv(object_buffer + size, 4 * recv_size, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);
      size += recv_size;
    }
  } else {
    if (use_rtree) {
      object_t* all_objects = NULL;
      rectangle_t* all_rectangles = NULL;
      search_with_rect(r_tree->root, r_tree->root->rectangle, &all_objects, &all_rectangles);
      MPI_Send(all_rectangles, size * 4, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
      MPI_Send(all_objects, size * 4, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);
      free(all_objects);
      free(all_rectangles);
    } else {
      MPI_Send(rectangle_buffer, size * 4, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
      MPI_Send(object_buffer, size * 4, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);
    }
  }

  endTime = MPI_Wtime();

  MPI_Finalize();

  if (rank_id == 0) {
    printf("Total time used: %f for proc: %d, size: %d\n", endTime - startTime, rank_id, size);
  }
  return 0;
}