#include <css_list.h>
#include <mem.h>
#include <string.h>

#define MAX(a, b) a > b ? a : b

#define MIN(a, b) a < b ? a : b

List *init_css_list(size_t item_size) {
  List *css_list = NEW(List);
  css_list->item_size = item_size;
  css_list->items = 0;
  css_list->size = 0;

  return css_list;
}

void css_list_append(List *css_list, void *item) {
  css_list->size += 1;
  css_list->items = realloc(css_list->items, css_list->size * sizeof(css_list->item_size));
  css_list->items[css_list->size - 1] = item;
}

void *css_list_at(List *css_list, size_t index) { return css_list->items[index]; }

void css_list_free(List *css_list) {
  if (css_list->items)
    free(css_list->items);

  free(css_list);
}

void css_list_shift_left(List *list, int index) {
  for (int i = index; i < list->size - 1; i++)
    list->items[i] = list->items[i + 1];
}

void css_list_shift_right(List *list, int index) {
  for (int i = list->size - 1; i >= index; i--) {
    list->items[MIN(list->size - 1, i + 1)] = list->items[i];
    list->items[i] = 0;
  }
}

void css_list_remove(List *list, void *element, void (*free_method)(void *item)) {
  int index = 0;

  if (element == (void *)0)
    return;

  for (int i = 0; i < list->size; i++) {
    if (list->items[i] == element) {
      index = i;
      break;
    }
  }

  if (free_method != (void *)0)
    free_method(list->items[index]);

  css_list_shift_left(list, index); /* First shift the elements, then reallocate */
  void *tmp = realloc(list->items, (list->size - 1) * list->item_size);
  if (tmp == NULL && list->size > 1) {
    /* No memory available */
    exit(EXIT_FAILURE);
  }
  list->size = list->size - 1;
  list->items = tmp;
}


unsigned int css_list_contains_str(List *css_list, const char* str) {
  if (str == 0) return 0;
  if (css_list == 0) return 0;
  if (css_list->size == 0) return 0;

  for (int i = 0; i < (int)css_list->size; i++) {
    char* v = (char*)css_list->items[i];
    if (strcmp(v, str) == 0) return 1;
  }

  return 0;
}
