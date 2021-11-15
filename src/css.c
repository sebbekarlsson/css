#include <crayola.h>
#include <css.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <token.h>
#include <stdio.h>
#include <assert.h>

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
    css_list_append(items, ast);
  }

  if (ast->children != 0) {
    for (int i = 0; i < ast->children->size; i++) {
      css_get_rules(css_list_at(ast->children, i), items);
    }
  }
}

void css_get_declarations(CSSAST *ast, List *items) {

  if (ast->type == CSS_AST_DECL) {
    css_list_append(items, ast);
  }

  if (ast->children != 0) {
    for (int i = 0; i < ast->children->size; i++) {
      css_get_declarations(css_list_at(ast->children, i), items);
    }
  }
}

CSSAST *css_get_rule(CSSAST *css, char *selector) {
  CSSAST *rule = 0;
  if (css->children != 0) {
    for (int i = 0; i < css->children->size; i++) {
      CSSAST *child = css_list_at(css->children, i);
      if (child->type != CSS_AST_RULE)
        continue;
      char *selectorstr = ast_to_string(child);
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
  map_bucket_T *bucket = map_get(ast->keyvalue, key);
  if (bucket != 0) {
    return bucket->value;
  }
  CSSAST *value_ast = 0;
  List *declarations = init_css_list(sizeof(CSSAST *));
  css_get_declarations(ast, declarations);

  for (int i = 0; i < declarations->size; i++) {
    CSSAST *decl = css_list_at(declarations, i);
    if (!decl->left)
      continue;
    CSSAST *left = decl->left;
    if (!left->value_str)
      continue;

    if (strcmp(left->value_str, key) == 0) {
      value_ast = decl->right;
    }
  }
  css_list_free(declarations);
  return value_ast;
}

void css_set_value_string(CSSAST *ast, char *key, char *value) {
  if (ast == 0 || key == 0)
    return;

  CSSAST *val = css_get_value(ast, key);
  if (!val) {
    if (ast->children == 0)
      ast->children = init_css_list(sizeof(CSSAST *));

    val = init_css_ast(CSS_AST_DECL);
    CSSAST *left = init_css_ast(CSS_AST_ID);
    left->value_str = strdup(key);
    val->left = left;
    CSSAST *right = init_css_ast(CSS_AST_STR);
    right->value_str = value != 0 ? strdup(value) : 0;
    val->right = right;

    css_list_append(ast->children, val);
    map_set(ast->keyvalue, key, val);
  } else {
    if (val->value_str)
      free(val->value_str);
    val->value_str = strdup(value);
  }
}
void css_set_value_float(CSSAST *ast, char *key, float value) {
  if (ast == 0 || key == 0)
    return;

  CSSAST *val = css_get_value(ast, key);
  if (!val) {
    if (ast->children == 0)
      ast->children = init_css_list(sizeof(CSSAST *));

    val = init_css_ast(CSS_AST_DECL);
    CSSAST *left = init_css_ast(CSS_AST_ID);
    left->value_str = strdup(key);
    val->left = left;
    CSSAST *right = init_css_ast(CSS_AST_STR);
    right->value_float = value;
    right->value_double = (double)value;
    right->value_float = (float)value;
    right->value_int = (int)value;
    val->right = right;

    css_list_append(ast->children, val);
    map_set(ast->keyvalue, key, val);
  } else {
    val->value_float = value;
    val->value_double = (double)value;
    val->value_int = (int)value;
  }
}

void css_set_value_int(CSSAST *ast, char *key, float value) {
  if (ast == 0 || key == 0)
    return;

  CSSAST *val = css_get_value(ast, key);
  if (!val) {
    if (ast->children == 0)
      ast->children = init_css_list(sizeof(CSSAST *));

    val = init_css_ast(CSS_AST_BINOP);
    CSSAST *left = init_css_ast(CSS_AST_ID);
    left->value_str = strdup(key);
    val->left = left;
    CSSAST *right = init_css_ast(CSS_AST_INT);
    right->value_double = (double)value;
    right->value_float = (float)value;
    right->value_int = (int)value;
    val->right = right;

    css_list_append(ast->children, val);
    map_set(ast->keyvalue, key, val);
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

  if (css->value_str) {
    free(css->value_str);
  }
  if (css->children != 0) {
    for (int i = 0; i < css->children->size; i++) {
      css_free(css_list_at(css->children, i));
    }

    css_list_free(css->children);
  }

  if (css->rule_selectors != 0) {
    for (int i = 0; i < css->rule_selectors->size; i++) {
      css_free(css_list_at(css->rule_selectors, i));
    }

    css_list_free(css->rule_selectors);
  }

  if (css->keyvalue != 0) {
    map_free(css->keyvalue);
  }

  free(css);
}

unsigned int css_value_is_set(CSSAST* ast, char* key) {
  return css_get_value(ast, key) != 0;
}

static List *css_copy_css_list(List *incss_list) {
  if (!incss_list || (incss_list && incss_list->size == 0))
    return 0;
  List *newcss_list = init_css_list(incss_list->item_size);

  for (uint32_t i = 0; i < incss_list->size; i++) {
    CSSAST *child = incss_list->items[i];
    if (child == 0)
      continue;

    css_list_append(newcss_list, css_copy(child));
  }

  return newcss_list;
}

CSSAST *css_copy(CSSAST *css) {
  if (css == 0)
    return 0;
  CSSAST *ast = init_css_ast(css->type);
  if (css->value_str)
    ast->value_str = strdup(css->value_str);
  ast->value_double = css->value_double;
  ast->value_float = css->value_float;
  ast->value_int = css->value_int;
  ast->args = css_copy_css_list(css->args);
  ast->children = css_copy_css_list(css->children);
  ast->rule_selectors = css_copy_css_list(css->rule_selectors);
  ast->left = css_copy(css->left);
  ast->right = css_copy(css->right);
  ast->token = css_token_clone(css->token);
  ast->is_important = css->is_important;
  ast->unit = css->unit;
  css_reindex(ast);
  return ast;
}

void css_reindex(CSSAST *css) {
  List *declarations = init_css_list(sizeof(CSSAST *));
  css_get_declarations(css, declarations);

  for (int i = 0; i < declarations->size; i++) {
    CSSAST *decl = css_list_at(declarations, i);
    if (!decl->left)
      continue;
    CSSAST *left = decl->left;
    if (!left->value_str)
      continue;

    map_set(css->keyvalue, left->value_str, decl->right);
  }

  css_list_free(declarations);
}

const char *css_crayola_to_hex(char *name) {
  for (uint32_t i = 0; i < (uint32_t)CRAYOLA_LENGTH; i += 2) {
    const char *k = CRAYOLA[(int)MIN(CRAYOLA_LENGTH - 1, i)];
    const char *v = CRAYOLA[(int)MIN(CRAYOLA_LENGTH - 1, i + 1)];
    if (strcmp(k, name) == 0) {
      return v;
    }
  }

  return 0;
}

ECSSDisplay css_to_display(char *value) {
  if (value == 0)
    return CSS_DISPLAY_AUTO;

  if (strcmp(value, "none") == 0)
    return CSS_DISPLAY_NONE;
  if (strcmp(value, "auto") == 0)
    return CSS_DISPLAY_AUTO;
  if (strcmp(value, "block") == 0)
    return CSS_DISPLAY_BLOCK;
  if (strcmp(value, "inline-block") == 0)
    return CSS_DISPLAY_INLINE_BLOCK;
  if (strcmp(value, "inline-flex") == 0)
    return CSS_DISPLAY_INLINE_FLEX;
   if (strcmp(value, "inline") == 0)
    return CSS_DISPLAY_INLINE;
  if (strcmp(value, "table-cell") == 0)
    return CSS_DISPLAY_TABLE_CELL;
  if (strcmp(value, "flex") == 0)
    return CSS_DISPLAY_FLEX;
  if (strcmp(value, "grid") == 0)
    return CSS_DISPLAY_GRID;
  return CSS_DISPLAY_AUTO;
}

ECSSFlexDirection css_to_flex_direction(char *value) {
  if (value == 0)
    return CSS_FLEX_DIRECTION_ROW;
  if (strcmp(value, "row") == 0)
    return CSS_FLEX_DIRECTION_ROW;
  if (strcmp(value, "column") == 0)
    return CSS_FLEX_DIRECTION_COLUMN;
  return CSS_FLEX_DIRECTION_ROW;
}

ECSSFlexAlign css_to_flex_align(char *value) {
  if (value == 0)
    return CSS_FLEX_ALIGN_BEGIN;
  if (strcmp(value, "flex-begin") == 0)
    return CSS_FLEX_ALIGN_BEGIN;
  if (strcmp(value, "flex-end") == 0)
    return CSS_FLEX_ALIGN_END;
  if (strcmp(value, "flex-center") == 0)
    return CSS_FLEX_ALIGN_CENTER;
  if (strcmp(value, "center") == 0)
    return CSS_FLEX_ALIGN_CENTER;
  return CSS_FLEX_ALIGN_BEGIN;
}

ECSSTextAlign css_to_text_align(char *value) {
    if (value == 0)
    return CSS_TEXT_ALIGN_LEFT;
  if (strcmp(value, "left") == 0)
    return CSS_TEXT_ALIGN_LEFT;
  if (strcmp(value, "RIGHT") == 0)
    return CSS_TEXT_ALIGN_RIGHT;
  if (strcmp(value, "center") == 0)
    return CSS_TEXT_ALIGN_CENTER;
  return CSS_TEXT_ALIGN_LEFT;
}

ECSSPosition css_to_position(char *value) {
  if (value == 0)
    return CSS_POSITION_AUTO;
  if (strcmp(value, "auto") == 0)
    return CSS_POSITION_AUTO;
  if (strcmp(value, "absolute") == 0)
    return CSS_POSITION_ABSOLUTE;
  if (strcmp(value, "relative") == 0)
    return CSS_POSITION_RELATIVE;
  return CSS_POSITION_AUTO;
}

ECSSDisplay css_get_value_display(CSSAST* ast, const char* key) {
  char* str = css_get_value_string(ast, (char *)key);
  return css_to_display(str);
}

ECSSFlexAlign css_get_value_flex_align(CSSAST* ast, const char* key) {
  char* str = css_get_value_string(ast, (char *)key);
  return css_to_flex_align(str);
}

ECSSPosition css_get_value_position(CSSAST* ast, const char* key){
  char* str = css_get_value_string(ast, (char *)key);
  return css_to_position(str);
}

static const char *ensure_hex(char *value) {
  const char *v = (const char *)value;
  if (v == 0)
    return 0;
  if (strchr(v, '#') == 0)
    v = css_crayola_to_hex(value);
  return v;
}

CSSColor css_hex_to_color(char *value) {
  const char *v = ensure_hex(value);
  if (v == 0)
    return (CSSColor){-1, -1, -1, -1};
  if (v[0] == '#')
    v++;
  unsigned int hexvalue = strtol(v, 0, 16);
  return (CSSColor){((hexvalue >> 16) & 0xFF), ((hexvalue >> 8) & 0xFF),
                    ((hexvalue)&0xFF), 1};
}

CSSColor css_value_to_color(CSSAST *ast, const char *key) {
  CSSAST *val = css_get_value(ast, (char *)key);

  if (val && val->args && val->args->size >= 3) {
    CSSAST *_r = (CSSAST *)css_list_at(val->args, 0);
    CSSAST *_g = (CSSAST *)css_list_at(val->args, 1);
    CSSAST *_b = (CSSAST *)css_list_at(val->args, 2);

    float r = css_ast_get_float(_r);
    float g = css_ast_get_float(_g);
    float b = css_ast_get_float(_b);
    float a = 1.0f;

    if (val->args->size >= 4) {
      CSSAST *_a = (CSSAST *)css_list_at(val->args, 3);

      a = css_ast_get_float(_a);
    }

    return (CSSColor){r, g, b, a};
  } else if (val && val->value_str) {
    return css_hex_to_color(val->value_str);
  }
  return (CSSColor){-1, -1, -1, -1};
}

void assert_is_decl(CSSAST* ast) {
  assert(ast != 0);
  assert(ast->type == CSS_AST_DECL);
  assert(ast->left != 0);
  assert(ast->right != 0);
  assert(ast->left->value_str != 0);
}

void css_unset_value(CSSAST* ast, char* key) {
  int size = (int)ast->children->size;

  for (int i = 0; i < size; i++) {
    CSSAST* child = (CSSAST*)ast->children->items[i];
    assert_is_decl(child);

    if (strcmp(child->left->value_str, key) == 0)
      css_list_remove(ast->children, child, (void(*)(void*))css_free);
  }

  map_unset(ast->keyvalue, key);
}

void css_add_decl(CSSAST* ast, CSSAST* decl) {
  assert(ast != 0);
  assert_is_decl(decl);
  assert(ast->keyvalue != 0);


  CSSAST* existing = (CSSAST*)css_get_value(ast, decl->left->value_str);
  if (existing && existing->is_important) {
    return;
  } else if (existing != 0) {
    css_unset_value(ast, decl->left->value_str);
  }

  CSSAST* newdecl = css_copy(decl);

  css_list_append(ast->children, newdecl);
  map_set(ast->keyvalue, newdecl->left->value_str, newdecl->right);
}

CSSAST* css_merge(CSSAST* a, CSSAST* b) {
  if (a->type != CSS_AST_RULE || b->type != CSS_AST_RULE) {
    fprintf(stderr, "CSS: Can only merge rules.\n");
  }
  for (int i = 0; i < (int)b->children->size; i++) {
    CSSAST* child = (CSSAST*)b->children->items[i];
    css_add_decl(a, child);
  }

  return a;
}

CSSColor css_get_value_color(CSSAST *ast, const char *key) {
  return css_value_to_color(ast, key);
}

ECSSTextAlign css_get_value_align(CSSAST* ast, const char* key) {
  char* str = css_get_value_string(ast, (char*)key);
  return css_to_text_align(str);
}


static void _css_query(CSSAST* cssnode, const char* selector, List* list) {
  char* str = ast_to_string(cssnode);

  if (str) {
    if (strcmp(str, selector) == 0) css_list_append(list, cssnode);
  }

  if (cssnode->children) {
    for (int i = 0; i < (int)cssnode->children->size; i++) {
      CSSAST* child = (CSSAST*)cssnode->children->items[i];
      _css_query(child, selector, list);
    }
  }
}

List* css_query(CSSAST* cssnode, const char* selector) {
  List* list = init_css_list(sizeof(CSSAST*));

  _css_query(cssnode, selector, list);

  return list;
}


static float percentage(float x, float y) {
  return (x / 100.0f) * y;
}

float css_get_value_float_computed(CSSAST* ast, const char* key, CSSContext context) {
  CSSAST* value = css_get_value(ast, (char*)key);
  if (value == 0) return 0;

  float x = css_get_value_float(ast, (char*)key);

  float z = x;

  switch (value->unit) {
    case CSS_UNIT_PERCENT: z = percentage(x, context.size); break;
    case CSS_UNIT_REM: z = x * context.rem; break;
    case CSS_UNIT_EM: z = x * context.em; break;
    case CSS_UNIT_PX: z = x; break;
    case CSS_UNIT_VH: z = percentage(x, context.vh); break;
    case CSS_UNIT_VW: z = percentage(x, context.vh); break;
    default: { z = x; } break;
  }

  return z;
}


ECSSValueType css_get_value_type(CSSAST* ast, const char* key) {
  char* str = css_get_value_string(ast, (char*)key);
  if (str == 0) return CSS_UNSPECIFIED;

  if (strcmp(str, "auto") == 0) return CSS_AUTO;
  if (strcmp(str, "none") == 0) return CSS_NONE;

  return CSS_UNSPECIFIED;
}

ECSSUnit css_get_value_unit(CSSAST* ast, const char* key) {
  CSSAST* value = css_get_value(ast, (char*)key);
  if (value == 0) return CSS_UNIT_PX;

  return value->unit;
}
