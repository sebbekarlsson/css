#ifndef CSS_ITERATOR_H
#define CSS_ITERATOR_H
#include <AST.h>
#include <stdint.h>
typedef struct {
  CSSAST* value;
  uint32_t length;
  uint32_t i;
  CSSAST** values;
} CSSIterator;


CSSIterator css_iterate(CSSAST** valus, uint32_t length);

CSSAST* css_iterator_next(CSSIterator* iterator);

void css_iterator_destroy(CSSIterator* iterator);
#endif
