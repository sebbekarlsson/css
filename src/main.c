#include <css.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  CSS *cssdata = css("{"
                     "  background-color: red;"
                     "  color: black;"
                     "  top: 10px;"
                     "}");

  CSSAST *val = css_get_value(cssdata, "color");
  printf("%s\n", val->value_str);
  // CSSAST *rule = css_get_rule(cssdata, "div");
  // printf("%s\n", css_ast_selector_to_string(rule));

  css_free(cssdata);

  return 0;
}
