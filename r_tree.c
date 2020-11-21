#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "r_tree.h"

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
    printf("Object ID: %ld\n", node->entries[i]->object->id);
    printf("outcome: %d\n", intersect(node->entries[i]->rectangle, area));
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
}

node_t* choose_and_insert(node_t* node, node_t* parent, rectangle_t o_area, object_t* object) {
    if (node->node_type == LEAF) {
        node_t* new_obj = (node_t*) malloc(sizeof(object_t));
        new_obj->node_type = OBJECT;
        new_obj->rectangle = o_area;
        new_obj->object = object;
        node->entries[node->node_size] = new_obj;
        node->node_size++;

    } else {
        int min_area = 0;
        int min_idx = -1;
        int i;
        for (i = 0; i < node->node_size; i++) {
            int extra_area = area_diff(cover(node->entries[i]->rectangle, o_area), node->entries[i]->rectangle);
            if(min_idx == -1 || min_area > extra_area) {
                min_area = extra_area;
                min_idx = i;
            }
        }

        if (min_idx == -1) {
            node_t* new_node = (node_t*) malloc(sizeof(node_t));
            new_node->node_type = LEAF;
            new_node->node_size = 0;
            new_node->rectangle = o_area;
            node->entries[0] = new_node;
            node->node_size++;
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

}

int main() {
  printf("test basic search\n");

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

  node_t leaf_nodes[4];
  memset(leaf_nodes, 0, 4 * sizeof(node_t));

  for (int i = 0; i < 4; i++) {
    leaf_nodes[i].node_type = LEAF;
    leaf_nodes[i].node_size = 2;
    leaf_nodes[i].entries[0] = &(object_nodes[2 * i]);
    leaf_nodes[i].entries[1] = &(object_nodes[2 * i + 1]);
    leaf_nodes[i].rectangle = cover(leaf_nodes[i].entries[0]->rectangle, leaf_nodes[i].entries[1]->rectangle);
  }

  node_t internal_nodes[2];
  memset(internal_nodes, 0, 2 * sizeof(node_t));

  for (int i = 0; i < 2; i++) {
    internal_nodes[i].node_type = INTERNAL;
    internal_nodes[i].node_size = 2;
    internal_nodes[i].entries[0] = &(leaf_nodes[2 * i]);
    internal_nodes[i].entries[1] = &(leaf_nodes[2 * i + 1]);
    internal_nodes[i].rectangle = cover(internal_nodes[i].entries[0]->rectangle, internal_nodes[i].entries[1]->rectangle);
  }

  node_t root;

  root.node_type = INTERNAL;
  root.node_size = 2;
  root.entries[0] = &(internal_nodes[0]);
  root.entries[1] = &(internal_nodes[1]);
  root.rectangle = cover(root.entries[0]->rectangle, root.entries[1]->rectangle);

  r_tree_t r_tree;
  r_tree.root = &root;

  object_t **object_pointer = NULL;

  size_t count = search(r_tree.root, init(3, 5, 6, 6), &object_pointer);

  printf("Object Founded:\n");
  for (int i = 0; i < count; i++) {
    printf("Object Id : %ld\n", object_pointer[i]->id);
  }
  free(object_pointer);
}
