#ifndef XCSS_LIST_H
#define XCSS_LIST_H
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  void **items;
  size_t size;
  size_t item_size;
} List;

List *init_css_list(size_t item_size);

void css_list_append(List *css_list, void *item);

void *css_list_at(List *css_list, size_t index);

unsigned int css_list_contains_str(List *css_list, const char* str);

void css_list_free(List *css_list);

void css_list_shift_left(List *list, int index);

void css_list_shift_right(List *list, int index);

void css_list_remove(List *list, void *element, void (*free_method)(void *item));

#endif
