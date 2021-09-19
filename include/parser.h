#ifndef XCSS_PARSER_H
#define XCSS_PARSER_H
#include <AST.h>
#include <lexer.h>
#include <parser.h>
typedef struct {
  CSSLexer *lexer;
  CSSToken *token;
  List *trash;
  int parent;
} CSSParser;

CSSParser *init_css_parser(CSSLexer *lexer);

void css_parser_eat(CSSParser *parser, int token_type);

CSSAST *css_parser_parse(CSSParser *parser);

CSSAST *css_parser_parse_expr(CSSParser *parser);
CSSAST *css_parser_parse_term(CSSParser *parser);
CSSAST *css_parser_parse_statement(CSSParser *parser);
CSSAST *css_parser_parse_factor(CSSParser *parser);
void css_parser_parse_rule_body(CSSParser *parser, CSSAST *ast);

void css_parser_free(CSSParser *parser);
CSSAST *css_parser_parse_selector(CSSParser *parser);
#endif
