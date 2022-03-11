#include <css.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stringify.h>

int main(int argc, char *argv[]) {
  CSSAST *a = css(".box { background-color: red; width: 33px; } div { color: "
                  "green; } div:hover { color: blue; }");

  CSSAST* b = css(".button { color: red; } .box { position: absolute; } div { box-sizing: border-box; color: purple; }");


  CSSAST* c = css_anon("color: red");
  CSSAST* d = css_anon("color: blue; position: absolute;");
  CSSAST* m = css_merge(a, b);
  CSSAST* m2 = css_merge(c, d);

  printf("--------------- 1 -----------------\n");
  printf("%s\n", css_stringify(m));


  printf("--------------- 2 -----------------\n");
  printf("%s\n", css_stringify(m2));

  CSSAST* unwrapped = css_unwrap(m);

  printf("--------------- 3 -----------------\n");
  printf("%s\n", css_stringify(unwrapped));

  printf("--------------- 4 -----------------\n");
  CSSNode* e = css_anon("top: 0");
  CSSNode* m3 = css_merge(unwrapped, e);
  printf("%s\n", css_stringify(m3));


  printf("COLOR: %s\n", css_get_value_string(m3, "color"));

  // CSSAST* a = css_anon("color: red; width: 32px; height: 32px");
  // CSSAST* b = css_anon("color: blue; width: 32px; height: 32px");
  // CSSAST* c = css_merge(a, b);

  // char* color = css_get_value_string(c, "color");

  // printf("%s\n", color);

  return 0;
}

int main_old(int argc, char *argv[]) {
  CSSAST *parsed =
      css("@font-face { font-family: 'Product Sans';  font-style: normal;  "
          "font-weight: 400;  src: "
          "url(https://fonts.gstatic.com/s/productsans/v18/"
          "pxiDypQkot1TnFhsFMOfGShVE9eOcEg.woff2) format('woff2');  "
          "unicode-range: U+0400-045F, U+0490-0491, U+04B0-04B1, U+2116;}");

  CSSIterator it = css_get_rules(parsed, "@font-face");

  CSSAST *rule = 0;
  while ((rule = css_iterator_next(&it)) != 0) {
    CSSAST *call_node = css_get_value_call(rule, "src", "url");
    CSSAST *arg = (CSSAST *)call_node->args->items[0];
    printf("%s\n", arg->value_str);
  }

  css_free(parsed);

  return 0;
}

int main3(int argc, char *argv[]) {
  CSSAST *a = css(".box { background-color: red; width: 33px; } div { color: "
                  "green; } div:hover { color: blue; }");
  List *results = css_query(a, ".box");

  if (results && results->size) {
    CSSAST *r = (CSSAST *)results->items[0];

    float x = css_get_value_float(r, "width");
    float value = css_get_value_float_computed(r, "width", (CSSContext){0, 0});
    printf("%12.6f\n", value);
  }
  // CSSAST* a = css_anon("color: red; width: 32px; height: 32px");
  // CSSAST* b = css_anon("color: blue; width: 32px; height: 32px");
  // CSSAST* c = css_merge(a, b);

  // char* color = css_get_value_string(c, "color");

  // printf("%s\n", color);

  return 0;
}

int main2(int argc, char *argv[]) {
  /*CSSAST *cssdata = css("{"
                     "  background-color: red;"
                     "  color: #00FF00;"
                     "  top: 10px;"
                     "}");

  CSSAST *val = css_get_value(cssdata, "color");
  printf("%s\n", val->value_str);*/
  // CSSAST *rule = css_get_rule(cssdata, "div");
  // printf("%s\n", css_ast_selector_to_string(rule));

  //  css_free(cssdata);
  //
  // const char* hexcolor = css_crayola_to_hex("green");
  // if (hexcolor) {
  //  printf("%s\n", hexcolor);
  //}
  // return 0;

  char *g = css_read_file(argv[1]);
  CSSAST *cssdata = css(g);

  List *results = css_query(cssdata, "slot");

  printf("%d\n", (int)results->size);

  return 0;
}
