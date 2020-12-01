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
  self_area.bottom_left.y = MATRIX_SIZE / (float)world_size * rank_id;
  self_area.top_right.x = (float) MATRIX_SIZE;
  self_area.top_right.y = MATRIX_SIZE / (float)world_size * (rank_id + 1);

  // gather object and rectangle into buffer
  object_t object_buffer[TEST_SIZE];
  rectangle_t rectangle_buffer[TEST_SIZE];
  int size = static_gather(object_buffer, objects, rectangle_buffer, rectangles, self_area, rank_id);
  free(objects);
  free(rectangles);
  object_statistic(object_buffer, size);

  // build r_tree if use
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
  object_t up_recv_objs[TEST_SIZE / world_size];
  int up_recv_size = 0;
  rectangle_t down_recv_buf[TEST_SIZE / world_size];
  object_t down_recv_objs[TEST_SIZE / world_size];
  int down_recv_size = 0;

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
          found = scan_search(object_buffer, rectangle_buffer, size, rectangle_buffer[i], &search_object);
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

    //update status
    status_update(object_buffer, size, iter);

    // move objects
    if (use_rtree) {
      // propagate up forward
//      random_move_rtree_objects(r_tree->root, STEP_SIZE, MATRIX_SIZE);

      // reconstruct rtree
      object_t* all_objects = NULL;
      rectangle_t* all_rectangles = NULL;
      search_with_rect(r_tree->root, r_tree->root->rectangle, &all_objects, &all_rectangles);
      free_rtree(r_tree);
      r_tree = init_rtree();
      for (size_t i = 0; i < size; i++) {
        object_buffer[i] = all_objects[i];
        rectangle_buffer[i] = all_rectangles[i];
        random_object_move(object_buffer + i, rectangle_buffer + i, STEP_SIZE, (float) MATRIX_SIZE);
        insert(r_tree->root, object_buffer + i, rectangle_buffer[i]);
      }
      free(all_objects);
      free(all_rectangles);
    } else {
      for (size_t i = 0; i < size; i++) {
        random_object_move(object_buffer + i, rectangle_buffer + i, STEP_SIZE, (float) MATRIX_SIZE);
      }
    }

    // pick out outsiders
    object_t* up_send_objs = NULL;
    object_t* down_send_objs = NULL;
    rectangle_t* up_send_rects = NULL;
    rectangle_t* down_send_rects = NULL;
    if (use_rtree) {
      if (rank_id < world_size - 1) {
        rectangle_t up = init(0, self_area.top_right.y + HALF_INFECT_ZONE_LENGTH, MATRIX_SIZE, MATRIX_SIZE);
        up_send_size = search_with_rect(r_tree->root, up, &up_send_objs, &up_send_rects);
        for (size_t j = 0; j < up_send_size; j++) {
          delete(r_tree->root, up_send_objs + j, up_send_rects[j]);
        }
       } else {
         up_send_size = 0;
      }

      if (rank_id > 0) {
        rectangle_t down = init(0, 0, MATRIX_SIZE, self_area.bottom_left.y - HALF_INFECT_ZONE_LENGTH);
        down_send_size = search_with_rect(r_tree->root, down, &down_send_objs, &down_send_rects);
        for (size_t j = 0; j < down_send_size; j++) {
          delete(r_tree->root, down_send_objs + j, down_send_rects[j]);
        }
      } else {
        down_send_size = 0;
      }

      size -= (up_send_size + down_send_size);
    } else {
      up_send_size = 0;
      down_send_size = 0;
      up_send_objs = (object_t*) malloc(size * sizeof(object_t));
      down_send_objs = (object_t*) malloc(size * sizeof(object_t));
      up_send_rects  = (rectangle_t*) malloc(size * sizeof(rectangle_t));
      down_send_rects = (rectangle_t*) malloc(size * sizeof(rectangle_t));
      int left_mask = 1;
      int right_mask = 1;
      int top_mask = rank_id == 0 ? 1 : 0;
      int bottom_mask = 1;
      size_t total_rev = 0;
      size_t* removed_idx = (size_t*) calloc(size, sizeof(size_t));

      for (size_t j = 0; j < size; j++) {
        point_t center = mid_point(rectangle_buffer[j]);
        if (!if_cover_point(self_area, center, left_mask, bottom_mask, right_mask, top_mask)) {
          if (if_above_point(self_area, center)) {
            up_send_objs[up_send_size] = object_buffer[j];
            up_send_rects[up_send_size++] = rectangle_buffer[j];
          } else {
            down_send_objs[down_send_size] = object_buffer[j];
            down_send_rects[down_send_size++] = rectangle_buffer[j];
          }
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

      MPI_Barrier(MPI_COMM_WORLD);
      // object reallocate
      if (rank_id % 2 == 0) {

          // down send and recv
          if (rank_id > 0) {
              MPI_Send(down_send_rects, 4 * down_send_size, MPI_FLOAT, rank_id - 1, 0, MPI_COMM_WORLD);
              MPI_Send(down_send_objs, 4 * down_send_size, MPI_UNSIGNED, rank_id - 1, 0, MPI_COMM_WORLD);
//              printf("%d sends %zu objects to %d at iteration %d\n", rank_id, down_send_size, rank_id - 1, iter);

              MPI_Status status;
              MPI_Recv(down_recv_buf, 4 * TEST_SIZE / world_size, MPI_FLOAT, rank_id - 1, 0, MPI_COMM_WORLD, &status);
              MPI_Get_count(&status, MPI_FLOAT, &down_recv_size);
              down_recv_size = down_recv_size / 4;
              MPI_Recv(down_recv_objs, 4 * down_recv_size, MPI_UNSIGNED, rank_id - 1, 0, MPI_COMM_WORLD, &status);
//            printf("%d receives %d objects to %d at iteration %d\n", rank_id, down_recv_size, rank_id - 1, iter);
          }

          // up send and recv
          if (rank_id < world_size - 1) {
              MPI_Send(up_send_rects, 4 * up_send_size, MPI_FLOAT, rank_id + 1, 0, MPI_COMM_WORLD);
              MPI_Send(up_send_objs, 4 * up_send_size, MPI_UNSIGNED, rank_id + 1, 0, MPI_COMM_WORLD);
//              printf("%d sends %zu objects to %d at iteration %d\n", rank_id, up_send_size, rank_id + 1, iter);

              MPI_Status status;
              MPI_Recv(up_recv_buf, 4 * TEST_SIZE / world_size, MPI_FLOAT, rank_id + 1, 0, MPI_COMM_WORLD, &status);
              MPI_Get_count(&status, MPI_FLOAT, &up_recv_size);
              up_recv_size = up_recv_size / 4;
              MPI_Recv(up_recv_objs, 4 * up_recv_size, MPI_UNSIGNED, rank_id + 1, 0, MPI_COMM_WORLD, &status);
//            printf("%d receives %d objects to %d at iteration %d\n", rank_id, up_recv_size, rank_id + 1, iter);
          }
      } else {
          // up recv and send
          if (rank_id < world_size - 1) {
              MPI_Status status;
              MPI_Recv(up_recv_buf, 4 * TEST_SIZE / world_size, MPI_FLOAT, rank_id + 1, 0, MPI_COMM_WORLD, &status);
              MPI_Get_count(&status, MPI_FLOAT, &up_recv_size);
              up_recv_size = up_recv_size / 4;
              MPI_Recv(down_recv_objs, 4 * up_recv_size, MPI_UNSIGNED, rank_id + 1, 0, MPI_COMM_WORLD, &status);
//            printf("%d receives %d objects to %d at iteration %d\n", rank_id, up_recv_size, rank_id + 1, iter);

              MPI_Send(up_send_rects, 4 * up_send_size, MPI_FLOAT, rank_id + 1, 0, MPI_COMM_WORLD);
              MPI_Send(up_send_objs, 4 * up_send_size, MPI_UNSIGNED, rank_id + 1, 0, MPI_COMM_WORLD);
//            printf("%d sends %zu objects to %d at iteration %d\n", rank_id, up_send_size, rank_id + 1, iter);
          }

          // down recv and send
          if (rank_id > 0) {
              MPI_Status status;
              MPI_Recv(down_recv_buf, 4 * TEST_SIZE / world_size, MPI_FLOAT, rank_id - 1, 0, MPI_COMM_WORLD, &status);
              MPI_Get_count(&status, MPI_FLOAT, &down_recv_size);
              down_recv_size = down_recv_size / 4;
              MPI_Recv(down_recv_objs, 4 * down_recv_size, MPI_UNSIGNED, rank_id - 1, 0, MPI_COMM_WORLD, &status);
//            printf("%d receives %d objects to %d at iteration %d\n", rank_id, down_recv_size, rank_id - 1, iter);

              MPI_Send(down_send_rects, 4 * down_send_size, MPI_FLOAT, rank_id - 1, 0, MPI_COMM_WORLD);
              MPI_Send(down_send_objs, 4 * down_send_size, MPI_UNSIGNED, rank_id - 1, 0, MPI_COMM_WORLD);
//            printf("%d sends %d objects to %zu at iteration %d\n", rank_id, down_send_size, rank_id - 1, iter);
          }
      }

      free(up_send_objs);
      free(down_send_objs);
      free(up_send_rects);
      free(down_send_rects);
      MPI_Barrier(MPI_COMM_WORLD);

      // add received objects
      for (size_t i = 0; i < up_recv_size; i++) {
          if (use_rtree) {
              insert(r_tree->root, up_recv_objs + i, up_recv_buf[i]);
              size++;
          } else {
              object_buffer[size] = up_recv_objs[i];
              rectangle_buffer[size++] = up_recv_buf[i];
          }
      }
      for (size_t i = 0; i < down_recv_size; i++) {
          if (use_rtree) {
              insert(r_tree->root, down_recv_objs + i, down_recv_buf[i]);
              size++;
          } else {
              object_buffer[size] = down_recv_objs[i];
              rectangle_buffer[size++] = down_recv_buf[i];
          }
      }
    // object_statistic(object_buffer, size);
  }

  // gather status
  int status_buffer[TEST_SIZE];
  if (rank_id == 0) {
    for (int i = 1; i < world_size; i++) {
      memset(status_buffer, 0, TEST_SIZE * sizeof(int));
      MPI_Status status;
      int recv_size = 0;
      MPI_Recv(status_buffer, TEST_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_INT, &recv_size);

      for (int j = 0; j < recv_size; j++) {
        object_buffer[size++].status = status_buffer[j];
      }
    }
  } else {
    memset(status_buffer, 0, BUF_SIZE * sizeof(int));
    for (int i = 0; i < size; i++) {
      status_buffer[i] = object_buffer[i].status;
    }
    MPI_Send(status_buffer, size, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  if (rank_id == 0) {
    object_statistic(object_buffer, TEST_SIZE);
  }
  endTime = MPI_Wtime();

  MPI_Finalize();
  if (rank_id == 0) {
    printf("Total time used: %f for proc: %d, size: %d\n", endTime - startTime, rank_id, size);
  }
  return 0;

}
