#include <css.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    CSSAST* parsed = css("div { color: red; width: 32px; height: 32px; }");

    CSSAST* rule = css_get_rule(parsed, "div");
    CSSColor color = css_get_value_color(rule, "color");


    printf("%1.2f, %1.2f, %1.2f %1.2f\n", color.r, color.g, color.b, color.a);

    css_free(parsed);

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

  CSSAST* copied = css_copy(cssdata);

  CSSAST *rule = css_get_rule(copied, "input");

  if (rule) {
    CSSColor color = css_get_value_color(rule, "background-color");

    printf("%1.2f %1.2f %1.2f %1.2f\n", color.r, color.g, color.b, color.a);
  }

  css_free(cssdata);
  css_free(copied);
  free(g);

  return 0;
}
