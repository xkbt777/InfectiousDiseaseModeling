//
// Created by shawn lv on 2020-11-17.
//

#ifndef INFECTIOUSDISEASEMODELING_UTIL_H
#define INFECTIOUSDISEASEMODELING_UTIL_H


typedef struct {
  size_t x;
  size_t y;
}point_t;

typedef struct {
  point_t bottom_left;
  point_t top_right;
}rectangle_t;

typedef struct {
  size_t id;
}object_t;

int intersect(rectangle_t rec1, rectangle_t rec2);
rectangle_t cover(rectangle_t rec1, rectangle_t rec2);
size_t min(size_t a, size_t b);
size_t max(size_t a, size_t b);
rectangle_t init(size_t left, size_t bottom, size_t right, size_t top);

#endif //INFECTIOUSDISEASEMODELING_UTIL_H
