#ifndef XCSS_AST_H
#define XCSS_AST_H
#include <list.h>
#include <token.h>

typedef struct {
  List *children;
  List *rule_selectors;

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
  } type;

  void *left;
  void *right;
  CSSToken *token;
  char *unit;

  int value_int;
  char *value_str;
  float value_float;
  double value_double;
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

#endif
