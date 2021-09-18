#include <mem.h>
#include <stdio.h>

void *_scalloc(size_t ts, size_t len) {
  void *memory = calloc(ts, len);
  if (!memory) {
    printf("Could not allocate memory %ld %ld", ts, len);
    exit(1);
  }

  return memory;
}
