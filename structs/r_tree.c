#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "r_tree.h"

r_tree_t* init_rtree() {
    node_t* root = (node_t*) malloc(sizeof(node_t));
    root->node_type = LEAF;
    root->rectangle = init(0, 0, 0, 0);
    root->node_size = 0;

    r_tree_t* r_tree = (r_tree_t*) malloc(sizeof(r_tree_t));
    r_tree->root = root;

    return r_tree;
}

size_t search(node_t *node, rectangle_t area, object_t ***objects) {
  if (node == NULL) {
    *objects = NULL;
    return 0;
  }

  if (node->node_type == INTERNAL) {
    size_t *sizes = (size_t *) calloc(node->node_size, sizeof(size_t));
    object_t ***object_pointers = (object_t ***) calloc(node->node_size, sizeof(object_t **));

    size_t size_sum = 0;

    for (int i = 0; i < node->node_size; i++) {
      if (intersect(node->rectangle, area)) {
        sizes[i] = search(node->entries[i], area, &(object_pointers[i]));
        size_sum += sizes[i];
      }
    }

    *objects = (object_t **) calloc(size_sum, sizeof(object_t *));
    size_sum = 0;

    for (int i = 0; i < node->node_size; i++) {
      if (sizes[i] > 0) {
        memcpy((*objects) + size_sum, object_pointers[i], sizes[i] * sizeof(object_t *));
        size_sum += sizes[i];
      }
      free(object_pointers[i]);
    }

    free(sizes);
    free(object_pointers);
    return size_sum;
  }

  object_t *objects_pointers[MAX_ENTRY_SIZE];
  size_t size_sum = 0;

  for (int i = 0; i < node->node_size; i++) {
    // printf("Object ID: %ld\n", node->entries[i]->object->id);
    // printf("outcome: %d\n", intersect(node->entries[i]->rectangle, area));
    if (intersect(node->entries[i]->rectangle, area)) {
      objects_pointers[size_sum] = node->entries[i]->object;
      size_sum += 1;
    }
  }
  *objects = (object_t **) calloc(size_sum, sizeof(object_t *));

  memcpy(*objects, objects_pointers, size_sum * sizeof(object_t *));
  return size_sum;
}

void insert(node_t *root, object_t *object, rectangle_t o_area) {
    choose_and_insert(root, NULL, o_area, object);
    // printf("\nRTREE\n");
    // print_tree(root, 0);
}

node_t* choose_and_insert(node_t* node, node_t* parent, rectangle_t o_area, object_t* object) {
    if (node->node_type == LEAF) {
        node_t* new_obj = (node_t*) malloc(sizeof(node_t));
        new_obj->node_type = OBJECT;
        new_obj->rectangle = o_area;
        new_obj->object = object;
        node->entries[node->node_size++] = new_obj;
        if (node->node_size == 1) {
            node->rectangle = o_area;
        }

    } else {
        float min_area = 0;
        int min_idx = -1;
        int i;
        for (i = 0; i < node->node_size; i++) {
            float extra_area = area_diff(cover(node->entries[i]->rectangle, o_area), node->entries[i]->rectangle);
            if (min_idx == -1 || min_area > extra_area) {
                min_area = extra_area;
                min_idx = i;
            }
        }

        if (min_idx == -1) {
            node_t* new_node = (node_t*) malloc(sizeof(node_t));
            new_node->node_type = LEAF;
            new_node->node_size = 0;
            new_node->rectangle = o_area;
            node->entries[node->node_size++] = new_node;
            node->node_type = INTERNAL;
            min_idx = 0;
        }

        node_t* res = choose_and_insert(node->entries[min_idx], node, o_area, object);
        o_area = res->rectangle;
    }

    node->rectangle = cover(node->rectangle, o_area);

    if (node->node_size > MAX_ENTRY_SIZE) {
        split_node(node, parent);
    }

    return node;
}

void split_node(node_t* node, node_t* parent) {
//    printf("%.2f %.2f %.2f %.2f SPLIT\n",
//           node->rectangle.bottom_left.x, node->rectangle.bottom_left.y,
//           node->rectangle.top_right.x, node->rectangle.top_right.y);
    int top = 0, top_idx = -1;
    int bottom = 0, bottom_idx = -1;
    int right = 0, right_idx = -1;
    int left = 0, left_idx = -1;
    float total_height = 0;
    float total_width = 0;

    size_t size = node->node_size;
    int i;
    for (i = 0; i < size; i++) {
        node_t* curr_n = node->entries[i];
        total_height += (curr_n->rectangle.top_right.y - curr_n->rectangle.bottom_left.y);
        total_width += (curr_n->rectangle.top_right.x - curr_n->rectangle.bottom_left.x);

        if (top_idx == -1 || top < curr_n->rectangle.top_right.y) {
            top = curr_n->rectangle.top_right.y;
            top_idx = i;
        }
        if (bottom_idx == -1 || bottom > curr_n->rectangle.bottom_left.y) {
            bottom = curr_n->rectangle.bottom_left.y;
            bottom_idx = i;
        }
        if (right_idx == -1 || right < curr_n->rectangle.top_right.x) {
            right = curr_n->rectangle.top_right.x;
            right_idx = i;
        }
        if (left_idx == -1 || left > curr_n->rectangle.bottom_left.x) {
            left = curr_n->rectangle.bottom_left.x;
            left_idx = i;
        }
    }

    int group1, group2;
    if (right_idx == left_idx || ((top-bottom)/total_height>(right-left)/total_width && top_idx != bottom_idx)) {
        group1 = bottom_idx;
        group2 = top_idx;
    } else {
        group1 = left_idx;
        group2 = right_idx;
    }

//    printf("SPLIT start with %.2f %.2f %.2f %.2f and %.2f %.2f %.2f %.2f\n",
//           node->entries[group1]->rectangle.bottom_left.x, node->entries[group1]->rectangle.bottom_left.y,
//           node->entries[group1]->rectangle.top_right.x, node->entries[group1]->rectangle.top_right.y,
//           node->entries[group2]->rectangle.bottom_left.x, node->entries[group2]->rectangle.bottom_left.y,
//           node->entries[group2]->rectangle.top_right.x, node->entries[group2]->rectangle.top_right.y);

    node_t* node2 = (node_t*) malloc(sizeof(node_t));
    node2->node_type = node->node_type;
    node2->entries[0] = node->entries[group2];
    node2->node_size = 1;
    node2->rectangle = node->entries[group2]->rectangle;
    node->rectangle = node->entries[group1]->rectangle;
    node->node_size = 0;

    node_t* remain = NULL;
    for (i = 0; i < size; i++) {
        if (i == group2) {
            continue;
        }
        if (i == group1) {
//            printf("%.2f %.2f %.2f %.2f assigned to node\n",
//              node->entries[i]->rectangle.bottom_left.x, node->entries[i]->rectangle.bottom_left.y,
//              node->entries[i]->rectangle.top_right.x, node->entries[i]->rectangle.top_right.y);
            node->entries[node->node_size++] = node->entries[i];
            continue;
        }
        if (node->node_size == MIN_ENTRY_SIZE && node2->node_size < MIN_ENTRY_SIZE) {
            remain = node2;
            break;
        }
        if (node2->node_size == MIN_ENTRY_SIZE && node->node_size < MIN_ENTRY_SIZE) {
            remain = node;
            break;
        }

        rectangle_t r1 = cover(node->rectangle, node->entries[i]->rectangle);
        float diff1 = area_diff(r1, node->rectangle);
        rectangle_t r2 = cover(node2->rectangle, node->entries[i]->rectangle);
        float diff2 = area_diff(r2, node2->rectangle);

        if (diff1 < diff2) {
            node->rectangle = r1;
            node->entries[node->node_size++] = node->entries[i];
        } else {
            node2->rectangle = r2;
            node2->entries[node2->node_size++] = node->entries[i];
        }
    }
    for (; i < size; i++) {
        if (i == group2) {
            continue;
        }
        if (i == group1) {
            node->entries[node->node_size++] = node->entries[i];
            continue;
        }
        remain->rectangle = cover(remain->rectangle, node->entries[i]->rectangle);
        remain->entries[remain->node_size++] = node->entries[i];
    }

    if (parent != NULL) {
        parent->entries[parent->node_size++] = node2;
        parent->rectangle = cover(parent->rectangle, node2->rectangle);
    } else {
        node_t* node1 = (node_t*) malloc(sizeof(node_t));
        copy_node(node, node1);
        node->node_type = INTERNAL;
        node->node_size = 2;
        node->rectangle = cover(node1->rectangle, node2->rectangle);
        node->entries[0] = node1;
        node->entries[1] = node2;
    }
}

void copy_node(node_t* src, node_t* dest) {
    dest->node_type = src->node_type;
    dest->node_size = src->node_size;
    dest->rectangle = src->rectangle;
    dest->object = src->object;
    int i;
    for (i = 0; i < src->node_size; i++) {
        dest->entries[i] = src->entries[i];
    }
}

void print_tree(node_t* node, int level) {
     if (node->node_type == OBJECT) {
         printf("Level: %d Object: %.2f %.2f %.2f %.2f\n", level, node->rectangle.bottom_left.x, node->rectangle.bottom_left.y,
                 node->rectangle.top_right.x, node->rectangle.top_right.y);
     } else {
         int i;
         for (i = 0; i < node->node_size; i++) {
             print_tree(node->entries[i], level + 1);
         }
         if (node->node_type == INTERNAL) {
             printf("Level: %d Size: %ld, Internal: ", level, node->node_size);
         } else {
             printf("Level: %d Size: %ld, Leaf: ", level, node->node_size);
         }
         printf("%.2f %.2f %.2f %.2f\n", node->rectangle.bottom_left.x, node->rectangle.bottom_left.y,
                node->rectangle.top_right.x, node->rectangle.top_right.y);
     }
}

void free_rtree(r_tree_t* r_tree) {
    free_node(r_tree->root);
    free(r_tree);
}

void free_node(node_t* node) {
    if (node->node_type == OBJECT) {
//        free(node->object);
    } else {
        int i;
        for (i = 0; i < node->node_size; i++) {
            free_node(node->entries[i]);
        }
    }

    free(node);
}

void tree_to_file(node_t* node, FILE *file, int level) {
  if (node->node_type == OBJECT) {
    fprintf(file, "%d,%f,%f,%f,%f\n", level, node->rectangle.bottom_left.x, node->rectangle.bottom_left.y,
           node->rectangle.top_right.x, node->rectangle.top_right.y);
  } else {
    int i;
    for (i = 0; i < node->node_size; i++) {
      tree_to_file(node->entries[i], file, level + 1);
    }
    fprintf(file, "%d,%f,%f,%f,%f\n", level, node->rectangle.bottom_left.x, node->rectangle.bottom_left.y,
           node->rectangle.top_right.x, node->rectangle.top_right.y);
  }
}

/*
int main() {
  printf("test basic search&insert\n");

  object_t objects[8];
  node_t object_nodes[8];
  memset(object_nodes, 0, 8 * sizeof(node_t));

  for (int i = 0; i < 8; i++) {
    objects[i].id = i;
    object_nodes[i].object = &(objects[i]);
    object_nodes[i].node_type = OBJECT;
  }

  object_nodes[0].rectangle = init(1, 1, 2, 2);
  object_nodes[1].rectangle = init(3, 1, 4, 2);
  object_nodes[2].rectangle = init(5, 1, 6, 2);
  object_nodes[3].rectangle = init(1, 3, 2, 4);
  object_nodes[4].rectangle = init(5, 3, 6, 4);
  object_nodes[5].rectangle = init(1, 5, 2, 6);
  object_nodes[6].rectangle = init(3, 5, 4, 6);
  object_nodes[7].rectangle = init(5, 5, 6, 6);

//  node_t leaf_nodes[4];
//  memset(leaf_nodes, 0, 4 * sizeof(node_t));
//
//  for (int i = 0; i < 4; i++) {
//    leaf_nodes[i].node_type = LEAF;
//    leaf_nodes[i].node_size = 2;
//    leaf_nodes[i].entries[0] = &(object_nodes[2 * i]);
//    leaf_nodes[i].entries[1] = &(object_nodes[2 * i + 1]);
//    leaf_nodes[i].rectangle = cover(leaf_nodes[i].entries[0]->rectangle, leaf_nodes[i].entries[1]->rectangle);
//  }
//
//  node_t internal_nodes[2];
//  memset(internal_nodes, 0, 2 * sizeof(node_t));
//
//  for (int i = 0; i < 2; i++) {
//    internal_nodes[i].node_type = INTERNAL;
//    internal_nodes[i].node_size = 2;
//    internal_nodes[i].entries[0] = &(leaf_nodes[2 * i]);
//    internal_nodes[i].entries[1] = &(leaf_nodes[2 * i + 1]);
//    internal_nodes[i].rectangle = cover(internal_nodes[i].entries[0]->rectangle, internal_nodes[i].entries[1]->rectangle);
//  }

//  node_t root;
//
//  root.node_type = INTERNAL;
//  root.node_size = 2;
//  root.entries[0] = &(internal_nodes[0]);
//  root.entries[1] = &(internal_nodes[1]);
//  root.rectangle = cover(root.entries[0]->rectangle, root.entries[1]->rectangle);
//
//  r_tree_t r_tree;
//  r_tree.root = &root;

  object_t **object_pointer = NULL;

  r_tree_t* r_tree = init_rtree();

  for (int i = 0; i < 8; i++) {
      insert(r_tree->root, object_nodes[i].object, object_nodes[i].rectangle);
  }

  size_t count = search(r_tree->root, init(3, 5, 6, 6), &object_pointer);

  printf("Object Founded:\n");
  for (int i = 0; i < count; i++) {
    printf("Object Id : %ld\n", object_pointer[i]->id);
  }
  free(object_pointer);
  free_rtree(r_tree);
}
*/
