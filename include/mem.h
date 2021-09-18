#ifndef XCSS_MEM_H
#define XCSS_MEM_H
#include <stdlib.h>
void *_scalloc(size_t ts, size_t len);

#define scalloc(T, ts, len) (T) _scalloc(ts, len)

#define NEW(T) (T *)_scalloc(sizeof(T), 1)
#endif
