#include <iterator.h>



CSSIterator css_iterate(CSSAST** values, uint32_t length) {
  CSSIterator it = {0};
  it.length = length;
  it.values = values;
  it.i = 0;
  it.value = it.values[it.i];
  return it;
}

CSSAST* css_iterator_next(CSSIterator* iterator) {
  if (iterator->length <= 0 || iterator->values == 0) return 0;
  if (iterator->i >= iterator->length) {
    iterator->value = 0;
    return iterator->value;
  }

  iterator->value = iterator->values[iterator->i];
  iterator->i++;

  return iterator->value;
}
