#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "rectangle.h"

int intersect(rectangle_t rec1, rectangle_t rec2) {

  // rec1 under rec2
  if (rec1.top_right.y <= rec2.bottom_left.y) {
    return 0;
  }

  // rec1 above rec2
  if (rec1.bottom_left.y >= rec2.top_right.y) {
    return 0;
  }

  // rec1 is in left of rec2
  if (rec1.top_right.x <= rec2.bottom_left.x) {
    return 0;
  }

  // rec1 is in right of rec2
  if (rec1.bottom_left.x >= rec2.top_right.x) {
    return 0;
  }
  return 1;
}

int if_cover(rectangle_t rec1, rectangle_t rec2) {

    // rec1 covers rec2 totally
    if (rec1.top_right.x >= rec2.top_right.x && rec1.top_right.y >= rec2.top_right.y
        && rec1.bottom_left.x <= rec2.bottom_left.x && rec1.bottom_left.y <= rec2.bottom_left.y) {
        return 1;
    }

    return 0;
}

rectangle_t cover(rectangle_t rec1, rectangle_t rec2) {
  rectangle_t rectangle;

  rectangle.top_right.y = max(rec1.top_right.y, rec2.top_right.y);
  rectangle.top_right.x = max(rec1.top_right.x, rec2.top_right.x);

  rectangle.bottom_left.y = min(rec1.bottom_left.y, rec2.bottom_left.y);
  rectangle.bottom_left.x = min(rec1.bottom_left.x, rec2.bottom_left.x);

  return rectangle;
}

float area_diff(rectangle_t rec1, rectangle_t rec2) {
    float area1 = (rec1.top_right.x - rec1.bottom_left.x) * (rec1.top_right.y - rec1.bottom_left.y);
    float area2 = (rec2.top_right.x - rec2.bottom_left.x) * (rec2.top_right.y - rec2.bottom_left.y);

    if (area1 < area2) {
        float tmp = area1;
        area1 = area2;
        area2 = tmp;
    }

    return area1 - area2;
}

float min(float a, float b) {
  if (a < b) {
    return a;
  }
  return b;
}


float max(float a, float b) {
  if (a > b) {
    return a;
  }
  return b;
}

rectangle_t init(float left, float bottom, float right, float top) {
  rectangle_t rectangle;

  rectangle.top_right.y = top;
  rectangle.top_right.x = right;
  rectangle.bottom_left.y = bottom;
  rectangle.bottom_left.x = left;

  return rectangle;
}
