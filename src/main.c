#include <css.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  CSSAST* a = css(".box { background-color: red; width: 33px; } div { color: green; } div:hover { color: blue; }");
  List* results = css_query(a, ".box");

  if (results && results->size) {
    CSSAST* r = (CSSAST*)results->items[0];

    float x = css_get_value_float(r, "width");
    float value = css_get_value_float_computed(r, "width", (CSSContext){ 0, 0 });
    printf("%12.6f\n", value);
  }
  //CSSAST* a = css_anon("color: red; width: 32px; height: 32px");
  //CSSAST* b = css_anon("color: blue; width: 32px; height: 32px");
  //CSSAST* c = css_merge(a, b);

  //char* color = css_get_value_string(c, "color");

  //printf("%s\n", color);

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

  List* results = css_query(cssdata, "slot");

  printf("%d\n", (int)results->size);


  return 0;
}
