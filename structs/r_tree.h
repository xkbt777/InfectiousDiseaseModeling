//
// Created by shawn lv on 2020-11-15.
//

#ifndef INFECTIOUSDISEASEMODELING_R_TREE_H
#define INFECTIOUSDISEASEMODELING_R_TREE_H

#include "rectangle.h"
#include "object.h"

#define MAX_ENTRY_SIZE 4
#define MIN_ENTRY_SIZE 2

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

r_tree_t* init_rtree();
size_t search(node_t *node, rectangle_t area, object_t ***objects);
size_t search_infect_rec(node_t *node, rectangle_t area, rectangle_t **recs);
size_t search_with_rect(node_t *node, rectangle_t area, object_t **objects, rectangle_t **rectangles);
void insert(node_t *root, object_t *object, rectangle_t o_area);
int delete(node_t* root, object_t *target, rectangle_t o_area);
void free_rtree(r_tree_t* r_tree);

node_t* choose_and_insert(node_t* node, node_t* parent, rectangle_t o_area, object_t* object);
void split_node(node_t* node, node_t* parent);
void pickSeed(node_t* node, int* group1, int* group2);
node_t* find_and_delete(node_t* node, node_t* parent, rectangle_t o_area, object_t* target,
        int level, node_t*** set, size_t* set_size);
void reinsert_node(node_t* root, node_t* node);
void copy_node(node_t* src, node_t* dest);
void print_tree(node_t* node, int level);
void tree_to_file(node_t* node, FILE *file, int level);
void free_node(node_t* root);

#endif //INFECTIOUSDISEASEMODELING_R_TREE_H
