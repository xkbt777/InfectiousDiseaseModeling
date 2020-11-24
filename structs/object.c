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

    float x = 1.0 * rand() / RAND_MAX * (matrix_size - 1);
    float y = 1.0 * rand() / RAND_MAX * (matrix_size - 1);

    (*rec)[i] = init(x, y, x + 1, y + 1);
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

void random_object_move(object_t *object, rectangle_t *rec, float step_size, float matrix_size) {
  float direction = to_deg(rand() % 360);
  float x_offset = cos(direction) * step_size;
  float y_offset = sin(direction) * step_size;

  float mid_x = (rec->bottom_left.x + rec->top_right.x) / 2;
  float length_x = mid_x - rec->bottom_left.x;
  float mid_y = (rec->bottom_left.y + rec->top_right.y) / 2;
  float length_y = mid_y - rec->bottom_left.y;


  mid_x = move(mid_x, x_offset, matrix_size);
  mid_y = move(mid_y, y_offset, matrix_size);

  rec->bottom_left.x = mid_x - length_x;
  rec->bottom_left.y = mid_y - length_y;
  rec->top_right.x = mid_x + length_x;
  rec->top_right.y = mid_y + length_y;
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
    return;
  }

  if (object1->status == INFECTED && object2->status == SUSPICIOUS) {
    if (((float) rand() / (float) RAND_MAX) < INFECTED_RATE) {
      object2->next_status = INFECTED;
      object2->infected_iteration = cur_iter;
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

