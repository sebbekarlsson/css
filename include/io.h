#ifndef CSS_IO_H
#define CSS_IO_H
char *css_read_file(const char *filename);

void css_write_file(const char *filename, char *outbuffer);

unsigned int css_file_exists(char *filepath);

#endif
