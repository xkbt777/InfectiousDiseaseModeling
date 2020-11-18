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

typedef struct node {
  size_t node_type;
  size_t node_size;
  rectangle_t rectangle;
  object_t *object;
  struct node *entries[MAX_ENTRY_SIZE];
}node_t;

typedef struct {
  node_t *root;
}r_tree_t;

size_t search(node_t *node, rectangle_t area, object_t ***objects);

#endif //INFECTIOUSDISEASEMODELING_R_TREE_H
