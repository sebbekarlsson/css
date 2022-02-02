#include <css.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define ASSERT(expr)                            \
  { printf("testing %s (%s)...\n", #expr, __func__); assert(expr); printf("OK\n"); }


#define WHOAMI()                                \
  printf("TEST (%s)\n", __func__);

static char *read_file(char *filepath) {
  char *buffer = NULL;
  size_t len;
  FILE *fp = fopen(filepath, "rb");
  getdelim(&buffer, &len, '\0', fp);
  fclose(fp);
  return buffer;
}

void test_color_css() {
  WHOAMI();
  char* contents = read_file("sources/color.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_wildcard_css() {
  WHOAMI();
  char* contents = read_file("sources/wildcard.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_id_css() {
  WHOAMI();
   char* contents = read_file("sources/id.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_media_query_css() {
  WHOAMI();
   char* contents = read_file("sources/media_query.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_style_big_css() {
  WHOAMI();
  char* contents = read_file("sources/style_big.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_font_face_css() {
  WHOAMI();
  char* contents = read_file("sources/font-face.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_font_face2_css() {
  WHOAMI();
  char* contents = read_file("sources/font-face2.css");
  CSSAST* style = css(contents);

  List* list = init_css_list(sizeof(CSSAST*));
  CSSAST* rule = css_get_rule(style, "@font-face");

  ASSERT(rule != 0);

  CSSAST* call_node = css_get_value_call(rule, "src", "url");

  ASSERT(call_node != 0);

  ASSERT(call_node->args != 0);
  ASSERT(call_node->args->size >= 1);

  CSSAST* arg = (CSSAST*)call_node->args->items[0];

  ASSERT(arg != 0);
  ASSERT(arg->value_str != 0);
  ASSERT(strcmp(arg->value_str, "https://fonts.gstatic.com/s/productsans/v18/pxiDypQkot1TnFhsFMOfGShVE9eOcEg.woff2") == 0);

  ASSERT(style != 0);
}

void test_bigbig_css() {
  WHOAMI();
  char* contents = read_file("sources/bigbig.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_news_css() {
  WHOAMI();
  char* contents = read_file("sources/news.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

void test_mystyle_css() {
  WHOAMI();
  char* contents = read_file("sources/mystyle.css");
  CSSAST* style = css(contents);

  ASSERT(style != 0);
}

int main(int argc, char* argv[]) {
  test_color_css();
  test_wildcard_css();
  test_id_css();
  test_media_query_css();
  test_style_big_css();
  test_font_face_css();
  test_font_face2_css();
  test_bigbig_css();
  test_news_css();
  test_mystyle_css();
  return 0;
}
