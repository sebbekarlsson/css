#include <AST.h>
#include <list.h>
#include <mem.h>
#include <parser.h>
#include <string.h>
#include <token.h>
#include <utils.h>

CSSAST *init_css_ast(int type) {
  CSSAST *ast = NEW(CSSAST);
  ast->type = type;
  ast->children = 0;

  return ast;
}

char *ast_to_string(CSSAST *ast) {
  switch (ast->type) {
  case CSS_AST_BINOP:
    return ast_binop_to_string(ast);
    break;
  default: {
    return ast->value_str ? strdup(ast->value_str) : strdup("");
  } break;
  }

  return strdup("");
}
char *ast_binop_to_string(CSSAST *ast) {
  char *v = 0;

  if (ast->left) {
    char *strv = ast_to_string(ast->left);
    str_append(&v, strv);
    free(strv);
  }

  if (ast->token && ast->token->value)
    str_append(&v, ast->token->value);

  if (ast->right) {
    char *strv = ast_to_string(ast->right);
    str_append(&v, strv);
    free(strv);
  }

  return v ? v : strdup("");
}

char *css_ast_selector_to_string(CSSAST *ast) {
  char *value = 0;

  if (!ast->rule_selectors)
    return value;

  for (int i = 0; i < ast->rule_selectors->size; i++) {
    char *v = ast_to_string(list_at(ast->rule_selectors, i));
    str_append(&value, v);
    free(v);
  }

  return value ? value : strdup("");
}

float css_ast_get_float(CSSAST* ast) {
  if (!ast) return 0.0f;
  return ast->value_float ? ast->value_float : ast->value_double ? ast->value_double : ast->value_int;
}
