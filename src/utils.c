#include <mem.h>
#include <utils.h>

char *char_to_string(char c) {
  char *s = scalloc(char *, sizeof(char), 2);
  s[0] = c;
  s[1] = '\0';

  return s;
}
