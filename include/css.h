#include <lexer.h>
#include <parser.h>
#include <AST.h>

CSSAST *css(char *value);

void css_get_rules(CSSAST *ast, List *items);
void css_get_declarations(CSSAST *ast, List *items);

CSSAST *css_get_value(CSSAST *ast, char *key);

char *css_get_value_string(CSSAST *ast, char *key);
int css_get_value_int(CSSAST *ast, char *key);
float css_get_value_float(CSSAST *ast, char *key);

void css_free(CSSAST *css);

CSSAST *css_get_rule(CSSAST *css, char *selector);
