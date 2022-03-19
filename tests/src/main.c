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
  ASSERT(style != 0);

  CSSAST* rule = css_get_rule_nth(style, "@font-face", 0);

  ASSERT(rule != 0);

  CSSAST* call_node = css_get_value_call(rule, "src", "url");

  ASSERT(call_node != 0);

  ASSERT(call_node->args != 0);
  ASSERT(call_node->args->size >= 1);

  CSSAST* arg = (CSSAST*)call_node->args->items[0];

  ASSERT(arg != 0);
  ASSERT(arg->value_str != 0);
  ASSERT(strcmp(arg->value_str, "https://fonts.gstatic.com/s/productsans/v18/pxiDypQkot1TnFhsFMOfGShVE9eOcEg.woff2") == 0);

}

void test_iterator_font_face2_css() {
  WHOAMI();

  char* contents = read_file("sources/font-face2.css");
  CSSAST* style = css(contents);
  ASSERT(style != 0);

  CSSIterator it = css_get_rules(style, "@font-face");

  ASSERT(it.length >= 4);


  CSSAST* rule = 0;
  while ((rule = css_iterator_next(&it)) != 0) {
    ASSERT(rule != 0);
    ASSERT(rule->type == CSS_AST_RULE);
    CSSAST* call_node = css_get_value_call(rule, "src", "url");

    ASSERT(call_node != 0);
    ASSERT(call_node->args != 0);
    ASSERT(call_node->args->size >= 1);

    CSSAST* arg = (CSSAST*)call_node->args->items[0];

    ASSERT(arg != 0);
    ASSERT(arg->value_str != 0);
  }
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

void test_padding_css() {
  WHOAMI();
  char* contents = read_file("sources/padding.css");
  CSSAST* style = css(contents);
  ASSERT(style != 0);


  CSSAST* div = css_get_rule(style, "div");
  ASSERT(div != 0);

  {
    int padding_top = css_get_value_int(div, "padding-top");
    int padding_right = css_get_value_int(div, "padding-right");
    int padding_bottom = css_get_value_int(div, "padding-bottom");
    int padding_left = css_get_value_int(div, "padding-left");

    ASSERT(padding_top == 25);
    ASSERT(padding_right == 50);
    ASSERT(padding_bottom == 75);
    ASSERT(padding_left == 100);
  }

  CSSAST* span = css_get_rule(style, "span");
  ASSERT(span != 0);
  ASSERT(span != div);

  {
    int padding_top = css_get_value_int(span, "padding-top");
    ASSERT(padding_top == 8);
  }
}

void test_margin_css() {
  WHOAMI();
  char* contents = read_file("sources/margin.css");
  CSSAST* style = css(contents);
  ASSERT(style != 0);


  CSSAST* div = css_get_rule(style, "div");
  ASSERT(div != 0);

  {
    int margin_top = css_get_value_int(div, "margin-top");
    int margin_right = css_get_value_int(div, "margin-right");
    int margin_bottom = css_get_value_int(div, "margin-bottom");
    int margin_left = css_get_value_int(div, "margin-left");

    ASSERT(margin_top == 25);
    ASSERT(margin_right == 50);
    ASSERT(margin_bottom == 75);
    ASSERT(margin_left == 100);
  }

  CSSAST* span = css_get_rule(style, "span");
  ASSERT(span != 0);
  ASSERT(span != div);

  {
    int margin_top = css_get_value_int(span, "margin-top");
    ASSERT(margin_top == 8);
  }
}

int main(int argc, char* argv[]) {
  test_color_css();
  test_wildcard_css();
  test_id_css();
  test_media_query_css();
  test_style_big_css();
  test_font_face_css();
  test_font_face2_css();
  test_iterator_font_face2_css();
  test_bigbig_css();
  test_news_css();
  test_mystyle_css();
  test_padding_css();
  test_margin_css();
  return 0;
}
