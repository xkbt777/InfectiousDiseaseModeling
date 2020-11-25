//
// Created by shawn lv on 2020-11-17.
//

#ifndef INFECTIOUSDISEASEMODELING_RECTANGLE_H
#define INFECTIOUSDISEASEMODELING_RECTANGLE_H


typedef struct {
  float x;
  float y;
}point_t;

typedef struct {
  point_t bottom_left;
  point_t top_right;
}rectangle_t;

int intersect(rectangle_t rec1, rectangle_t rec2);
int if_cover(rectangle_t rec1, rectangle_t rec2);
rectangle_t cover(rectangle_t rec1, rectangle_t rec2);
float area_diff(rectangle_t rec1, rectangle_t rec2);
float min(float a, float b);
float max(float a, float b);
rectangle_t init(float left, float bottom, float right, float top);

#endif //INFECTIOUSDISEASEMODELING_RECTANGLE_H
