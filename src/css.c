#include <css.h>
#include <crayola.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <token.h>
#include <stdio.h>
#include <assert.h>
#include <iterator.h>

CSSNode *css(char *value) {
  CSSLexer *lexer = init_css_lexer(value);
  CSSParser *parser = init_css_parser(lexer);
  CSSNode *root = css_parser_parse(parser);

  css_lexer_free(lexer);
  css_parser_free(parser);

  return root;
}

CSSNode *css_anon(char *value) {
  CSSLexer *lexer = init_css_lexer(value);
  CSSParser *parser = init_css_parser(lexer);
  CSSNode *root = css_parser_parse_rule_anon(parser);

  css_lexer_free(lexer);
  css_parser_free(parser);

  return root;
}

CSSIterator css_get_rules(CSSNode *ast, const char* selector) {

  List* items = init_css_list(sizeof(CSSNode*));

  if (ast->type == CSS_AST_RULE) {
    css_list_append(items, ast);
  }

  int count = 0;
  if (ast->children != 0) {
    for (int i = 0; i < ast->children->size; i++) {

         CSSNode *child = css_list_at(ast->children, i);
      if (child->type != CSS_AST_RULE)
        continue;


      if (selector == 0) {
        css_list_append(items,child);
        continue;
      }

      char *selectorstr = ast_to_string(child);


      if (!selectorstr)
        continue;

      if (strcmp(selectorstr, selector) == 0) {
        css_list_append(items,child);
        count++;
        free(selectorstr);
      } else {
        free(selectorstr);
      }

    }
  }

  if (count <= 0) {
    return (CSSIterator) { .length = 0 };
  }

  CSSIterator it = css_iterate(items->size > 0 ? (CSSNode**)items->items : 0, items->size);
  free(items);
  return it;
}

char* css_get_decl_name(CSSNode* ast) {
  if (!ast) return 0;
  if (ast->left && ast->left->value_str != 0) return ast->left->value_str;
  if (ast->value_str != 0) return ast->value_str;
  return 0;
}

void css_get_declarations(CSSNode *ast, List *items, unsigned int copy) {
  if (!ast || !items) return;

  if (ast->type == CSS_AST_DECL) {
    css_list_append(items, ast);
  }



  if (ast->children != 0) {
    for (int i = 0; i < ast->children->size; i++) {
      CSSNode* child = css_list_at(ast->children, i);
      if (!child) continue;

      if (copy) {
       child = css_copy(child);
      }

      css_get_declarations(child, items, copy);
    }
  }
}

void css_get_declarations_filtered(CSSNode *ast, List *items, unsigned int copy, map_T* lookup) {
  if (!ast || !items) return;

  if (ast->type == CSS_AST_DECL) {
    css_list_append(items, ast);
  }

  if (ast->children != 0) {
    for (int i = 0; i < ast->children->size; i++) {
      CSSNode* child = css_list_at(ast->children, i);
      if (!child) continue;

      char* name = css_get_decl_name(child);
      if (name && map_get_value(lookup, name) && !child->is_important) continue;

      if (copy) {
       child = css_copy(child);
      }

      if (name) {
        map_set(lookup, name, child);
      }

      css_get_declarations_filtered(child, items, copy, lookup);
    }
  }
}

CSSNode *css_get_rule(CSSNode *css, const char *selector) {
  CSSNode *rule = 0;
  if (css->children != 0) {
    for (int i = 0; i < css->children->size; i++) {
      CSSNode *child = css_list_at(css->children, i);
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

CSSNode *css_get_rule_nth(CSSNode *css, const char *selector, uint32_t n) {
  CSSNode *rule = 0;
  if (css->children != 0) {
    for (int i = 0; i < css->children->size; i++) {
      if (i < n) continue;

      CSSNode *child = css_list_at(css->children, i);
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


static CSSNode *_css_get_value(CSSNode *ast, const char *key) {
  map_bucket_T *bucket = map_get(ast->keyvalue, (char*)key);
  if (bucket != 0) {
    return bucket->value;//css_transform_value(ast, bucket->value, key);
  }
  CSSNode *value_ast = 0;
  List *declarations = init_css_list(sizeof(CSSNode *));
  css_get_declarations(ast, declarations, 0);

  for (int i = 0; i < declarations->size; i++) {
    CSSNode *decl = css_list_at(declarations, i);
    if (!decl->left)
      continue;
    CSSNode *left = decl->left;
    if (!left->value_str)
      continue;

    if (strcmp(left->value_str, key) == 0) {
      value_ast = decl->right;
    }
  }
  css_list_free(declarations);
  return value_ast;//css_transform_value(ast, value_ast, key);
}


CSSNode* css_transform_value(CSSNode* parent, CSSNode* value, const char* key) {
  if (value) return value;
  if (!key) return 0;
  if (!parent) return 0;


  if (strstr(key, "padding") != 0) {

    CSSNode* padding = _css_get_value(parent, "padding");
    if (!padding) return 0;
    if (!padding->children) return 0;
    List* children = padding->children;
    if (children->size <= 0) return 0;

    if (strcmp(key, "padding-top") == 0) {
      return padding;
    }

    if (strcmp(key, "padding-right") == 0  && children->size >= 1) {
      return (CSSNode*)children->items[0];
    }

    if (strcmp(key, "padding-bottom") == 0  && children->size >= 2) {

      return (CSSNode*)children->items[1];
    }

    if (strcmp(key, "padding-left") == 0  && children->size >= 3) {

      return (CSSNode*)children->items[2];
    }
  }

  return value;
}

CSSNode *css_get_value(CSSNode *ast, const char *key) {
  map_bucket_T *bucket = map_get(ast->keyvalue, (char*)key);
  if (bucket != 0) {
    return css_transform_value(ast, bucket->value, key);
  }
  CSSNode *value_ast = 0;
  List *declarations = init_css_list(sizeof(CSSNode *));
  css_get_declarations(ast, declarations, 0);

  for (int i = 0; i < declarations->size; i++) {
    CSSNode *decl = css_list_at(declarations, i);
    if (!decl->left)
      continue;
    CSSNode *left = decl->left;
    if (!left->value_str)
      continue;

    if (strcmp(left->value_str, key) == 0) {
      value_ast = decl->right;
    }
  }
  css_list_free(declarations);
  return css_transform_value(ast, value_ast, key);
}

CSSNode* css_get_value_call(CSSNode* ast, const char* key, const char* call_name) {
  CSSNode* value = css_get_value(ast, key);
  if (!value) return value;

  if (value->type == CSS_AST_CALL) return value;

  if (!value->keyvalue) return 0;


  CSSNode* call_node = (CSSNode*)map_get_value(value->keyvalue, (char*)call_name);

  return call_node;
}

void css_set_value_string(CSSNode *ast, const char *key, char *value) {
  if (ast == 0 || key == 0)
    return;

  CSSNode *val = css_get_value(ast, key);
  if (!val) {
    if (ast->children == 0)
      ast->children = init_css_list(sizeof(CSSNode *));

    val = init_css_ast(CSS_AST_DECL);
    CSSNode *left = init_css_ast(CSS_AST_ID);
    left->value_str = strdup(key);
    val->left = left;
    CSSNode *right = init_css_ast(CSS_AST_STR);
    right->value_str = value != 0 ? strdup(value) : 0;
    val->right = right;

    css_list_append(ast->children, val);
    map_set(ast->keyvalue, (char*)key, val);
  } else {
    if (val->value_str)
      free(val->value_str);
    val->value_str = strdup(value);
  }
}
void css_set_value_float(CSSNode *ast, const char *key, float value) {
  if (ast == 0 || key == 0)
    return;

  CSSNode *val = css_get_value(ast, key);
  if (!val) {
    if (ast->children == 0)
      ast->children = init_css_list(sizeof(CSSNode *));

    val = init_css_ast(CSS_AST_DECL);
    CSSNode *left = init_css_ast(CSS_AST_ID);
    left->value_str = strdup(key);
    val->left = left;
    CSSNode *right = init_css_ast(CSS_AST_STR);
    right->value_float = value;
    right->value_double = (double)value;
    right->value_float = (float)value;
    right->value_int = (int)value;
    val->right = right;

    css_list_append(ast->children, val);
    map_set(ast->keyvalue, (char*)key, val);
  } else {
    val->value_float = value;
    val->value_double = (double)value;
    val->value_int = (int)value;
  }
}

void css_set_value_int(CSSNode *ast, const char *key, float value) {
  if (ast == 0 || key == 0)
    return;

  CSSNode *val = css_get_value(ast, key);
  if (!val) {
    if (ast->children == 0)
      ast->children = init_css_list(sizeof(CSSNode *));

    val = init_css_ast(CSS_AST_BINOP);
    CSSNode *left = init_css_ast(CSS_AST_ID);
    left->value_str = strdup(key);
    val->left = left;
    CSSNode *right = init_css_ast(CSS_AST_INT);
    right->value_double = (double)value;
    right->value_float = (float)value;
    right->value_int = (int)value;
    val->right = right;

    css_list_append(ast->children, val);
    map_set(ast->keyvalue, (char*)key, val);
  } else {
    val->value_float = (float)value;
    val->value_double = (double)value;
    val->value_int = (int)value;
  }
}

char *css_get_value_string(CSSNode *ast, const char *key) {
  CSSNode *val = css_get_value(ast, key);
  if (!val)
    return 0;
  return val->value_str ? strdup(val->value_str) : 0;
}
int css_get_value_int(CSSNode *ast, const char *key) {

  CSSNode *val = css_get_value(ast, key);
  if (!val)
    return 0;
  return val->value_int ? val->value_int : css_get_value_float(ast, key);
}
float css_get_value_float(CSSNode *ast, const char *key) {

  CSSNode *val = css_get_value(ast, key);
  if (!val)
    return 0;

  return val->value_float ? val->value_float
                          : val->value_double ? (float)val->value_double
                                              : (float)val->value_int;
}

void css_free(CSSNode *css) {
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

unsigned int css_value_is_set(CSSNode* ast, const char* key) {
  return css_get_value(ast, key) != 0;
}

static List *css_copy_css_list(List *incss_list) {
  if (!incss_list || (incss_list && incss_list->size == 0))
    return 0;
  List *newcss_list = init_css_list(incss_list->item_size);

  for (uint32_t i = 0; i < incss_list->size; i++) {
    CSSNode *child = incss_list->items[i];
    if (child == 0)
      continue;

    css_list_append(newcss_list, css_copy(child));
  }

  return newcss_list;
}

CSSNode *css_copy(CSSNode *css) {
  if (css == 0)
    return 0;
  CSSNode *ast = init_css_ast(css->type);
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

void css_reindex(CSSNode *css) {
  List *declarations = init_css_list(sizeof(CSSNode *));
  css_get_declarations(css, declarations, 0);

  for (int i = 0; i < declarations->size; i++) {
    CSSNode *decl = css_list_at(declarations, i);
    if (!decl->left)
      continue;
    CSSNode *left = decl->left;
    if (!left->value_str)
      continue;

    map_set(css->keyvalue, left->value_str, decl->right);
  }

  css_list_free(declarations);
}

const char *css_crayola_to_hex(const char *name) {
  for (uint32_t i = 0; i < (uint32_t)CRAYOLA_LENGTH; i += 2) {
    const char *k = CRAYOLA[(int)MIN(CRAYOLA_LENGTH - 1, i)];
    const char *v = CRAYOLA[(int)MIN(CRAYOLA_LENGTH - 1, i + 1)];
    if (strcmp(k, name) == 0) {
      return v;
    }
  }

  return 0;
}

ECSSDisplay css_to_display(const char *value) {
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

ECSSFlexDirection css_to_flex_direction(const char *value) {
  if (value == 0)
    return CSS_FLEX_DIRECTION_ROW;
  if (strcmp(value, "row") == 0)
    return CSS_FLEX_DIRECTION_ROW;
  if (strcmp(value, "column") == 0)
    return CSS_FLEX_DIRECTION_COLUMN;
  return CSS_FLEX_DIRECTION_ROW;
}

ECSSFlexAlign css_to_flex_align(const char *value) {
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

ECSSTextAlign css_to_text_align(const char *value) {
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

ECSSPosition css_to_position(const char *value) {
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

ECSSDisplay css_get_value_display(CSSNode* ast, const char* key) {
  char* str = css_get_value_string(ast, (char *)key);
  return css_to_display(str);
}

ECSSFlexAlign css_get_value_flex_align(CSSNode* ast, const char* key) {
  char* str = css_get_value_string(ast, (char *)key);
  return css_to_flex_align(str);
}

ECSSFlexDirection css_get_value_flex_direction(CSSNode* ast, const char* key) {
  char* str = css_get_value_string(ast, (char *)key);
  return css_to_flex_direction(str);
}

ECSSPosition css_get_value_position(CSSNode* ast, const char* key){
  char* str = css_get_value_string(ast, (char *)key);
  return css_to_position(str);
}

static const char *ensure_hex(const char *value) {
  const char *v = (const char *)value;
  if (v == 0)
    return 0;
  if (strchr(v, '#') == 0)
    v = css_crayola_to_hex(value);
  return v;
}

CSSColor css_hex_to_color(const char *value) {
  const char *v = ensure_hex(value);
  if (v == 0)
    return (CSSColor){-1, -1, -1, -1};
  if (v[0] == '#')
    v++;
  unsigned int hexvalue = strtol(v, 0, 16);
  return (CSSColor){((hexvalue >> 16) & 0xFF), ((hexvalue >> 8) & 0xFF),
                    ((hexvalue)&0xFF), 1};
}

CSSColor css_value_to_color(CSSNode *ast, const char *key) {
  CSSNode *val = css_get_value(ast, (char *)key);

  if (val && val->args && val->args->size >= 3) {
    CSSNode *_r = (CSSNode *)css_list_at(val->args, 0);
    CSSNode *_g = (CSSNode *)css_list_at(val->args, 1);
    CSSNode *_b = (CSSNode *)css_list_at(val->args, 2);

    float r = css_ast_get_float(_r);
    float g = css_ast_get_float(_g);
    float b = css_ast_get_float(_b);
    float a = 1.0f;

    if (val->args->size >= 4) {
      CSSNode *_a = (CSSNode *)css_list_at(val->args, 3);

      a = css_ast_get_float(_a);
    }

    return (CSSColor){r, g, b, a};
  } else if (val && val->value_str) {
    return css_hex_to_color(val->value_str);
  }
  return (CSSColor){-1, -1, -1, -1};
}

unsigned int css_is_decl(CSSNode* ast) {
  if (!ast) return 0;
  return ast->type == CSS_AST_DECL && ast->left != 0 && ast->right != 0 && ast->left->value_str != 0;
}

void css_unset_value(CSSNode* ast, const char* key) {
  if (!key) return;
  if (!ast->children && !ast->keyvalue) return;
  int size = (int)(ast->children != 0 ? ast->children->size : 0);

  for (int i = 0; i < size; i++) {
    CSSNode* child = (CSSNode*)ast->children->items[i];
    if (!css_is_decl(child)) continue;
    if (!child->left) continue;

    unsigned int left_matches = child->left->value_str == 0 ? 0 : strcmp(child->left->value_str, key) == 0;
    unsigned int self_matches = child->value_str != 0 && strcmp(child->value_str, key) == 0;
    if (left_matches || self_matches) {
      css_list_remove(ast->children, child, (void(*)(void*))css_free);
    }
  }

  if (ast->keyvalue) {
    map_unset(ast->keyvalue, (char*)key);
  }
}

void css_add_decl(CSSNode* ast, CSSNode* decl) {
  assert(ast != 0);
  if (!css_is_decl(decl)) return;
  assert(ast->keyvalue != 0);


  CSSNode* existing = (CSSNode*)css_get_value(ast, decl->left->value_str);
  if (existing && existing->is_important) {
    return;
  } else if (existing != 0) {
    css_unset_value(ast, decl->left->value_str);
  }

  CSSNode* newdecl = css_copy(decl);

  css_list_append(ast->children, newdecl);
  map_set(ast->keyvalue, newdecl->left->value_str, newdecl->right);
}

CSSNode* css_merge_rules(CSSNode* a, CSSNode* b) {
  for (int i = 0; i < (int)b->children->size; i++) {
    CSSNode* child = (CSSNode*)b->children->items[i];
    if (a->type == CSS_AST_RULE) {
      css_add_decl(a, child);
    } else {
      css_list_append(a->children, child);
    }
  }

  return a;
}

CSSNode* css_merge(CSSNode* a, CSSNode* b) {
  if (!b->children) return a;


  CSSIterator it_a = css_get_rules(a, 0);
  CSSIterator it_b = css_get_rules(b, 0);


  CSSNode* rule_a = 0;
  CSSNode* rule_b = 0;

  if ((it_a.length <= 0 || it_b.length <= 0) && b->children && b->children->size > 0) {
    for (uint32_t i = 0; i < b->children->size; i++) {
      CSSNode* child = (CSSNode*)css_list_at(b->children, i);
      if (!child)  continue;

      CSSNode* child_copy = css_copy(child);
      css_list_append(a->children, child_copy);
    }
  }

  while ((rule_a = css_iterator_next(&it_a)) != 0) {
    while ((rule_b = css_iterator_next(&it_b)) != 0) {
      css_merge_rules(rule_a, rule_b);
    }
  }



  return a;
}

CSSNode* css_unwrap(CSSNode* css) {
  CSSNode* new_node = init_css_ast(CSS_AST_COMPOUND);
  new_node->children = init_css_list(sizeof(CSSNode*));

  map_T* lookup = NEW_MAP();

  css_get_declarations_filtered(css, new_node->children, 1, lookup);

  map_free(lookup);
  lookup = 0;

  css_reindex(css);

  return new_node;
}

CSSColor css_get_value_color(CSSNode *ast, const char *key) {
  return css_value_to_color(ast, key);
}

ECSSTextAlign css_get_value_align(CSSNode* ast, const char* key) {
  char* str = css_get_value_string(ast, (char*)key);
  return css_to_text_align(str);
}


static void _css_query(CSSNode* cssnode, const char* selector, List* list) {
  char* str = ast_to_string(cssnode);

  if (str) {
    if (strcmp(str, selector) == 0) css_list_append(list, cssnode);
  }

  if (cssnode->children) {
    for (int i = 0; i < (int)cssnode->children->size; i++) {
      CSSNode* child = (CSSNode*)cssnode->children->items[i];
      _css_query(child, selector, list);
    }
  }
}

List* css_query(CSSNode* cssnode, const char* selector) {
  List* list = init_css_list(sizeof(CSSNode*));

  _css_query(cssnode, selector, list);

  return list;
}


static float percentage(float x, float y) {
  return (x / 100.0f) * y;
}

float css_get_value_float_computed(CSSNode* ast, const char* key, CSSContext context) {
  CSSNode* value = css_get_value(ast, (char*)key);
  if (value == 0) return 0;

  float x = css_get_value_float(ast, (char*)key);

  float z = x;

  unsigned int is_vertical = strcmp(key, "height") == 0;

  switch (value->unit) {
    case CSS_UNIT_PERCENT: z = percentage(x, is_vertical ? context.height : context.width); break;
    case CSS_UNIT_REM: z = x * context.rem; break;
    case CSS_UNIT_EM: z = x * context.em; break;
    case CSS_UNIT_PX: z = x; break;
    case CSS_UNIT_VH: z = percentage(x, context.vh); break;
    case CSS_UNIT_VW: z = percentage(x, context.vh); break;
    default: { z = x; } break;
  }

  return z;
}


ECSSValueType css_get_value_type(CSSNode* ast, const char* key) {
  char* str = css_get_value_string(ast, (char*)key);
  if (str == 0) return CSS_UNSPECIFIED;

  if (strcmp(str, "auto") == 0) return CSS_AUTO;
  if (strcmp(str, "none") == 0) return CSS_NONE;

  return CSS_UNSPECIFIED;
}

ECSSUnit css_get_value_unit(CSSNode* ast, const char* key) {
  CSSNode* value = css_get_value(ast, (char*)key);
  if (value == 0) return CSS_UNIT_PX;

  return value->unit;
}
