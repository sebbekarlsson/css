#include <mem.h>
#include <parser.h>
#include <stdio.h>
#include <string.h>
#include <utils.h>

#define PARSER_DONE(parser) (parser->token->type == TOKEN_EOF)

#define PARSER_HAS_NEXT(parser)                                                \
  ((parser->token->type == TOKEN_SEMI ||                                       \
    parser->token->type == TOKEN_RBRACE) &&                                    \
   !PARSER_DONE(parser))

CSSParser *init_css_parser(CSSLexer *lexer) {
  CSSParser *parser = NEW(CSSParser);
  parser->lexer = lexer;
  parser->trash = init_list(sizeof(CSSToken *));
  parser->token = css_lexer_next_token(lexer);
  // list_append(parser->trash, parser->token);

  return parser;
}

void css_parser_eat(CSSParser *parser, int token_type) {
  if (parser->token->type != token_type) {
    printf("Parser: Unexpected token: `%s\n` (%d), was expecting (%d)",
           parser->token->value, parser->token->type, token_type);
    exit(1);
  } else {
    if (parser->token != 0) {
      css_token_free(parser->token);
    }
    parser->token = css_lexer_next_token(parser->lexer);
    // list_append(parser->trash, parser->token);
  }
}

char *css_parser_parse_unit(CSSParser *parser) {
  char *s = 0;
  if (parser->token->type == TOKEN_ID) {
    s = strdup(parser->token->value);
    css_parser_eat(parser, TOKEN_ID);
  }

  return s;
}

CSSAST *css_parser_parse_id(CSSParser *parser) {
  char *value = SSTRDUP(parser->token->value);
  CSSAST *ast = init_css_ast(CSS_AST_ID);
  css_parser_eat(parser, TOKEN_ID);
  ast->value_str = value;

  return ast;
}

CSSAST *css_parser_parse_string(CSSParser *parser) {
  char *value = SSTRDUP(parser->token->value);
  CSSAST *ast = init_css_ast(CSS_AST_STR);
  ast->value_str = value;
  css_parser_eat(parser, TOKEN_STR);

  return ast;
}
CSSAST *css_parser_parse_int(CSSParser *parser) {
  int value = SSTRINT(parser->token->value);
  CSSAST *ast = init_css_ast(CSS_AST_INT);
  ast->value_int = value;
  css_parser_eat(parser, TOKEN_INT);

  ast->unit = css_parser_parse_unit(parser);

  return ast;
}
CSSAST *css_parser_parse_float(CSSParser *parser) {
  float value = SSTRFLOAT(parser->token->value);
  CSSAST *ast = init_css_ast(CSS_AST_FLOAT);
  ast->value_float = value;
  css_parser_eat(parser, TOKEN_FLOAT);

  ast->unit = css_parser_parse_unit(parser);

  return ast;
}
CSSAST *css_parser_parse_double(CSSParser *parser) {
  float value = SSTRFLOAT(parser->token->value);
  CSSAST *ast = init_css_ast(CSS_AST_DOUBLE);
  css_parser_eat(parser, TOKEN_DOUBLE);
  ast->value_float = value;

  ast->unit = css_parser_parse_unit(parser);

  return ast;
}

CSSAST *css_parser_parse_factor(CSSParser *parser) {
  switch (parser->token->type) {
  case TOKEN_STR:
    return css_parser_parse_string(parser);
  case TOKEN_INT:
    return css_parser_parse_int(parser);
  case TOKEN_FLOAT:
    return css_parser_parse_float(parser);
  case TOKEN_DOUBLE:
    return css_parser_parse_double(parser);
  case TOKEN_ID:
    return css_parser_parse_id(parser);
  default: {
    printf("Parser.factor: Unexpected token `%s` (%d)", parser->token->value,
           parser->token->type);
    exit(1);
  }
  }

  return 0;
}

CSSAST *css_parser_parse_selector(CSSParser *parser) {
  CSSAST *left = css_parser_parse_factor(parser);

  while (parser->token->type == TOKEN_PLUS ||
         parser->token->type == TOKEN_AND ||
         parser->token->type == TOKEN_TILDE) {
    CSSAST *binop = init_css_ast(CSS_AST_BINOP);
    binop->token = css_token_clone(parser->token);
    css_parser_eat(parser, parser->token->type);
    binop->left = left;
    binop->right = css_parser_parse_selector(parser);
    left = binop;
  }

  return left;
}
CSSAST *css_parser_parse_decl(CSSParser *parser) {

  CSSAST *ast = init_css_ast(CSS_AST_DECL);
  ast->left = css_parser_parse_id(parser);
  css_parser_eat(parser, TOKEN_COLON);
  ast->right = css_parser_parse_factor(parser);

  return ast;
}

CSSAST *css_parser_parse_rule(CSSParser *parser) {

  CSSAST *ast = init_css_ast(CSS_AST_RULE);
  ast->children = init_list(sizeof(CSSAST *));
  ast->rule_selectors = init_list(sizeof(CSSAST *));

  list_append(ast->rule_selectors, css_parser_parse_id(parser));

  while (parser->token->type == TOKEN_COMMA) {
    css_parser_eat(parser, TOKEN_COMMA);
    list_append(ast->rule_selectors, css_parser_parse_selector(parser));
  }

  css_parser_eat(parser, TOKEN_LBRACE);

  if (parser->token->type != TOKEN_RBRACE) {
    list_append(ast->children, css_parser_parse_decl(parser));
  }

  while (parser->token->type == TOKEN_SEMI) {
    css_parser_eat(parser, TOKEN_SEMI);

    if (parser->token->type != TOKEN_RBRACE) {
      list_append(ast->children, css_parser_parse_decl(parser));
    }
  }

  css_parser_eat(parser, TOKEN_RBRACE);

  return ast;
}

CSSAST *css_parser_parse_term(CSSParser *parser) {
  CSSAST *left = css_parser_parse_factor(parser);

  return left;
}

CSSAST *css_parser_parse_expr(CSSParser *parser) {
  if (parser->token->type == TOKEN_ID) {
    return css_parser_parse_rule(parser);
  }

  CSSAST *left = css_parser_parse_term(parser);

  return left;
}

CSSAST *css_parser_parse_compound(CSSParser *parser) {
  CSSAST *ast = init_css_ast(CSS_AST_COMPOUND);
  ast->children = init_list(sizeof(CSSAST *));

  if (!PARSER_DONE(parser)) {
    list_append(ast->children, css_parser_parse_expr(parser));
  }

  while (PARSER_HAS_NEXT(parser)) {
    list_append(ast->children, css_parser_parse_expr(parser));
  }

  return ast;
}

CSSAST *css_parser_parse(CSSParser *parser) {
  return css_parser_parse_compound(parser);
}

void css_parser_free(CSSParser *parser) {
  if (parser->token)
    css_token_free(parser->token);
  if (parser->trash) {
    for (int i = 0; i < parser->trash->size; i++) {
      css_token_free(list_at(parser->trash, i));
    }

    list_free(parser->trash);
  }
  free(parser);
}
