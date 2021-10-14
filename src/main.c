#include <css.h>
#include <io.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
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

  CSSAST *rule = css_get_rule(cssdata, "input[type=\"text\"]");

  if (rule) {
    char *s = css_get_value_string(rule, "color");

    if (s) {
      printf("%s\n", s);
    }

    printf("%p\n", rule);
  }

  css_free(cssdata);

  return 0;
}
