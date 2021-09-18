#ifndef CSS_LIST_H
#define CSS_LIST_H
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  void **items;
  size_t size;
  size_t item_size;
} List;

List *init_list(size_t item_size);

void list_append(List *list, void *item);

void *list_at(List *list, size_t index);

void list_free(List *list);

#endif
