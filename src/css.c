#include <css.h>
#include <string.h>

CSSAST *css(char *value) {
  CSSLexer *lexer = init_css_lexer(value);
  CSSParser *parser = init_css_parser(lexer);
  CSSAST *root = css_parser_parse(parser);

  css_lexer_free(lexer);
  css_parser_free(parser);

  return root;
}

CSSAST *css_anon(char *value) {
  CSSLexer *lexer = init_css_lexer(value);
  CSSParser *parser = init_css_parser(lexer);
  CSSAST *root = css_parser_parse_rule_anon(parser);

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

CSSAST *css_get_rule(CSSAST *css, char *selector) {
  CSSAST *rule = 0;
  if (css->children != 0) {
    for (int i = 0; i < css->children->size; i++) {
      CSSAST *child = list_at(css->children, i);
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

void css_set_value_string(CSSAST* ast, char* key, char* value) {
  if (ast == 0 || key == 0) return;

  CSSAST *val = css_get_value(ast, key);
  if (!val) {
    if (ast->children == 0) ast->children = init_list(sizeof(CSSAST*));

    val = init_css_ast(CSS_AST_BINOP);
    CSSAST* left = init_css_ast(CSS_AST_ID);
    left->value_str = strdup(key);
    val->left = left;
    CSSAST* right = init_css_ast(CSS_AST_STR);
    right->value_str = value != 0 ? strdup(value) : 0;

    list_append(ast->children, val);
  } else {
    if (val->value_str) free(val->value_str);
    val->value_str = strdup(value);
  }
}
void css_set_value_float(CSSAST* ast, char* key, float value) {
  if (ast == 0 || key == 0) return;

  CSSAST *val = css_get_value(ast, key);
  if (!val) {
    if (ast->children == 0) ast->children = init_list(sizeof(CSSAST*));

    val = init_css_ast(CSS_AST_BINOP);
    CSSAST* left = init_css_ast(CSS_AST_ID);
    left->value_str = strdup(key);
    val->left = left;
    CSSAST* right = init_css_ast(CSS_AST_STR);
    right->value_float = value;
    val->value_double = (double)value;
    val->value_float = (float)value;
    val->value_int = (int)value;

    list_append(ast->children, val);
  } else {
    val->value_float = value;
    val->value_double = (double)value;
    val->value_int = (int)value;
  }
}

void css_set_value_int(CSSAST* ast, char* key, float value) {
  if (ast == 0 || key == 0) return;

  CSSAST *val = css_get_value(ast, key);
  if (!val) {
    if (ast->children == 0) ast->children = init_list(sizeof(CSSAST*));

    val = init_css_ast(CSS_AST_BINOP);
    CSSAST* left = init_css_ast(CSS_AST_ID);
    left->value_str = strdup(key);
    val->left = left;
    CSSAST* right = init_css_ast(CSS_AST_STR);
    val->value_double = (double)value;
    val->value_float = (float)value;
    val->value_int = (int)value;

    list_append(ast->children, val);
  } else {
    val->value_float = (float)value;
    val->value_double = (double)value;
    val->value_int = (int)value;
  }
}


char *css_get_value_string(CSSAST *ast, char *key) {
  CSSAST *val = css_get_value(ast, key);
  if (!val)
    return 0;
  return val->value_str ? strdup(val->value_str) : 0;
}
int css_get_value_int(CSSAST *ast, char *key) {

  CSSAST *val = css_get_value(ast, key);
  if (!val)
    return 0;
  return val->value_int ? val->value_int : css_get_value_float(ast, key);
}
float css_get_value_float(CSSAST *ast, char *key) {

  CSSAST *val = css_get_value(ast, key);
  if (!val)
    return 0;

  return val->value_float ? val->value_float
                          : val->value_double ? (float)val->value_double
                                              : (float)val->value_int;
}

void css_free(CSSAST *css) {
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
