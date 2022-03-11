#include <stringify.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>


char* css_stringify(CSSAST* node) {
  if (!node) return strdup("");
  switch (node->type) {
    case CSS_AST_ARRAY: return css_stringify_array(node); break;
    case CSS_AST_INT: return css_stringify_int(node); break;
    case CSS_AST_FLOAT: return css_stringify_float(node); break;
    case CSS_AST_DOUBLE: return css_stringify_double(node); break;
    case CSS_AST_ID: return css_stringify_id(node); break;
    case CSS_AST_CLASSNAME: return css_stringify_classname(node); break;
    case CSS_AST_PSEUDO_CLASS: return css_stringify_pseudo_class(node); break;
    case CSS_AST_RULE: return css_stringify_rule(node); break;
    case CSS_AST_SELECTOR: return css_stringify_selector(node); break;
    case CSS_AST_DECL: return css_stringify_declaration(node); break;
    case CSS_AST_BINOP: return css_stringify_binop(node); break;
    case CSS_AST_UNOP: return css_stringify_unop(node); break;
    case CSS_AST_STATEMENT: return css_stringify_statement(node); break;
    case CSS_AST_CALL: return css_stringify_call(node); break;
    case CSS_AST_OPT: return css_stringify_opt(node); break;
    case CSS_AST_COMPOUND: return css_stringify_compound(node); break;
    case CSS_AST_STR: return css_stringify_str(node); break;
    default: { return strdup(node->value_str ? node->value_str : ""); }; break;
  }

  return strdup("");
}
char* css_stringify_int(CSSAST* node){
  char buff[PATH_MAX];
  sprintf(buff, "%d", node->value_int);
  return strdup(buff);
}
char* css_stringify_str(CSSAST* node) {
  if (!node->value_str) return strdup("");
  return strdup(node->value_str);
}
char* css_stringify_float(CSSAST* node){
  char buff[PATH_MAX];
  sprintf(buff, "%1.1f", node->value_float);
  return strdup(buff);
}
char* css_stringify_double(CSSAST* node){
  char buff[PATH_MAX];
  sprintf(buff, "%1.1f", node->value_double);
  return strdup(buff);
}
char* css_stringify_id(CSSAST* node){
  return css_stringify_str(node);
}
char* css_stringify_classname(CSSAST* node){
  return ast_to_string(node);
}
char* css_stringify_pseudo_class(CSSAST* node){
  return ast_to_string(node);
}
char* css_stringify_rule(CSSAST* node){
  char* str = 0;
  char* selector = ast_to_string(node);

  if (selector) {

    str_append(&str, selector);
    str_append(&str, " ");
  }
    str_append(&str, "{\n");

  if (!node->children || node->children->size <= 0) return ast_to_string(node);

  for (uint32_t i = 0; i < node->children->size; i++) {
    CSSAST* child = (CSSAST*)node->children->items[i];
    if (!child) continue;
    char* child_str = css_stringify(child);
    if (!child_str) continue;

    str_append(&str, child_str);

    if (i <= node->children->size-1) {
      str_append(&str, "\n");
    }
  }
  str_append(&str, "\n}");

  return str;

}
char* css_stringify_selector(CSSAST* node){
  return ast_to_string(node);
}
char* css_stringify_declaration(CSSAST* node){
  char* str = 0;
  if (node->left && node->right) {
    char* leftstr = css_stringify(node->left);
    str_append(&str, leftstr);
    str_append(&str, ": ");
    char* rightstr = css_stringify(node->right);
    str_append(&str, rightstr);
    str_append(&str, ";");
  }

  return str == 0 ? strdup("") : str;
}
char* css_stringify_binop(CSSAST* node){
  return ast_to_string(node);
}
char* css_stringify_unop(CSSAST* node){
  return ast_to_string(node);
}
char* css_stringify_statement(CSSAST* node){
  return ast_to_string(node);
}
char* css_stringify_call(CSSAST* node){
  return ast_to_string(node);
}
char* css_stringify_opt(CSSAST* node){
  return ast_to_string(node);
}
char* css_stringify_compound(CSSAST* node){
  char* str = 0;

  if (!node->children) return strdup("");
  if (node->children->size <= 0) return strdup("");

  for (uint32_t i = 0; i < node->children->size; i++) {
    CSSAST* child = (CSSAST*)node->children->items[i];
    if (!child) continue;
    char* child_str = css_stringify(child);
    if (!child_str) continue;

    str_append(&str, child_str);

     if (i >= node->children->size-1) {
      str_append(&str, "\n");
    }
  }

  return str == 0 ? strdup("") : str;
}
char* css_stringify_array(CSSAST* node) {
    char* str = 0;
    str_append(&str, "[");

  for (uint32_t i = 0; i < node->children->size; i++) {
    CSSAST* child = (CSSAST*)node->children->items[i];
    if (!child) continue;
    char* child_str = css_stringify(child);
    if (!child_str) continue;

    str_append(&str, child_str);
  }

  str_append(&str, "]");

  return str;
}
