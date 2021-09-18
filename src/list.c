#include <list.h>
#include <mem.h>

List *init_list(size_t item_size) {
  List *list = NEW(List);
  list->item_size = item_size;
  list->items = 0;
  list->size = 0;

  return list;
}

void list_append(List *list, void *item) {
  list->size += 1;
  list->items = realloc(list->items, list->size * sizeof(list->item_size));
  list->items[list->size - 1] = item;
}

void *list_at(List *list, size_t index) { return list->items[index]; }

void list_free(List *list) {
  if (list->items)
    free(list->items);

  free(list);
}
