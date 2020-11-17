#include <stdlib.h>
#include <string.h>

#include "r_tree.h"
#include "util.h"

size_t search(node_t *node, rectangle_t area, object_t **objects) {
  if (node == NULL) {
    objects = NULL;
    return 0;
  }

  if (node->node_type == INTERNAL) {
    size_t *sizes = (size_t *) calloc(node->node_size, sizeof(size_t));
    object_t ***object_pointers = (object_t ***) calloc(node->node_size, sizeof(object_t **));

    size_t size_sum;

    for (int i = 0; i < node->node_size; i++) {
      sizes[i] = search(node->entries[i], area, object_pointers[i]);
      size_sum += sizes[i];
    }

    objects = (object_t **) calloc(size_sum, sizeof(object_t *));
    size_sum = 0;

    for (int i = 0; i < node->node_size; i++) {
      if (sizes[i] > 0) {
        memcpy(objects + size_sum, object_pointers[i], sizes[i] * sizeof(object_t *));
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
    if (intersect(node->entries[i]->rectangle, area)) {
      objects_pointers[size_sum] = node->entries[i]->object;
      size_sum += 1;
    }
  }

  memcpy(objects, objects_pointers, size_sum * sizeof(object_t *));
  return size_sum;
}

int main() {
  printf('test basic search');

  object_t objects[8];
  node_t object_nodes[8];
  memset(object_nodes, 0, 8 * sizeof(node_t));

  for (int i = 0; i < 8; i++) {
    objects[i].id = i;
    object_nodes[i].object = &(objects[i]);
    object_nodes[i].node_type = OBJECT;
  }

  object_nodes[0] = init(1, 1, 2, 2);
  object_nodes[1] = init(3, 1, 4, 2);
  object_nodes[2] = init(5, 1, 6, 2);
  object_nodes[3] = init(1, 3, 2, 4);
  object_nodes[4] = init(5, 3, 6, 4);
  object_nodes[5] = init(1, 5, 2, 6);
  object_nodes[6] = init(3, 5, 4, 6);
  object_nodes[7] = init(5, 5, 6, 6);

  node_t leaf_nodes[4];
  memset(leaf_nodes, 0, 4 * sizeof(node_t));

  for (int i = 0; i < 4; i++) {
    leaf_nodes[i].node_type = LEAF;
    leaf_nodes[i].node_size = 2;
    leaf_nodes[i].entries[0] = &(object_nodes[2 * i]);
    leaf_nodes[i].entries[1] = &(object_nodes[2 * i + 1]);
    leaf_nodes[i].rectangle = cover(leaf_nodes[i].entries[0].rectangle, leaf_nodes[i].entries[1].rectangle);
  }

  node_t internal_nodes[2];
  memset(internal_nodes, 0, 2 * sizeof(node_t));

  for (int i = 0; i < 2; i++) {
    internal_nodes[i].node_type = INTERNAL;
    internal_nodes[i].node_size = 2;
    internal_nodes[i].entries[0] = &(leaf_nodes[2 * i]);
    internal_nodes[i].entries[1] = &(leaf_nodes[2 * i + 1]);
    internal_nodes[i].rectangle = cover(internal_nodes[i].entries[0].rectangle, internal_nodes[i].entries[1].rectangle);
  }

  node_t root;

  root.node_type = INTERNAL;
  root.node_size = 2;
  root.entries[0] = &(internal_nodes[0]);
  root.entries[1] = &(internal_nodes[1]);
  root.rectangle = cover(root.entries[0].rectangle, root.entries[1].rectangle);

  r_tree_t r_tree;
  r_tree.root = &root;

  object_t **object_pointer;

  size_t count = search(r_tree.root, init(1, 1, 4, 4), object_pointer);

  for (int i = 0; i < count; i++) {
    printf("Object Id : %d\n", object_pointer[i]->id);
  }
  free(object_pointer);
}
