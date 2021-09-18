#include <css.h>
#include <string.h>

CSS *css(char *value) {
  CSSLexer *lexer = init_css_lexer(value);
  CSSParser *parser = init_css_parser(lexer);
  CSSAST *root = css_parser_parse(parser);

  css_lexer_free(lexer);
  css_parser_free(parser);

  return root;
}

void css_get_rules(CSSAST *ast, List *items) {

  if (ast->type == CSS_AST_RULE) {
    list_append(items, ast);
  }

  if (ast->children != 0) {
    for (int i = 0; i < ast->children->size; i++) {
      css_get_rules(list_at(ast->children, i), items);
    }
  }
}

void css_get_declarations(CSSAST *ast, List *items) {

  if (ast->type == CSS_AST_DECL) {
    list_append(items, ast);
  }

  if (ast->children != 0) {
    for (int i = 0; i < ast->children->size; i++) {
      css_get_declarations(list_at(ast->children, i), items);
    }
  }
}

CSS *css_get_rule(CSS *css, char *selector) {
  CSS *rule = 0;
  if (css->children != 0) {
    for (int i = 0; i < css->children->size; i++) {
      CSS *child = list_at(css->children, i);
      if (child->type != CSS_AST_RULE)
        continue;
      char *selectorstr = css_ast_selector_to_string(child);
      if (!selectorstr)
        continue;

      if (strcmp(selectorstr, selector) == 0) {
        rule = child;
        free(selectorstr);
        break;
      } else {
        free(selectorstr);
      }
    }
  }

  return rule;
}

CSSAST *css_get_value(CSSAST *ast, char *key) {
  CSSAST *value_ast = 0;
  List *declarations = init_list(sizeof(CSSAST *));
  css_get_declarations(ast, declarations);

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

char *css_get_value_string(CSS *ast, char *key) {
  CSSAST *val = css_get_value(ast, key);
  return val->value_str ? strdup(val->value_str) : 0;
}
int css_get_value_int(CSS *ast, char *key) {

  CSSAST *val = css_get_value(ast, key);
  return val->value_int;
}
float css_get_value_float(CSS *ast, char *key) {

  CSSAST *val = css_get_value(ast, key);

  return val->value_float ? val->value_float : val->value_double;
}
