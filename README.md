# css parser
> CSS parsing library


## Usage
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


## Building it
> Run:

``` bash
    mkdir -p build ; cd build ; cmake .. && make
```
