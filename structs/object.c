#include "object.h"

void random_generate(size_t object_number, size_t matrix_size, unsigned int seed, object_t **out, rectangle_t **rec) {
  *out = (object_t *) calloc(object_number, sizeof(object_t));
  *rec = (rectangle_t *) calloc(object_number, sizeof(rectangle_t));
  srand(seed);

  for (int i = 0; i < object_number; i++) {
    (*out)[i].id = i;
    if (((float) rand() / (float) RAND_MAX) < INIT_RATE) {
      (*out)[i].status = INFECTED;
      (*out)[i].next_status = INFECTED;
      (*out)[i].infected_iteration = -1;
    } else {
      (*out)[i].status = SUSPICIOUS;
      (*out)[i].next_status = SUSPICIOUS;
    }

    float x = 1.0 * rand() / RAND_MAX * matrix_size;
    float y = 1.0 * rand() / RAND_MAX * matrix_size;

    (*rec)[i] = init(x - HALF_INFECT_ZONE_LENGTH, y - HALF_INFECT_ZONE_LENGTH,
      x + HALF_INFECT_ZONE_LENGTH, y + HALF_INFECT_ZONE_LENGTH);
  }
}

size_t scan_search(object_t *objects, rectangle_t *recs, size_t object_number, rectangle_t target, object_t ***output) {
  *output = (object_t **) calloc(BUF_SIZE, sizeof(object_t *));
  size_t size = 0;
  size_t buf_size = BUF_SIZE;

  for (int i = 0; i < object_number; i++) {
    if (intersect(target, recs[i])) {

      // buffer write
      if (size == buf_size) {
        buf_size += BUF_SIZE;
        object_t **tmp = *output;
        *output = (object_t **) calloc(buf_size, sizeof(object_t *));
        memcpy(*output, tmp, size * sizeof(object_t *));
        free(tmp);
      }

      (*output)[size] = objects + i;
      size += 1;
    }
  }
  return size;
}

size_t scan_search_infect_rec(object_t *objects, rectangle_t *recs, size_t object_number, rectangle_t target, rectangle_t **output) {
  *output = (rectangle_t *) calloc(BUF_SIZE, sizeof(rectangle_t));
  size_t size = 0;
  size_t buf_size = BUF_SIZE;

  for (int i = 0; i < object_number; i++) {
    if (objects[i].status == INFECTED && intersect(target, recs[i])) {

      // buffer write
      if (size == buf_size) {
        buf_size += BUF_SIZE;
        rectangle_t *tmp = *output;
        *output = (rectangle_t *) calloc(buf_size, sizeof(rectangle_t));
        memcpy(*output, tmp, size * sizeof(rectangle_t));
        free(tmp);
      }

      (*output)[size] = recs[i];
      size += 1;
    }
  }
  return size;
}


void random_object_move(object_t *object, rectangle_t *rec, float step_size, float matrix_size) {
  float direction = to_deg(rand() % 360);
  float x_offset = cos(direction) * step_size;
  float y_offset = sin(direction) * step_size;

  point_t mid_p = mid_point(*rec);

  float length_x = (rec->top_right.x - rec->bottom_left.x) / 2;
  float length_y = (rec->top_right.y - rec->bottom_left.y) / 2;

  mid_p.x = move(mid_p.x, x_offset, matrix_size);
  mid_p.y = move(mid_p.y, y_offset, matrix_size);

  rec->bottom_left.x = mid_p.x - length_x;
  rec->bottom_left.y = mid_p.y - length_y;
  rec->top_right.x = mid_p.x + length_x;
  rec->top_right.y = mid_p.y + length_y;
}

float move(float origin, float offset, float matrix_size) {
  float outcome = origin + offset;

  if (outcome < 0) {
    return -outcome;
  }

  if (outcome > matrix_size) {
    return 2 * matrix_size - outcome;
  }

  return outcome;
}

void contact(object_t *object1, object_t *object2, size_t cur_iter) {
  if (object1->status == SUSPICIOUS && object2->status == INFECTED) {
    if (((float) rand() / (float) RAND_MAX) < INFECTED_RATE) {
      object1->next_status = INFECTED;
      object1->infected_iteration = cur_iter;
    }
  }
}

void status_update(object_t *objects, size_t object_number, size_t cur_iter) {
  for (int i = 0; i < object_number; i++) {

    // recovery check
    if (objects[i].status == INFECTED && cur_iter - objects[i].infected_iteration >= RECOVER_THRESHOLD) {
      if (((float) rand() / (float) RAND_MAX) < RECOVER_RATE) {
        objects[i].status = RECOVERED;
        objects[i].next_status = RECOVERED;
        continue;
      }
    }
    objects[i].status = objects[i].next_status;
  }
}


void object_to_file(object_t *objects, rectangle_t *recs, size_t object_number, char *filename, size_t matrix_size) {
    FILE *file = fopen(filename, "w");

    fprintf(file, "%zu\n", matrix_size);

    for (int i = 0; i < object_number; i++) {
        fprintf(file, "%zu,%f,%f,%f,%f,%zu\n", objects[i].id, recs[i].bottom_left.x, recs[i].bottom_left.y,
                recs[i].top_right.x, recs[i].top_right.y, objects[i].status);
    }
    fclose(file);
}

void object_statistic(object_t *objects, size_t object_number) {
  int suspicious = 0, infected = 0, recovered = 0;
  for (int i = 0; i < object_number; i++) {
    if (objects[i].status == SUSPICIOUS) {
      suspicious += 1;
      continue;
    }

    if (objects[i].status == INFECTED) {
      infected += 1;
      continue;
    }

    recovered += 1;
  }

  printf("Sus: %d, Inf: %d, Rec: %d\n", suspicious, infected, recovered);
}

/*
int main() {
  object_t *object_pointer = NULL;
  rectangle_t *rectangle_pointer = NULL;

  random_generate(10, 10, time(NULL), &object_pointer, &rectangle_pointer, 0.1);

  for (int i = 0; i < 10; i++) {
    printf("Object status: %zu, left: %f, bottom: %f\n", object_pointer[i].status, rectangle_pointer[i].bottom_left.x, rectangle_pointer[i].bottom_left.y);
  }

  object_t **search_object = NULL;

  size_t found = scan_search(object_pointer, rectangle_pointer, 10, init(2, 2, 5, 5), &search_object);

  for (int i = 0; i < found; i++) {
    printf("Object id: %ld\n", search_object[i]->id);
  }
}
 */

