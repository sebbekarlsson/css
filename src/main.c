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

  CSSAST *cssdata = css(css_read_file("tests/style_big.css"));

  CSSAST *rule = css_get_rule(cssdata, "slot");

  printf("%p\n", rule);
  css_free(cssdata);

  return 0;
}
