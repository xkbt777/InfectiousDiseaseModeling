//
// Created by shawn lv on 2020-11-20.
//

#ifndef INFECTIOUSDISEASEMODELING_OBJECT_H
#define INFECTIOUSDISEASEMODELING_OBJECT_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "util.h"


#define BUF_SIZE 1024

void randomGenerate(size_t object_number, size_t matrix_size, unsigned int seed, object_t **out, rectangle_t **rec, float init_rate);
size_t scanSearch(object_t *objects, rectangle_t *recs, size_t object_number, rectangle_t target, object_t ***output);


#endif //INFECTIOUSDISEASEMODELING_OBJECT_H
