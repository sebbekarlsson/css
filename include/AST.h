#ifndef XCSS_AST_H
#define XCSS_AST_H
#include <hashmap/map.h>
#include <css_list.h>
#include <token.h>

typedef enum {
CSS_UNIT_PX,
CSS_UNIT_PERCENT,
CSS_UNIT_REM,
CSS_UNIT_EM,
CSS_UNIT_PT,
CSS_UNIT_FR,
CSS_UNIT_VW,
CSS_UNIT_VH,
CSS_UNIT_DEG
} ECSSUnit;

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
    CSS_AST_CLASSNAME,
    CSS_AST_PSEUDO_CLASS,
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

  int value_int;
  char *value_str;
  float value_float;
  double value_double;
  map_T *keyvalue;
  unsigned int is_important;
  unsigned int escaped;
  ECSSUnit unit;
} CSSAST;

CSSAST *init_css_ast(int type);

char *ast_to_string(CSSAST *ast);
char *ast_binop_to_string(CSSAST *ast);
char *css_ast_selector_to_string(CSSAST *ast);

CSSAST *css(char *value);


void css_free(CSSAST *css);

float css_ast_get_float(CSSAST *ast);


#endif
