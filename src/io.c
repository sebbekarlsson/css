#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *css_read_file(const char *filename) {
  FILE* fp = fopen(filename, "r");
  if (!fp) { printf("Could not open %s\n", filename);  return 0; }
  char* buffer = NULL;
size_t len;
ssize_t bytes_read = getdelim( &buffer, &len, '\0', fp);

return buffer;
}
void css_write_file(const char *filename, char *outbuffer) {
  FILE *fp;

  fp = fopen(filename, "wb");
  if (fp == NULL) {
    printf("Could not open file for writing `%s`\n", filename);
    exit(1);
  }

  fputs(outbuffer, fp);

  printf("Wrote to: %s\n", filename);

  fclose(fp);
}

unsigned int css_file_exists(char *filepath) {
  return access(filepath, F_OK) == 0;
}
