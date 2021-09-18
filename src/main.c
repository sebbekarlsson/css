#include <css.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  CSS *cssdata = css("div {"
                     "  background-color: red;"
                     "  color: black;"
                     "  top: 10px;"
                     "}");

  List *items = init_list(sizeof(CSSAST *));
  get_rules(cssdata, items);

  for (int i = 0; i < items->size; i++) {

    CSSAST *rule = list_at(items, i);
    CSSAST *value = css_get_value(rule, "top");
    printf("%12.6f\n", (float)value->value_int);
  }

  list_free(items);

  css_free(cssdata);

  return 0;
}
