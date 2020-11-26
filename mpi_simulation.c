//
// Created by shawn lv on 2020-11-25.
//

#include <mpi.h>
#include "mpi_simulation.h"

int static_gather(object_t *dst, object_t *src, rectangle_t *rectangle_dst, rectangle_t *rectangle_src, rectangle_t target, int rank_id) {
  int size = 0;

  int left_mask = 1;
  int right_mask = 1;
  int top_mask = 0;
  int bottom_mask = 1;

  if (rank_id == 0) {
    top_mask = 1;
  }

  for (int i = 0; i < TEST_SIZE; i++) {
    if (if_cover_point(target, mid_point(rectangle_src[i]), left_mask, bottom_mask, right_mask, top_mask)) {
      rectangle_dst[size] = rectangle_src[i];
      dst[size] = src[i];
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


  // random generate use same seed
  object_t *objects = NULL;
  rectangle_t *rectangles = NULL;
  random_generate(TEST_SIZE, MATRIX_SIZE, seed, &objects, &rectangles);

  // found local area
  rectangle_t self_area;
  self_area.bottom_left.x = 0.0f;
  self_area.bottom_left.y = MATRIX_SIZE / world_size * rank_id;
  self_area.top_right.x = (float) MATRIX_SIZE;
  self_area.top_right.y = MATRIX_SIZE / world_size * (rank_id + 1);

  // gather object and rectangle into buffer
  object_t object_buffer[TEST_SIZE];
  rectangle_t rectangle_buffer[TEST_SIZE];
  int size = static_gather(object_buffer, objects, rectangle_buffer, rectangles, self_area, rank_id);
  free(objects);
  free(rectangles);

  // build r_tree
  r_tree_t *r_tree = NULL;
  if (use_rtree) {
    r_tree = init_rtree();
    for (int i = 0; i < size; i++) {
      insert(r_tree->root, object_buffer + i, rectangle_buffer[i]);
    }
  }

  // found static ghost zone
  rectangle_t up_ghost_zone = init(0, self_area.top_right.y - HALF_INFECT_ZONE_LENGTH, (float) MATRIX_SIZE, self_area.top_right.y);
  rectangle_t down_ghost_zone = init(0, self_area.bottom_left.y, (float) MATRIX_SIZE, self_area.bottom_left.y + HALF_INFECT_ZONE_LENGTH);
  rectangle_t *up_send_pointer = NULL;
  size_t up_send_size = 0;
  rectangle_t *down_send_pointer = NULL;
  size_t down_send_size = 0;
  rectangle_t up_recv_buf[TEST_SIZE / world_size];
  int up_recv_size = 0;
  rectangle_t down_recv_buf[TEST_SIZE / world_size];
  int down_recv_size = 0;

  printf("S: %d, F: %d, Size: %d\n", seed, use_rtree, size);
  for (int iter = 0; iter < ITER_TIME; iter++) {

    // search for element in ghost zone
    if (rank_id > 0) {
      if (use_rtree) {
        down_send_size = search_infect_rec(r_tree->root, down_ghost_zone, &down_send_pointer);
      } else {
        down_send_size = scan_search_infect_rec(object_buffer, rectangle_buffer, size, down_ghost_zone, &down_send_pointer);
      }
    }

    if (rank_id < world_size - 1) {
      if (use_rtree) {
        up_send_size = search_infect_rec(r_tree->root, up_ghost_zone, &up_send_pointer);
      } else {
        up_send_size = scan_search_infect_rec(object_buffer, rectangle_buffer, size, up_ghost_zone, &up_send_pointer);
      }
    }

    // ghost area communication
    if (rank_id % 2 == 0) {

      // down send and recv
      if (rank_id > 0) {
        MPI_Send(down_send_pointer, 4 * down_send_size, MPI_FLOAT, rank_id - 1, 0, MPI_COMM_WORLD);

        memset(down_recv_buf, 0, TEST_SIZE / world_size * sizeof(rectangle_t));
        MPI_Status status;
        MPI_Recv(down_recv_buf, 4 * TEST_SIZE / world_size, MPI_FLOAT, rank_id - 1, 0, MPI_COMM_WORLD,
                 &status);

        MPI_Get_count(&status, MPI_FLOAT, &down_recv_size);
        down_recv_size = down_recv_size / 4;
      }

      // up send and recv
      if (rank_id < world_size - 1) {
        MPI_Send(up_send_pointer, 4 * up_send_size, MPI_FLOAT, rank_id + 1, 0, MPI_COMM_WORLD);

        memset(up_recv_buf, 0, TEST_SIZE / world_size * sizeof(rectangle_t));
        MPI_Status status;
        MPI_Recv(up_recv_buf, 4 * TEST_SIZE / world_size, MPI_FLOAT, rank_id + 1, 0, MPI_COMM_WORLD,
                 &status);

        MPI_Get_count(&status, MPI_FLOAT, &up_recv_size);
        up_recv_size = up_recv_size / 4;
      }
    } else {
      // up recv and send
      if (rank_id < world_size - 1) {
        memset(up_recv_buf, 0, TEST_SIZE / world_size * sizeof(rectangle_t));
        MPI_Status status;
        MPI_Recv(up_recv_buf, 4 * TEST_SIZE / world_size, MPI_FLOAT, rank_id + 1, 0, MPI_COMM_WORLD,
                 &status);

        MPI_Get_count(&status, MPI_FLOAT, &up_recv_size);
        up_recv_size = up_recv_size / 4;

        MPI_Send(up_send_pointer, 4 * up_send_size, MPI_FLOAT, rank_id + 1, 0, MPI_COMM_WORLD);
      }

      // down recv and send
      if (rank_id > 0) {
        memset(down_recv_buf, 0, TEST_SIZE / world_size * sizeof(rectangle_t));
        MPI_Status status;
        MPI_Recv(down_recv_buf, 4 * TEST_SIZE / world_size, MPI_FLOAT, rank_id - 1, 0, MPI_COMM_WORLD,
                 &status);

        MPI_Get_count(&status, MPI_FLOAT, &down_recv_size);
        down_recv_size = down_recv_size / 4;

        MPI_Send(down_send_pointer, 4 * down_send_size, MPI_FLOAT, rank_id - 1, 0, MPI_COMM_WORLD);
      }
    }

    free(up_send_pointer);
    free(down_send_pointer);


    // contact local
    for (int i = 0; i < size; i++) {
      if (object_buffer[i].status == INFECTED) {
        object_t **search_object = NULL;

        size_t found;
        if (use_rtree) {
          found = search(r_tree->root, rectangle_buffer[i], &search_object);
        } else {
          found = scan_search(object_buffer, rectangle_buffer, TEST_SIZE, rectangle_buffer[i], &search_object);
        }

        for (int k = 0; k < found; k++) {
          contact(search_object[k], object_buffer + i, iter);
        }

        free(search_object);
      }
    }

    object_t infected_object;
    infected_object.status = INFECTED;

    // contact down
    if (rank_id > 0) {
      for (int i = 0; i < down_recv_size; i++) {
        object_t **search_object = NULL;

        size_t found;
        if (use_rtree) {
          found = search(r_tree->root, down_recv_buf[i], &search_object);
        } else {
          found = scan_search(object_buffer, rectangle_buffer, size, down_recv_buf[i], &search_object);
        }

        for (int k = 0; k < found; k++) {
          contact(search_object[k], &infected_object, iter);
        }

        free(search_object);
      }
    }

    // contact up
    if (rank_id < world_size - 1) {
      for (int i = 0; i < up_recv_size; i++) {
        object_t **search_object = NULL;

        size_t found;
        if (use_rtree) {
          found = search(r_tree->root, up_recv_buf[i], &search_object);
        } else {
          found = scan_search(object_buffer, rectangle_buffer, size, up_recv_buf[i], &search_object);
        }

        for (int k = 0; k < found; k++) {
          contact(search_object[k], &infected_object, iter);
        }

        free(search_object);
      }
    }



  }


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