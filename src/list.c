#include <css_list.h>
#include <mem.h>

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
