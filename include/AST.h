#ifndef XCSS_AST_H
#define XCSS_AST_H
#include <hashmap/map.h>
#include <css_list.h>
#include <token.h>

typedef struct CSS_AST_STRUCT {
  List *children;
  List *rule_selectors;
  List *args;

  enum {
    CSS_AST_INT,
    CSS_AST_STR,
    CSS_AST_FLOAT,
    CSS_AST_DOUBLE,
    CSS_AST_ID,
    CSS_AST_COMPOUND,
    CSS_AST_RULE,
    CSS_AST_SELECTOR,
    CSS_AST_DECL,
    CSS_AST_BINOP,
    CSS_AST_STATEMENT,
    CSS_AST_CALL,
    CSS_AST_OPT,
    CSS_AST_UNOP,
    CSS_AST_ARRAY
  } type;

  struct CSS_AST_STRUCT *left;
  struct CSS_AST_STRUCT *right;
  CSSToken *token;
  char *unit;

  int value_int;
  char *value_str;
  float value_float;
  double value_double;
  map_T *keyvalue;
} CSSAST;

CSSAST *init_css_ast(int type);

char *ast_to_string(CSSAST *ast);
char *ast_binop_to_string(CSSAST *ast);
char *css_ast_selector_to_string(CSSAST *ast);

CSSAST *css(char *value);

void css_get_rules(CSSAST *ast, List *items);
void css_get_declarations(CSSAST *ast, List *items);

CSSAST *css_get_value(CSSAST *ast, char *key);

char *css_get_value_string(CSSAST *ast, char *key);
int css_get_value_int(CSSAST *ast, char *key);
float css_get_value_float(CSSAST *ast, char *key);

void css_free(CSSAST *css);

CSSAST *css_get_rule(CSSAST *css, char *selector);
float css_ast_get_float(CSSAST *ast);

#endif
