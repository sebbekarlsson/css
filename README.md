# css parser
> CSS parsing library


## Usage

### Basic usage
``` C
#include <css.h>
#include <stdio.h>


int main(int argc, char* argv[]) {
    CSSAST* parsed = css("div { color: red; width: 32px; height: 32px; }");
    
    CSSAST* rule = css_get_rule(parsed, "div");
    CSSColor color = css_get_value_color(rule, "color");

    printf("%1.2f, %1.2f, %1.2f %1.2f\n", color.r, color.g, color.b, color.a);
    
    css_free(parsed);
    
    return 0;
}
```

### Parsing font faces

``` C
int main(int argc, char *argv[]) {
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
```



## Building it
> Run:

``` bash
    mkdir -p build ; cd build ; cmake .. && make
```
