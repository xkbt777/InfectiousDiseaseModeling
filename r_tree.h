//
// Created by shawn lv on 2020-11-15.
//

#ifndef INFECTIOUSDISEASEMODELING_R_TREE_H
#define INFECTIOUSDISEASEMODELING_R_TREE_H

#include "util.h"

#define MAX_ENTRY_SIZE 10

#define INTERNAL 0
#define LEAF 1
#define OBJECT 2

typedef struct {
  size_t x;
  size_t y;
}point_t;

typedef struct {
  point_t bottom_left;
  point_t top_right;
}rectangle_t;

typedef struct {
  size_t node_type;
  size_t node_size;
  rectangle_t rectangle;
  object_t *object;
  node_t *entries[MAX_ENTRY_SIZE];
}node_t;

typedef struct {
  node_t *root;
}r_tree_t;

size_t search(node_t *node, rectangle_t area, object_t ***objects);

#endif //INFECTIOUSDISEASEMODELING_R_TREE_H
