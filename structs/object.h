//
// Created by shawn lv on 2020-11-20.
//

#ifndef INFECTIOUSDISEASEMODELING_OBJECT_H
#define INFECTIOUSDISEASEMODELING_OBJECT_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "rectangle.h"

#define SUSPICIOUS 0
#define INFECTED 1
#define RECOVERED 2

#define HALF_INFECT_ZONE_LENGTH 0.5f

#define INFECTED_RATE 1.0f

#define RECOVER_THRESHOLD 5
#define RECOVER_RATE 1.0f

#define INIT_RATE 0.2

#define BUF_SIZE 1024
#define PI 3.14159265358979323846
#define to_deg(x) (x * PI / 180)

typedef struct {
    size_t id;
    size_t status;
    size_t next_status;
    size_t infected_iteration;
}object_t;

void random_generate(size_t object_number, size_t matrix_size, unsigned int seed, object_t **out, rectangle_t **rec);
size_t scan_search(object_t *objects, rectangle_t *recs, size_t object_number, rectangle_t target, object_t ***output);
size_t scan_search_infect_rec(object_t *objects, rectangle_t *recs, size_t object_number, rectangle_t target, rectangle_t **output);
void random_object_move(object_t *object, rectangle_t *rec, float step_size, float matrix_size);
float move(float origin, float offset, float matrix_size);
void contact(object_t *object1, object_t *object2, size_t cur_iter);
void status_update(object_t *objects, size_t object_number, size_t cur_iter);
void object_to_file(object_t *objects, rectangle_t *recs, size_t object_number, char *filename, size_t matrix_size);
void object_statistic(object_t *objects, size_t object_number);


#endif //INFECTIOUSDISEASEMODELING_OBJECT_H
