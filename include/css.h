#ifndef XCSS_L_H
#define XCSS_L_H
#ifdef __cplusplus
extern "C" {
#endif

#include <AST.h>
#include <lexer.h>
#include <parser.h>

CSSAST *css(char *value);
CSSAST *css_anon(char *value);

void css_get_rules(CSSAST *ast, List *items);
void css_get_declarations(CSSAST *ast, List *items);

CSSAST *css_get_value(CSSAST *ast, char *key);

char *css_get_value_string(CSSAST *ast, char *key);

void css_set_value_string(CSSAST *ast, char *key, char *value);
void css_set_value_int(CSSAST *ast, char *key, float value);
void css_set_value_float(CSSAST *ast, char *key, float value);

const char *css_crayola_to_hex(char *name);

int css_get_value_int(CSSAST *ast, char *key);
float css_get_value_float(CSSAST *ast, char *key);

void css_free(CSSAST *css);

CSSAST *css_get_rule(CSSAST *css, char *selector);
#ifdef __cplusplus
}
#endif
#endif
