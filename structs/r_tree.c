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
      objects_pointers[size_sum] = &node->entries[i]->object;
      size_sum += 1;
    }
  }

  *objects = (object_t **) calloc(size_sum, sizeof(object_t *));
  memcpy(*objects, objects_pointers, size_sum * sizeof(object_t *));

  return size_sum;
}

size_t search_infect_rec(node_t *node, rectangle_t area, rectangle_t **recs) {
  if (node == NULL) {
    *recs = NULL;
    return 0;
  }

  if (node->node_type == INTERNAL) {
    size_t *sizes = (size_t *) calloc(node->node_size, sizeof(size_t));
    rectangle_t **rec_pointers = (rectangle_t **) calloc(node->node_size, sizeof(rectangle_t *));

    size_t size_sum = 0;

    for (int i = 0; i < node->node_size; i++) {
      if (intersect(node->rectangle, area)) {
        sizes[i] = search_infect_rec(node->entries[i], area, &(rec_pointers[i]));
        size_sum += sizes[i];
      }
    }

    *recs = (rectangle_t *) calloc(size_sum, sizeof(rectangle_t));
    size_sum = 0;

    for (int i = 0; i < node->node_size; i++) {
      if (sizes[i] > 0) {
        memcpy((*recs) + size_sum, rec_pointers[i], sizes[i] * sizeof(rectangle_t));
        size_sum += sizes[i];
      }
      free(rec_pointers[i]);
    }

    free(sizes);
    free(rec_pointers);
    return size_sum;
  }

  rectangle_t rec_pointers[MAX_ENTRY_SIZE];
  size_t size_sum = 0;

  for (int i = 0; i < node->node_size; i++) {
    // printf("Object ID: %ld\n", node->entries[i]->object->id);
    // printf("outcome: %d\n", intersect(node->entries[i]->rectangle, area));
    if (node->entries[i]->object.status == INFECTED && intersect(node->entries[i]->rectangle, area)) {
      rec_pointers[size_sum] = node->entries[i]->rectangle;
      size_sum += 1;
    }
  }
  *recs = (rectangle_t *) calloc(size_sum, sizeof(rectangle_t));

  memcpy(*recs, rec_pointers, size_sum * sizeof(rectangle_t));
  return size_sum;
}

size_t search_with_rect(node_t *node, rectangle_t area, object_t **objects, rectangle_t **rectangles) {
    if (node == NULL) {
        *objects = NULL;
        return 0;
    }

    if (node->node_type == INTERNAL) {
        size_t *sizes = (size_t *) calloc(node->node_size, sizeof(size_t));
        object_t **object_pointers = (object_t **) calloc(node->node_size, sizeof(object_t *));
        rectangle_t **rectangle_pointers = (rectangle_t**) calloc(node->node_size, sizeof(rectangle_t*));

        size_t size_sum = 0;

        for (int i = 0; i < node->node_size; i++) {
            if (intersect(node->rectangle, area)) {
                sizes[i] = search_with_rect(node->entries[i], area, &(object_pointers[i]), &(rectangle_pointers[i]));
                size_sum += sizes[i];
            }
        }

        *rectangles = (rectangle_t*) calloc(size_sum, sizeof(rectangle_t));
        *objects = (object_t *) calloc(size_sum, sizeof(object_t));
        size_sum = 0;

        for (int i = 0; i < node->node_size; i++) {
            if (sizes[i] > 0) {
                memcpy((*rectangles) + size_sum, rectangle_pointers[i], sizes[i] * sizeof(rectangle_t));
                memcpy((*objects) + size_sum, object_pointers[i], sizes[i] * sizeof(object_t));
                size_sum += sizes[i];
            }
            free(object_pointers[i]);
            free(rectangle_pointers[i]);
        }

        free(sizes);
        free(object_pointers);
        free(rectangle_pointers);
        return size_sum;
    }

    object_t objects_pointers[MAX_ENTRY_SIZE];
    size_t size_sum = 0;
    rectangle_t rectangle_pointers[MAX_ENTRY_SIZE];

    for (int i = 0; i < node->node_size; i++) {
        // printf("Object ID: %ld\n", node->entries[i]->object->id);
        // printf("outcome: %d\n", intersect(node->entries[i]->rectangle, area));
        if (intersect(node->entries[i]->rectangle, area)) {
            objects_pointers[size_sum] = node->entries[i]->object;
            rectangle_pointers[size_sum] = node->entries[i]->rectangle;
            size_sum += 1;
        }
    }

    *objects = (object_t *) calloc(size_sum, sizeof(object_t));
    memcpy(*objects, objects_pointers, size_sum * sizeof(object_t));
    *rectangles = (rectangle_t*) calloc(size_sum, sizeof(rectangle_t));
    memcpy(*rectangles, rectangle_pointers, size_sum * sizeof(rectangle_t));

    return size_sum;
}

void insert(node_t *root, object_t *object, rectangle_t o_area) {
    choose_and_insert(root, NULL, o_area, object);
//     printf("\nINSERT RTREE\n");
//     print_tree(root, 0);
}

int delete(node_t* root, object_t *target, rectangle_t o_area) {
    node_t** set;
    size_t set_size = 0;

    node_t* res = find_and_delete(root, NULL, o_area, target, 1, &set, &set_size);

    if (res == NULL) {
        printf("Delete failed due to %.2f %.2f %.2f %.2f not found\n",
               o_area.bottom_left.x, o_area.bottom_left.y,
               o_area.top_right.x, o_area.top_right.y);
        return 0;
    }

    size_t i;
    for (i = 0; i < set_size; i++) {
        reinsert_node(root, set[i]);
    }
    free(set);

//    if (root->node_size == 1) {
//        node_t* node = root->entries[0];
//        copy_node(node, root);
//        free(node);
//    }
    if (root->node_size == 0) {
        root->rectangle = init(0, 0, 0, 0);
    }

//    printf("\nDELETE RTREE\n");
//    print_tree(root, 0);

    return 1;
}

void free_rtree(r_tree_t* r_tree) {
    free_node(r_tree->root);
    free(r_tree);
}

void random_move_rtree_objects(node_t* node, float step, int matrix_size) {
  if (node->node_type == LEAF) {
    for (size_t i = 0; i < node->node_size; i++) {
      random_object_move(&node->entries[i]->object, &node->entries[i]->rectangle, step, matrix_size);
      if (i == 0) {
        node->rectangle = node->entries[i]->rectangle;
      } else {
        node->rectangle = cover(node->rectangle, node->entries[i]->rectangle);
      }
    }
  } else {
    for (size_t i = 0; i < node->node_size; i++) {
      random_move_rtree_objects(node->entries[i], step, matrix_size);
      if (i == 0) {
        node->rectangle = node->entries[i]->rectangle;
      } else {
        node->rectangle = cover(node->rectangle, node->entries[i]->rectangle);
      }
    }
  }
}

node_t* choose_and_insert(node_t* node, node_t* parent, rectangle_t o_area, object_t* object) {
    if (node->node_type == LEAF) {
        node_t* new_obj = (node_t*) malloc(sizeof(node_t));
        new_obj->node_type = OBJECT;
        new_obj->rectangle = o_area;
        new_obj->object = *object;
        node->entries[node->node_size++] = new_obj;
        if (node->node_size == 1) {
            node->rectangle = o_area;
        }

    } else {
        float min_area = 0;
        int min_idx = -1;
        size_t i;
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
    int group1, group2;
    pickSeed(node, &group1, &group2);

    node_t* node2 = (node_t*) malloc(sizeof(node_t));
    node2->node_type = node->node_type;
    node2->entries[0] = node->entries[group2];
    node2->node_size = 1;
    node2->rectangle = node->entries[group2]->rectangle;
    size_t size = node->node_size;
    node->rectangle = node->entries[group1]->rectangle;
    node->node_size = 0;
    node_t* remain = NULL;

    size_t i;
    for (i = 0; i < size; i++) {
        if (i == group2) {
            continue;
        }
        if (i == group1) {
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

void pickSeed(node_t* node, int* group1, int* group2) {
    int top = 0, top_idx = -1;
    int bottom = 0, bottom_idx = -1;
    int right = 0, right_idx = -1;
    int left = 0, left_idx = -1;
    float total_height = 0;
    float total_width = 0;

    size_t i;
    for (i = 0; i < node->node_size; i++) {
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

    if (right_idx == left_idx || ((top-bottom)/total_height>(right-left)/total_width && top_idx != bottom_idx)) {
        *group1 = bottom_idx;
        *group2 = top_idx;
    } else {
        *group1 = left_idx;
        *group2 = right_idx;
    }
}

node_t* find_and_delete(node_t* node, node_t* parent, rectangle_t o_area, object_t* target,
        int level, node_t*** set, size_t* set_size) {
    if (node->node_type == LEAF) {
        size_t i;
        for (i = 0; i < node->node_size; i++) {
            if (&node->entries[i]->object == target || if_same_rectangle(node->entries[i]->rectangle, o_area)) {
//                printf("Delete object %.2f %.2f %.2f %.2f\n",
//                        node->entries[i]->rectangle.bottom_left.x, node->entries[i]->rectangle.bottom_left.y,
//                       node->entries[i]->rectangle.top_right.x, node->entries[i]->rectangle.top_right.y);
                break;
            }
        }

        // object not found
        if (i == node->node_size) {
            return NULL;
        }

        // move the remaining nodes
        for (i++; i < node->node_size; i++) {
            node->entries[i - 1] = node->entries[i];
        }
        node->node_size--;

        // allocate memory for elimination set
        *set = (node_t**) malloc(level * sizeof(node_t*));

    } else {
        size_t i;
        node_t* res = NULL;
        for (i = 0; i < node->node_size; i++) {
            if (if_cover(node->entries[i]->rectangle, o_area)) {
                res = find_and_delete(node->entries[i], node, o_area, target, level + 1, set, set_size);
                if (res != NULL) {
                    break;
                }
            }
        }

        // object not found
        if (res == NULL) {
            return NULL;
        }

//        printf("return to %.2f %.2f %.2f %.2f with node size %zu at level %d\n",
//               node->rectangle.bottom_left.x, node->rectangle.bottom_left.y,
//               node->rectangle.top_right.x, node->rectangle.top_right.y, node->node_size, level);
    }

    if (node->node_size < MIN_ENTRY_SIZE && parent != NULL) {
        (*set)[*set_size] = node;
        *set_size += 1;
//        printf("Add %.2f %.2f %.2f %.2f to elimination set %zu\n",
//               node->rectangle.bottom_left.x, node->rectangle.bottom_left.y,
//               node->rectangle.top_right.x, node->rectangle.top_right.y, *set_size);
        size_t i;
        for (i = 0; i < parent->node_size; i++) {
            if (parent->entries[i] == node) {
                break;
            }
        }

        for (i++; i < parent->node_size; i++) {
            parent->entries[i - 1] = parent->entries[i];
        }
        parent->node_size--;
    } else if (node->node_size > 0) {
        node->rectangle = node->entries[0]->rectangle;
        size_t i;
        for (i = 1; i < node->node_size; i++) {
            node->rectangle = cover(node->rectangle, node->entries[i]->rectangle);
        }
    }

    return node;
}

void reinsert_node(node_t* root, node_t* node) {
    if (node->node_type == LEAF) {
        size_t i;
        for (i = 0; i < node->node_size; i++) {
//            printf("Reinsert %.2f %.2f %.2f %.2f\n",
//                   node->entries[i]->rectangle.bottom_left.x, node->entries[i]->rectangle.bottom_left.y,
//                   node->entries[i]->rectangle.top_right.x, node->entries[i]->rectangle.top_right.y);
            choose_and_insert(root, NULL, node->entries[i]->rectangle, &node->entries[i]->object);
        }
    } else {
        size_t i;
        for (i = 0; i < node->node_size; i++) {
            reinsert_node(root, node->entries[i]);
        }
    }
    free(node);
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

void free_node(node_t* node) {
    if (node->node_type != OBJECT) {
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
