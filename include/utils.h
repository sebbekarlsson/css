#ifndef CSS_UTILS_H
#define CSS_UTILS_H
char *char_to_string(char c);

#define STR_APPEND_CHAR(s, c)                                                  \
  {                                                                            \
    size_t len = (s != 0 ? strlen(s) : 0) + 1;                                 \
    s = realloc(s, (len + 1) * sizeof(char));                                  \
    s[len - 1] = c;                                                            \
    s[len] = 0;                                                                \
  }

#define SSTRDUP(s) s ? strdup(s) : s

#define SSTRINT(s) s ? atoi(s) : 0

#define SSTRFLOAT(s) s ? atof(s) : 0.0
#endif
