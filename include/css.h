#ifndef CSS_H
#define CSS_H
#include <lexer.h>
#include <parser.h>
CSS *css(char *value);

void css_get_rules(CSS *ast, List *items);
void css_get_declarations(CSS *ast, List *items);

CSS *css_get_value(CSS *ast, char *key);

char *css_get_value_string(CSS *ast, char *key);
int css_get_value_int(CSS *ast, char *key);
float css_get_value_float(CSS *ast, char *key);

void css_free(CSS *css);

CSS *css_get_rule(CSS *css, char *selector);
#endif
