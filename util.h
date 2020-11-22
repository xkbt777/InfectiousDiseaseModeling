//
// Created by shawn lv on 2020-11-17.
//

#ifndef INFECTIOUSDISEASEMODELING_UTIL_H
#define INFECTIOUSDISEASEMODELING_UTIL_H


typedef struct {
  float x;
  float y;
}point_t;

typedef struct {
  point_t bottom_left;
  point_t top_right;
}rectangle_t;

typedef struct {
  size_t id;
  size_t status;
  size_t next_status;
  size_t infected_iteration;
}object_t;

int intersect(rectangle_t rec1, rectangle_t rec2);
int if_cover(rectangle_t rec1, rectangle_t rec2);
rectangle_t cover(rectangle_t rec1, rectangle_t rec2);
float area_diff(rectangle_t rec1, rectangle_t rec2);
float min(float a, float b);
float max(float a, float b);
rectangle_t init(float left, float bottom, float right, float top);
void object_to_file(object_t *objects, rectangle_t *recs, size_t object_number, char *filename, size_t matrix_size);

#endif //INFECTIOUSDISEASEMODELING_UTIL_H
