#include <AST.h>
#include <css_list.h>
#include <mem.h>
#include <parser.h>
#include <stdio.h>
#include <string.h>
#include <token.h>
#include <utils.h>

CSSAST *init_css_ast(int type) {
  CSSAST *ast = NEW(CSSAST);
  ast->type = type;
  ast->children = 0;
  ast->keyvalue = NEW_MAP();
  ast->unit = CSS_UNIT_PX;

  return ast;
}

char *ast_array_to_string(CSSAST *ast) {
  char *str = 0;

  if (ast->left) {
    char *leftstr = ast_to_string(ast->left);
    if (leftstr) {
      str_append(&str, leftstr);
      free(leftstr);
    }
  }

  str_append(&str, "[");
  if (ast->children && ast->children->size) {
    for (uint32_t i = 0; i < ast->children->size; i++) {
      CSSAST *child = (CSSAST *)ast->children->items[i];
      char *childstr = ast_to_string(child);

      if (childstr) {
        str_append(&str, childstr);
        free(childstr);
      }
    }
  }
  str_append(&str, "]");

  return str;
}

char *ast_call_to_string(CSSAST *ast) {
  char *str = 0;
  if (ast->args && ast->args->size) {
    for (uint32_t i = 0; i < ast->args->size; i++) {
      CSSAST *child = (CSSAST *)ast->args->items[i];
      char *childstr = ast_to_string(child);

      if (childstr) {
        str_append(&str, childstr);
        free(childstr);
      }
    }
  }

  return str ? str : strdup("");
}

char *ast_string_to_string(CSSAST *ast) {
  char *str = strdup("\"");
  char *value = ast->value_str;
  if (value) {
    str_append(&str, value);
  }
  str_append(&str, "\"");

  return str;
}

char *ast_to_string(CSSAST *ast) {
  switch (ast->type) {
  case CSS_AST_BINOP:
    return ast_binop_to_string(ast);
    break;
  case CSS_AST_ARRAY:
    return ast_array_to_string(ast);
    break;
  case CSS_AST_CALL:
    return ast_call_to_string(ast);
    break;

  case CSS_AST_STR:
    return ast_string_to_string(ast);
    break;
    case CSS_AST_RULE:
      return css_ast_selector_to_string(ast); break;
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
    char *v = ast_to_string(css_list_at(ast->rule_selectors, i));
    str_append(&value, v);
    free(v);
  }

  return value ? value : strdup("");
}

float css_ast_get_float(CSSAST *ast) {
  if (!ast)
    return 0.0f;
  return ast->value_float
             ? ast->value_float
             : ast->value_double ? ast->value_double : ast->value_int;
}
