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
  struct node *entries[MAX_ENTRY_SIZE + 1];
}node_t;

typedef struct {
  node_t *root;
}r_tree_t;

size_t search(node_t *node, rectangle_t area, object_t ***objects);
void insert(node_t *root, object_t *object, rectangle_t o_area); // return 0 if insert successfully
node_t* choose_and_insert(node_t* node, node_t* parent, rectangle_t o_area, object_t* object);
void split_node(node_t* node, node_t* parent);

#endif //INFECTIOUSDISEASEMODELING_R_TREE_H
