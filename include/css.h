#ifndef XCSS_L_H
#define XCSS_L_H
#ifdef __cplusplus
extern "C" {
#endif

#include <AST.h>
#include <lexer.h>
#include <parser.h>

typedef enum {
  CSS_DISPLAY_AUTO,
  CSS_DISPLAY_BLOCK,
  CSS_DISPLAY_INLINE_BLOCK,
  CSS_DISPLAY_INLINE,
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

typedef struct CSS_COLOR_STRUCT {
  float r;
  float g;
  float b;
  float a;
} CSSColor;

ECSSDisplay css_to_display(char *value);
ECSSFlexDirection css_to_flex_direction(char *value);
ECSSFlexAlign css_to_flex_align(char *value);
ECSSTextAlign css_to_text_align(char *value);
ECSSPosition css_to_position(char *value);
CSSColor css_hex_to_color(char *value);
CSSColor css_value_to_color(CSSAST *ast, const char *key);

CSSColor css_get_value_color(CSSAST *ast, const char *key);
ECSSTextAlign css_get_value_align(CSSAST* ast, const char* key);
ECSSDisplay css_get_value_display(CSSAST* ast, const char* key);

CSSAST *css(char *value);
CSSAST *css_anon(char *value);

void css_get_rules(CSSAST *ast, List *items);
void css_get_declarations(CSSAST *ast, List *items);

CSSAST *css_get_value(CSSAST *ast, char *key);

unsigned int css_value_is_set(CSSAST* ast, char* key);


char *css_get_value_string(CSSAST *ast, char *key);

void css_set_value_string(CSSAST *ast, char *key, char *value);
void css_set_value_int(CSSAST *ast, char *key, float value);
void css_set_value_float(CSSAST *ast, char *key, float value);

const char *css_crayola_to_hex(char *name);

int css_get_value_int(CSSAST *ast, char *key);
float css_get_value_float(CSSAST *ast, char *key);

void css_free(CSSAST *css);

CSSAST *css_get_rule(CSSAST *css, char *selector);

CSSAST *css_copy(CSSAST *css);

void css_reindex(CSSAST *css);

#ifdef __cplusplus
}
#endif
#endif
