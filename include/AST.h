#ifndef CSS_AST_H
#define CSS_AST_H
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
#endif
