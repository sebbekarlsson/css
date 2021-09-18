#include <AST.h>
#include <list.h>
#include <mem.h>
#include <string.h>
#include <token.h>

CSSAST *init_css_ast(int type) {
  CSSAST *ast = NEW(CSSAST);
  ast->type = type;
  ast->children = 0;

  return ast;
}

void get_rules(CSSAST *ast, List *items) {

  if (ast->type == CSS_AST_RULE) {
    list_append(items, ast);
  }

  if (ast->children != 0) {
    for (int i = 0; i < ast->children->size; i++) {
      get_rules(list_at(ast->children, i), items);
    }
  }
}

void get_declarations(CSSAST *ast, List *items) {

  if (ast->type == CSS_AST_DECL) {
    list_append(items, ast);
  }

  if (ast->children != 0) {
    for (int i = 0; i < ast->children->size; i++) {
      get_declarations(list_at(ast->children, i), items);
    }
  }
}

CSSAST *css_get_value(CSSAST *ast, char *key) {
  CSSAST *value_ast = 0;
  List *declarations = init_list(sizeof(CSSAST *));
  get_declarations(ast, declarations);

  for (int i = 0; i < declarations->size; i++) {
    CSSAST *decl = list_at(declarations, i);
    if (!decl->left)
      continue;
    CSSAST *left = decl->left;
    if (!left->value_str)
      continue;

    if (strcmp(left->value_str, key) == 0) {
      value_ast = decl->right;
    }
  }
  list_free(declarations);
  return value_ast;
}

void css_free(CSS *css) {
  if (css->token) {
    css_token_free(css->token);
  }
  if (css->left)
    css_free(css->left);
  if (css->right)
    css_free(css->right);
  if (css->unit)
    free(css->unit);

  if (css->value_str) {
    free(css->value_str);
  }
  if (css->children != 0) {
    for (int i = 0; i < css->children->size; i++) {
      css_free(list_at(css->children, i));
    }

    list_free(css->children);
  }

  if (css->rule_selectors != 0) {
    for (int i = 0; i < css->rule_selectors->size; i++) {
      css_free(list_at(css->rule_selectors, i));
    }

    list_free(css->rule_selectors);
  }

  free(css);
}
