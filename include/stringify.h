#ifndef XCSS_STRINGIFY_H
#define XCSS_STRINGIFY_H
#include <AST.h>



char* css_stringify(CSSAST* node);
char* css_stringify_int(CSSAST* node);
char* css_stringify_str(CSSAST* node);
char* css_stringify_float(CSSAST* node);
char* css_stringify_double(CSSAST* node);
char* css_stringify_id(CSSAST* node);
char* css_stringify_classname(CSSAST* node);
char* css_stringify_pseudo_class(CSSAST* node);
char* css_stringify_rule(CSSAST* node);
char* css_stringify_selector(CSSAST* node);
char* css_stringify_declaration(CSSAST* node);
char* css_stringify_binop(CSSAST* node);
char* css_stringify_unop(CSSAST* node);
char* css_stringify_statement(CSSAST* node);
char* css_stringify_call(CSSAST* node);
char* css_stringify_opt(CSSAST* node);
char* css_stringify_array(CSSAST* node);
char* css_stringify_compound(CSSAST* node);
#endif
