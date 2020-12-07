//
// Created by shawn lv on 2020-12-03.
//

#include <mpi.h>
#include "costz_dynamic_mpi.h"

void found_all_adjacent(block_t **matrix_map, int *block_assignment, int rank_id, int world_size, int *rec_num, rectangle_t ***ghost_zones) {
  *ghost_zones = (rectangle_t **) calloc(world_size, sizeof(rectangle_t *));

  for (int i = 0; i < world_size; i++) {
    rec_num[i] = 0;
    (*ghost_zones)[i] = (rectangle_t *) calloc(BUF_SIZE, sizeof(rectangle_t));
  }

  for (int r = 0; r < BLOCK_NUM_PER_DIM; r++) {
    for (int c = 0; c < BLOCK_NUM_PER_DIM; c++) {
      if (block_assignment[matrix_map[r][c].id] == rank_id) {

        // search top
        if (r < BLOCK_NUM_PER_DIM - 1 && block_assignment[matrix_map[r + 1][c].id] != rank_id) {
          int adjacent_id = block_assignment[matrix_map[r + 1][c].id];
          rectangle_t ghost_zone = init(matrix_map[r][c].rectangle.bottom_left.x,
                                        matrix_map[r][c].rectangle.top_right.y - HALF_INFECT_ZONE_LENGTH,
                                        matrix_map[r][c].rectangle.top_right.x,
                                        matrix_map[r][c].rectangle.top_right.y);
          (*ghost_zones)[adjacent_id][rec_num[adjacent_id]] = ghost_zone;
          rec_num[adjacent_id] += 1;
        }

        // search bottom
        if (r > 0 && block_assignment[matrix_map[r - 1][c].id] != rank_id) {
          int adjacent_id = block_assignment[matrix_map[r - 1][c].id];
          // printf("bottom relation at %d %d, %d, %d\n", r, c, rank_id, adjacent_id);
          rectangle_t ghost_zone = init(matrix_map[r][c].rectangle.bottom_left.x,
                                        matrix_map[r][c].rectangle.bottom_left.y,
                                        matrix_map[r][c].rectangle.top_right.x,
                                        matrix_map[r][c].rectangle.bottom_left.y + HALF_INFECT_ZONE_LENGTH);
          (*ghost_zones)[adjacent_id][rec_num[adjacent_id]] = ghost_zone;
          rec_num[adjacent_id] += 1;
        }

        // search right
        if (c < BLOCK_NUM_PER_DIM - 1 && block_assignment[matrix_map[r][c + 1].id] != rank_id) {
          int adjacent_id = block_assignment[matrix_map[r][c + 1].id];
          // printf("right relation at %d %d, %d, %d\n", r, c, rank_id, adjacent_id);
          rectangle_t ghost_zone = init(matrix_map[r][c].rectangle.top_right.x - HALF_INFECT_ZONE_LENGTH,
                                        matrix_map[r][c].rectangle.bottom_left.y,
                                        matrix_map[r][c].rectangle.top_right.x,
                                        matrix_map[r][c].rectangle.top_right.y);
          (*ghost_zones)[adjacent_id][rec_num[adjacent_id]] = ghost_zone;
          rec_num[adjacent_id] += 1;
        }

        // search left
        if (c > 0 && block_assignment[matrix_map[r][c - 1].id] != rank_id) {
          int adjacent_id = block_assignment[matrix_map[r][c - 1].id];
          //printf("left relation at %d %d, %d, %d\n", r, c, rank_id, adjacent_id);
          rectangle_t ghost_zone = init(matrix_map[r][c].rectangle.bottom_left.x,
                                        matrix_map[r][c].rectangle.bottom_left.y,
                                        matrix_map[r][c].rectangle.bottom_left.x + HALF_INFECT_ZONE_LENGTH,
                                        matrix_map[r][c].rectangle.top_right.y);
          (*ghost_zones)[adjacent_id][rec_num[adjacent_id]] = ghost_zone;
          rec_num[adjacent_id] += 1;
        }

        // search topRight
        if (r < BLOCK_NUM_PER_DIM - 1 && c < BLOCK_NUM_PER_DIM - 1 &&
            block_assignment[matrix_map[r + 1][c + 1].id] != rank_id) {
          int adjacent_id = block_assignment[matrix_map[r + 1][c + 1].id];

          if (adjacent_id != block_assignment[matrix_map[r][c + 1].id] &&
            adjacent_id != block_assignment[matrix_map[r + 1][c].id] &&
            rank_id != block_assignment[matrix_map[r][c + 1].id] &&
            rank_id != block_assignment[matrix_map[r + 1][c].id]) {
            // printf("topRIght relation at %d %d, %d, %d\n", r, c, rank_id, adjacent_id);
            rectangle_t ghost_zone = init(matrix_map[r][c].rectangle.top_right.x - HALF_INFECT_ZONE_LENGTH,
                                          matrix_map[r][c].rectangle.top_right.y - HALF_INFECT_ZONE_LENGTH,
                                          matrix_map[r][c].rectangle.top_right.x,
                                          matrix_map[r][c].rectangle.top_right.y);
            (*ghost_zones)[adjacent_id][rec_num[adjacent_id]] = ghost_zone;
            rec_num[adjacent_id] += 1;
          }
        }

        // search bottomRight
        if (r > 0 && c < BLOCK_NUM_PER_DIM - 1 && block_assignment[matrix_map[r - 1][c + 1].id] != rank_id) {
          int adjacent_id = block_assignment[matrix_map[r - 1][c + 1].id];

          if (adjacent_id != block_assignment[matrix_map[r - 1][c].id] &&
              adjacent_id != block_assignment[matrix_map[r][c + 1].id] &&
              rank_id != block_assignment[matrix_map[r - 1][c].id] &&
              rank_id != block_assignment[matrix_map[r][c + 1].id]) {
            // printf("bottomRIght relation at %d %d, %d, %d\n", r, c, rank_id, adjacent_id);
            rectangle_t ghost_zone = init(matrix_map[r][c].rectangle.top_right.x - HALF_INFECT_ZONE_LENGTH,
                                          matrix_map[r][c].rectangle.bottom_left.y,
                                          matrix_map[r][c].rectangle.top_right.x,
                                          matrix_map[r][c].rectangle.bottom_left.y + HALF_INFECT_ZONE_LENGTH);
            (*ghost_zones)[adjacent_id][rec_num[adjacent_id]] = ghost_zone;
            rec_num[adjacent_id] += 1;
          }
        }

        // search topLeft
        if (r < BLOCK_NUM_PER_DIM - 1 && c > 0 && block_assignment[matrix_map[r + 1][c - 1].id] != rank_id) {
          int adjacent_id = block_assignment[matrix_map[r + 1][c - 1].id];
          // printf("topLeft relation at %d %d, %d, %d\n", r, c, rank_id, adjacent_id);
          if (adjacent_id != block_assignment[matrix_map[r + 1][c].id] &&
              adjacent_id != block_assignment[matrix_map[r][c - 1].id] &&
              rank_id != block_assignment[matrix_map[r + 1][c].id] &&
              rank_id != block_assignment[matrix_map[r][c - 1].id]) {
            rectangle_t ghost_zone = init(matrix_map[r][c].rectangle.bottom_left.x,
                                          matrix_map[r][c].rectangle.top_right.y - HALF_INFECT_ZONE_LENGTH,
                                          matrix_map[r][c].rectangle.bottom_left.x + HALF_INFECT_ZONE_LENGTH,
                                          matrix_map[r][c].rectangle.top_right.y);
            (*ghost_zones)[adjacent_id][rec_num[adjacent_id]] = ghost_zone;
            rec_num[adjacent_id] += 1;
          }
        }

        // search bottomLeft
        if (r > 0 && c > 0 && block_assignment[matrix_map[r - 1][c - 1].id] != rank_id) {
          int adjacent_id = block_assignment[matrix_map[r - 1][c - 1].id];
          //printf("bottomLEft relation at %d %d, %d, %d\n", r, c, rank_id, adjacent_id);
          if (adjacent_id != block_assignment[matrix_map[r - 1][c].id] &&
              adjacent_id != block_assignment[matrix_map[r][c - 1].id] &&
              rank_id != block_assignment[matrix_map[r - 1][c].id] &&
              rank_id != block_assignment[matrix_map[r][c - 1].id]) {
            rectangle_t ghost_zone = init(matrix_map[r][c].rectangle.bottom_left.x,
                                          matrix_map[r][c].rectangle.bottom_left.y,
                                          matrix_map[r][c].rectangle.bottom_left.x + HALF_INFECT_ZONE_LENGTH,
                                          matrix_map[r][c].rectangle.bottom_left.y + HALF_INFECT_ZONE_LENGTH);
            (*ghost_zones)[adjacent_id][rec_num[adjacent_id]] = ghost_zone;
            rec_num[adjacent_id] += 1;
          }
        }
      }
    }
  }
}

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

  if (rank_id == 0) {
    object_statistic(objects, TEST_SIZE);
  }

  // count objects
  block_t** matrix_map = matrix_generator(BLOCK_NUM_PER_DIM, MATRIX_SIZE);
  long total_block = BLOCK_NUM_PER_DIM * BLOCK_NUM_PER_DIM;
  int count_size = TEST_SIZE / world_size;
  int start_object_idx = rank_id * count_size;
  int buffer_size = total_block;
  int count_buffer[buffer_size];
  memset(count_buffer, 0, buffer_size * sizeof(int));
  count_size = rank_id == world_size - 1 ? TEST_SIZE - count_size * (world_size - 1) : count_size;

  for (size_t i = start_object_idx; i < start_object_idx + count_size; i++) {
    int block_x, block_y;
    get_belonged_block(mid_point(rectangles[i]), BLOCK_NUM_PER_DIM, MATRIX_SIZE, &block_x, &block_y);
    count_buffer[matrix_map[block_x][block_y].id]++;
//      printf("block %.2f % .2f %.2f %.2f contains point %.2f %.2f is %d\n",
//              matrix_map[block_x][block_y].rectangle.bottom_left.x,
//              matrix_map[block_x][block_y].rectangle.bottom_left.y,
//              matrix_map[block_x][block_y].rectangle.top_right.x,
//              matrix_map[block_x][block_y].rectangle.top_right.y,
//              mid_point(rectangles[i]).x, mid_point(rectangles[i]).y,
//              if_cover_point(matrix_map[block_x][block_y].rectangle, mid_point(rectangles[i]), 1, 1, 1, 1));
  }

  if (rank_id == 0) {
    int recv_buffer[total_block];
    for (size_t i = 1; i < world_size; i++) {
      MPI_Status status;
      MPI_Recv(recv_buffer, total_block, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      for (size_t j = 0; j < total_block; j++) {
        count_buffer[j] += recv_buffer[j];
      }
    }
//    int sum = 0;
//    for (size_t i = 0; i < total_block; i++) {
//      printf("Block %zu has %d objects\n", i, count_buffer[i]);
//      sum += count_buffer[i];
//    }
//    printf("all blocks have %d objects\n", sum);
  } else {
    MPI_Send(count_buffer, total_block, MPI_INT, 0, 0, MPI_COMM_WORLD);
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
  int block_num = 0;
  while (block_assignment[start_index] != rank_id) {
    start_index++;
  }
  while (start_index + block_num < total_block && block_assignment[start_index + block_num] == rank_id) {
    block_num++;
  }

  object_t object_buffer[TEST_SIZE];
  rectangle_t rectangle_buffer[TEST_SIZE];
  int size = 0;
  for (size_t i = 0; i < TEST_SIZE; i++) {
    int block_x, block_y;
    get_belonged_block(mid_point(rectangles[i]), BLOCK_NUM_PER_DIM, MATRIX_SIZE, &block_x, &block_y);
    int block_idx = matrix_map[block_x][block_y].id;
    if (block_idx >= start_index && block_idx < start_index + block_num) {
      object_buffer[size] = objects[i];
      rectangle_buffer[size] = rectangles[i];
      if (use_rtree) {
        insert(r_tree->root, object_buffer + size, rectangle_buffer[size]);
      }
      size++;
    }
  }
  printf("Worker %d is assigned block %d to %d and gathered %d objects\n", rank_id, start_index, start_index + block_num - 1, size);


  int rec_num[world_size];
  rectangle_t **ghost_rec_buf = NULL;

  found_all_adjacent(matrix_map, block_assignment, rank_id, world_size, rec_num, &ghost_rec_buf);


  /*
  printf("Id: %d   ", rank_id);
  for (int i = 0; i < world_size; i++) {
    printf("%d ", rec_num[i]);
  }
  printf("\n");
   */


  int send_num[world_size];
  rectangle_t send_rec_buf[world_size][TEST_SIZE];
  int recv_size = 0;
  rectangle_t recv_buf[TEST_SIZE];
  int search_size = 0;
  rectangle_t *search_pointer = NULL;

  for (int iter = 0; iter < ITER_TIME; iter++) {
    // ghost gathering
    for (int i = 0; i < world_size; i++) {
      // empty
      send_num[i] = 0;
      memset(send_rec_buf[i], 0, TEST_SIZE * sizeof(rectangle_t));

      for (int j = 0; j < rec_num[i]; j++) {
        if (use_rtree) {
          search_size = search_infect_rec(r_tree->root, ghost_rec_buf[i][j], &search_pointer);
        } else {
          search_size = scan_search_infect_rec(object_buffer, rectangle_buffer, size, ghost_rec_buf[i][j], &search_pointer);
        }

        memcpy(send_rec_buf[i] + send_num[i], search_pointer, sizeof(rectangle_t) * search_size);
        send_num[i] += search_size;
        free(search_pointer);
      }
    }


    // start to send and recv
    recv_size = 0;
    memset(recv_buf, 0, sizeof(rectangle_t) * TEST_SIZE);

    for (int i = 0; i < world_size; i++) {
      if (rec_num[i] > 0) {
        if (i > rank_id) {
          // recv and send
          MPI_Status status;
          MPI_Recv(recv_buf + recv_size, 4 * TEST_SIZE, MPI_FLOAT, i, 0, MPI_COMM_WORLD,
                   &status);
          int cur_recv_size;

          MPI_Get_count(&status, MPI_FLOAT, &cur_recv_size);
          cur_recv_size = cur_recv_size / 4;
          recv_size += cur_recv_size;
          // printf("ID: %d, received %d rec from %d in iter %d\n", rank_id, cur_recv_size, i, iter);

          // printf("ID: %d, send %d rec to %d in iter %d\n", rank_id, send_num[i], i, iter);
          MPI_Send(send_rec_buf[i], 4 * send_num[i], MPI_FLOAT, i, 0, MPI_COMM_WORLD);
        } else if (i < rank_id) {
          // printf("ID: %d, send %d rec to %d in iter %d\n", rank_id, send_num[i], i, iter);
          MPI_Send(send_rec_buf[i], 4 * send_num[i], MPI_FLOAT, i, 0, MPI_COMM_WORLD);
          // recv and send
          MPI_Status status;
          MPI_Recv(recv_buf + recv_size, 4 * TEST_SIZE, MPI_FLOAT, i, 0, MPI_COMM_WORLD,
                   &status);
          int cur_recv_size;

          MPI_Get_count(&status, MPI_FLOAT, &cur_recv_size);
          cur_recv_size = cur_recv_size / 4;
          recv_size += cur_recv_size;
          // printf("ID: %d, received %d rec from %d in iter %d\n", rank_id, cur_recv_size, i, iter);
        } else {
          printf("Error: don't need to send to self\n");
        }
      }
    }


    // start contact
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

    // contact ghost element
    for (int i = 0; i < recv_size; i++) {
      object_t **search_object = NULL;

      size_t found;
      if (use_rtree) {
        found = search(r_tree->root, recv_buf[i], &search_object);
      } else {
        found = scan_search(object_buffer, rectangle_buffer, size, recv_buf[i], &search_object);
      }

      for (int k = 0; k < found; k++) {
        contact(search_object[k], &infected_object, iter);
      }

      free(search_object);
    }

    //update status
    if (use_rtree) {
      tree_update(r_tree->root, iter);
    } else {
      status_update(object_buffer, size, iter);
    }

    // move objects
    if (use_rtree) {
      random_move_rtree_objects(r_tree->root, STEP_SIZE, MATRIX_SIZE);

      object_t* all_objects = NULL;
      rectangle_t* all_rectangles = NULL;
      search_with_rect(r_tree->root, r_tree->root->rectangle, &all_objects, &all_rectangles);
      for (size_t i = 0; i < size; i++) {
        object_buffer[i] = all_objects[i];
        rectangle_buffer[i] = all_rectangles[i];
      }
      free(all_objects);
      free(all_rectangles);

      /*
      // reconstruct

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
       */
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
          if (matrix_map[i][j].id < start_index || matrix_map[i][j].id >= start_index + block_num) {
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
        if (block_idx < start_index || block_idx >= start_index + block_num) {
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
      MPI_Recv(object_buffer + size, 4 * recv_size, MPI_INT, i, 0, MPI_COMM_WORLD, &status);

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
        MPI_Send(send_objs[j], send_size[j] * 4, MPI_INT, j, 0, MPI_COMM_WORLD);
      }
    }
    for (size_t i = rank_id + 1; i < world_size; i++) {
      int recv_size;
      MPI_Status status;
      MPI_Recv(rectangle_buffer + size, 4 * TEST_SIZE, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_FLOAT, &recv_size);
      recv_size = recv_size / 4;
      MPI_Recv(object_buffer + size, 4 * recv_size, MPI_INT, i, 0, MPI_COMM_WORLD, &status);

      if (use_rtree) {
        for (size_t j = 0; j < recv_size; j++) {
          insert(r_tree->root, object_buffer + size + j, rectangle_buffer[size + j]);
        }
      }

      size += recv_size;
    }
  }

  for (int i = 0; i < world_size; i++) {
    free(ghost_rec_buf[i]);
  }
  free(ghost_rec_buf);

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
      MPI_Recv(rectangle_buffer + size, 4 * TEST_SIZE, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_FLOAT, &recv_size);
      recv_size = recv_size / 4;
      MPI_Recv(object_buffer + size, 4 * recv_size, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
      size += recv_size;
//      printf("Receive %d objects from worker %d\n", recv_size, status.MPI_SOURCE);
    }
  } else {
    if (use_rtree) {
      object_t* all_objects = NULL;
      rectangle_t* all_rectangles = NULL;
      search_with_rect(r_tree->root, r_tree->root->rectangle, &all_objects, &all_rectangles);
      MPI_Send(all_rectangles, size * 4, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
      MPI_Send(all_objects, size * 4, MPI_INT, 0, 0, MPI_COMM_WORLD);
      free(all_objects);
      free(all_rectangles);
    } else {
      MPI_Send(rectangle_buffer, size * 4, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
      MPI_Send(object_buffer, size * 4, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
//    printf("Worker %d send %d objects\n", rank_id, size);
  }

  endTime = MPI_Wtime();

  MPI_Finalize();

  if (rank_id == 0) {
    printf("Total time used: %f for proc: %d, size: %d\n", endTime - startTime, rank_id, size);
    object_statistic(object_buffer, TEST_SIZE);
  }
  return 0;
}