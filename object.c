#include "object.h"

void randomGenerate(size_t object_number, size_t matrix_size, unsigned int seed, object_t **out, rectangle_t **rec, float init_rate) {
  *out = (object_t *) calloc(object_number, sizeof(object_t));
  *rec = (rectangle_t *) calloc(object_number, sizeof(rectangle_t));
  srand(seed);

  for (int i = 0; i < object_number; i++) {
    (*out)[i].id = i;
    if (((float) rand() / (float) RAND_MAX) < init_rate) {
      (*out)[i].rate = 1.0f;
    } else {
      (*out)[i].rate = 0.0f;
    }

    size_t x = rand() % matrix_size;
    size_t y = rand() % matrix_size;

    (*rec)[i] = init(x, y, x + 1, y + 1);
  }
}

size_t scanSearch(object_t *objects, rectangle_t *recs, size_t object_number, rectangle_t target, object_t ***output) {
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

int main() {
  object_t *object_pointer = NULL;
  rectangle_t *rectangle_pointer = NULL;

  randomGenerate(10, 10, time(NULL), &object_pointer, &rectangle_pointer, 0.1);

  for (int i = 0; i < 10; i++) {
    printf("Object rate: %f, left: %ld, bottom: %ld\n", object_pointer[i].rate, rectangle_pointer[i].bottom_left.x, rectangle_pointer[i].bottom_left.y);
  }

  object_t **search_object = NULL;

  size_t found = scanSearch(object_pointer, rectangle_pointer, 10, init(2, 2, 5, 5), &search_object);

  for (int i = 0; i < found; i++) {
    printf("Object id: %ld\n", search_object[i]->id);
  }
}

