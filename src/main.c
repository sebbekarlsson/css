#include <css.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  CSS *cssdata = css("div {"
                     "  background-color: red;"
                     "  color: black;"
                     "  top: 10px;"
                     "}");


  CSSAST *rule = css_get_rule(cssdata, "div");
  printf("%s\n", css_ast_selector_to_string(rule));

  css_free(cssdata);

  return 0;
}
