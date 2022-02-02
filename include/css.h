#ifndef XCSS_L_H
#define XCSS_L_H
#ifdef __cplusplus
extern "C" {
#endif

#include <AST.h>
#include <lexer.h>
#include <parser.h>
#include <css_context.h>

typedef CSSAST CSSNode;

typedef enum {
  CSS_DISPLAY_AUTO,
  CSS_DISPLAY_BLOCK,
  CSS_DISPLAY_INLINE_BLOCK,
  CSS_DISPLAY_INLINE,
  CSS_DISPLAY_INLINE_FLEX,
  CSS_DISPLAY_TABLE_CELL,
  CSS_DISPLAY_FLEX,
  CSS_DISPLAY_NONE,
  CSS_DISPLAY_GRID
} ECSSDisplay;

typedef enum {
  CSS_FLEX_DIRECTION_ROW,
  CSS_FLEX_DIRECTION_COLUMN,
} ECSSFlexDirection;

typedef enum {
  CSS_FLEX_ALIGN_BEGIN,
  CSS_FLEX_ALIGN_END,
  CSS_FLEX_ALIGN_CENTER,
} ECSSFlexAlign;

typedef enum {
  CSS_POSITION_AUTO,
  CSS_POSITION_ABSOLUTE,
  CSS_POSITION_RELATIVE
} ECSSPosition;

typedef enum {
CSS_TEXT_ALIGN_CENTER,
CSS_TEXT_ALIGN_LEFT,
CSS_TEXT_ALIGN_RIGHT
} ECSSTextAlign;


typedef enum {
CSS_UNSPECIFIED,
CSS_AUTO,
CSS_NONE
} ECSSValueType;

typedef struct CSS_COLOR_STRUCT {
  float r;
  float g;
  float b;
  float a;
} CSSColor;

ECSSDisplay css_to_display(const char *value);
ECSSFlexDirection css_to_flex_direction(const char *value);
ECSSFlexAlign css_to_flex_align(const char *value);
ECSSTextAlign css_to_text_align(const char *value);
ECSSPosition css_to_position(const char *value);
CSSColor css_hex_to_color(const char *value);
CSSColor css_value_to_color(CSSNode *ast, const char *key);

CSSColor css_get_value_color(CSSNode *ast, const char *key);
ECSSTextAlign css_get_value_align(CSSNode* ast, const char* key);
ECSSDisplay css_get_value_display(CSSNode* ast, const char* key);
ECSSPosition css_get_value_position(CSSNode* ast, const char* key);
ECSSFlexAlign css_get_value_flex_align(CSSNode* ast, const char* key);
ECSSFlexDirection css_get_value_flex_direction(CSSNode* ast, const char* key);
ECSSValueType css_get_value_type(CSSNode* ast, const char* key);

ECSSUnit css_get_value_unit(CSSNode* ast, const char* key);

CSSNode *css(char *value);
CSSNode *css_anon(char *value);

void css_get_rules(CSSNode *ast, List *items);
void css_get_declarations(CSSNode *ast, List *items);

CSSNode *css_get_value(CSSNode *ast, const char *key);

CSSNode* css_get_value_call(CSSNode* ast, const char* key, const char* call_name);

unsigned int css_value_is_set(CSSNode* ast, const char* key);


char *css_get_value_string(CSSNode *ast, const char *key);

void css_set_value_string(CSSNode *ast, const char *key, char *value);
void css_set_value_int(CSSNode *ast, const char *key, float value);
  void css_set_value_float(CSSNode *ast, const char *key, float value);

CSSNode* css_merge(CSSNode* a, CSSNode* b);

const char *css_crayola_to_hex(const char *name);

int css_get_value_int(CSSNode *ast, const char *key);
float css_get_value_float(CSSNode *ast, const char *key);

float css_get_value_float_computed(CSSNode* ast, const char* key, CSSContext context);

void css_free(CSSNode *css);

CSSNode *css_get_rule(CSSNode *css, const char *selector);

List* css_query(CSSNode* css, const char* selector);

CSSNode *css_copy(CSSNode *css);

void css_reindex(CSSNode *css);

#ifdef __cplusplus
}
#endif
#endif
