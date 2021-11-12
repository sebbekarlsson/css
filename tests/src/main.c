#include <css.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define ASSERT(expr)                            \
  { printf("testing %s (%s)...\n", #expr, __func__); assert(expr); printf("OK\n"); }

static char *read_file(char *filepath) {
  char *buffer = NULL;
  size_t len;
  FILE *fp = fopen(filepath, "rb");
  getdelim(&buffer, &len, '\0', fp);
  fclose(fp);
  return buffer;
}

void test_color_css() {
  char* contents = read_file("sources/color.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_id_css() {
   char* contents = read_file("sources/id.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_media_query_css() {
   char* contents = read_file("sources/media_query.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_style_big_css() {
  char* contents = read_file("sources/style_big.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_font_face_css() {
  char* contents = read_file("sources/font-face.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_bigbig_css() {
  char* contents = read_file("sources/bigbig.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_news_css() {
  char* contents = read_file("sources/news.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_mystyle_css() {
  char* contents = read_file("sources/mystyle.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

int main(int argc, char* argv[]) {
  test_color_css();
  test_id_css();
  test_media_query_css();
  test_style_big_css();
  test_font_face_css();
  test_bigbig_css();
  test_news_css();
  test_mystyle_css();
  return 0;
}
