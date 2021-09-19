#include <mem.h>
#include <parser.h>
#include <stdio.h>
#include <string.h>
#include <utils.h>

#define PARSER_DONE(parser) (parser->token->type == TOKEN_EOF)

#define PARSER_HAS_NEXT(parser) (!PARSER_DONE(parser))

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
    printf("Parser: Unexpected token: `%s` (%d), was expecting (%d)\n",
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
  if (parser->token->type == TOKEN_ID ||
      parser->token->type == TOKEN_PERCENTAGE) {
    s = strdup(parser->token->value);
    css_parser_eat(parser, parser->token->type);
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

  CSSAST *left = ast;
  while (left &&
         (parser->token->type == TOKEN_ID || parser->token->type == TOKEN_STR ||
          parser->token->type == TOKEN_INT ||
          parser->token->type == TOKEN_FLOAT)) {
    if (!(left->children)) {
      left->children = init_list(sizeof(CSSAST *));
    }

    list_append(left->children, css_parser_parse_factor(parser));
  }

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

CSSAST *css_parser_parse_array(CSSParser *parser) {
  CSSAST *ast = init_css_ast(CSS_AST_ARRAY);
  ast->children = init_list(sizeof(CSSAST *));
  css_parser_eat(parser, TOKEN_LBRACKET);

  list_append(ast->children, css_parser_parse_factor(parser));
  css_parser_eat(parser, TOKEN_RBRACKET);

  return ast;
}

CSSAST *css_parser_parse_factor(CSSParser *parser) {
  CSSAST *left = 0;
  if (parser->token->type == TOKEN_LBRACE) {
    CSSAST *rule = init_css_ast(CSS_AST_RULE);
    css_parser_parse_rule_body(parser, rule);

    return rule;
  }
  if (parser->token->type == TOKEN_LBRACKET) {
    left = css_parser_parse_array(parser);
    return left;
  }

  while (parser->token->type == TOKEN_COLON ||
         parser->token->type == TOKEN_COLON_COLON ||
         parser->token->type == TOKEN_MINUS ||
         parser->token->type == TOKEN_PLUS) {
    left = init_css_ast(CSS_AST_UNOP);
    left->token = css_token_clone(parser->token);
    css_parser_eat(parser, parser->token->type);
    left->right = css_parser_parse_term(parser);

    return left;
  }

  switch (parser->token->type) {
  case TOKEN_STR:
    left = css_parser_parse_string(parser);
    break;
  case TOKEN_INT:
    left = css_parser_parse_int(parser);
    break;
  case TOKEN_FLOAT:
    left = css_parser_parse_float(parser);
    break;
  case TOKEN_DOUBLE:
    left = css_parser_parse_double(parser);
    break;
  case TOKEN_ID:
    left = css_parser_parse_id(parser);
    break;
  default: {
    left = 0;
  } break;
  }

  if (left && parser->token->type == TOKEN_LPAREN) {
    CSSAST *acall = init_css_ast(CSS_AST_CALL);
    acall->left = left;
    acall->args = init_list(sizeof(CSSAST *));
    css_parser_eat(parser, TOKEN_LPAREN);

    if (parser->token->type != TOKEN_RPAREN) {
      list_append(acall->args, css_parser_parse_term(parser));

      while (parser->token->type == TOKEN_COMMA) {
        css_parser_eat(parser, TOKEN_COMMA);
        list_append(acall->args, css_parser_parse_term(parser));
      }
    }
    css_parser_eat(parser, TOKEN_RPAREN);

    left = acall;
  }

  while (left && parser->token->type == TOKEN_LBRACKET) {
    CSSAST *acall = init_css_ast(CSS_AST_CALL);
    acall->left = left;
    acall->args = init_list(sizeof(CSSAST *));
    css_parser_eat(parser, TOKEN_LBRACKET);

    if (parser->token->type != TOKEN_RBRACKET) {
      list_append(acall->args, css_parser_parse_term(parser));

      while (parser->token->type != TOKEN_RBRACKET &&
             parser->token->type != TOKEN_RBRACE) {
        list_append(acall->args, css_parser_parse_term(parser));
      }
    }
    css_parser_eat(parser, TOKEN_RBRACKET);

    left = acall;
  }

  return left;
}

CSSAST *css_parser_parse_selector(CSSParser *parser) {
  CSSAST *left = css_parser_parse_term(parser);

  return left;
}
CSSAST *css_parser_parse_decl(CSSParser *parser) {

  CSSAST *ast = init_css_ast(CSS_AST_DECL);
  ast->left = css_parser_parse_id(parser);

  if (parser->token->type != TOKEN_LBRACE)
    css_parser_eat(parser, TOKEN_COLON);
  ast->right = css_parser_parse_term(parser);

  //while (parser->token->type == TOKEN_SEMI)
  //  css_parser_eat(parser, parser->token->type);

  if ((parser->token->type == TOKEN_ID || parser->token->type == TOKEN_FLOAT ||
       parser->token->type == TOKEN_STR || parser->token->type == TOKEN_INT) &&
      ast->right != 0) {
    CSSAST *right = ast->right;
    right->children = init_list(sizeof(CSSAST *));

    while (
        parser->token->type == TOKEN_ID || parser->token->type == TOKEN_FLOAT ||
        parser->token->type == TOKEN_STR || parser->token->type == TOKEN_INT) {
      list_append(right->children, css_parser_parse_term(parser));
    }
  }

  return ast;
}

void css_parser_parse_rule_body(CSSParser *parser, CSSAST *ast) {
  if (!ast->children) {
    ast->children = init_list(sizeof(CSSAST *));
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
}

CSSAST *css_parser_parse_rule(CSSParser *parser) {

  CSSAST *ast = init_css_ast(CSS_AST_RULE);
  ast->rule_selectors = init_list(sizeof(CSSAST *));

  list_append(ast->rule_selectors, css_parser_parse_selector(parser));

  while (parser->token->type == TOKEN_COMMA ||
         parser->token->type == TOKEN_ID) {
    if (parser->token->type == TOKEN_COMMA)
      css_parser_eat(parser, TOKEN_COMMA);
    list_append(ast->rule_selectors, css_parser_parse_selector(parser));
  }

  css_parser_parse_rule_body(parser, ast);

  return ast;
}

CSSAST *css_parser_parse_term(CSSParser *parser) {
  if (parser->token->type == TOKEN_LBRACE) {
    CSSAST *rule = init_css_ast(CSS_AST_RULE);
    css_parser_parse_rule_body(parser, rule);

    return rule;
  }

  if (parser->token->type == TOKEN_STATEMENT)
    return css_parser_parse_statement(parser);

  CSSAST *left = css_parser_parse_factor(parser);

  while (left && parser->token->type == TOKEN_EQUALS) {
    CSSAST *binop = init_css_ast(CSS_AST_BINOP);
    binop->token = css_token_clone(parser->token);
    css_parser_eat(parser, parser->token->type);
    binop->left = left;
    binop->right = css_parser_parse_term(parser);
    left = binop;
  }

  while (
      left &&
      (parser->token->type == TOKEN_PLUS || parser->token->type == TOKEN_AND ||
       parser->token->type == TOKEN_LT || parser->token->type == TOKEN_MINUS ||
       parser->token->type == TOKEN_PLUS ||
       parser->token->type == TOKEN_COLON_COLON ||
       parser->token->type == TOKEN_GT || parser->token->type == TOKEN_TILDE ||
       parser->token->type == TOKEN_COLON)) {
    CSSAST *binop = init_css_ast(CSS_AST_BINOP);
    binop->token = css_token_clone(parser->token);
    css_parser_eat(parser, parser->token->type);
    binop->left = left;
    binop->right = css_parser_parse_term(parser);
    left = binop;
  }

  return left;
}

CSSAST *css_parser_parse_statement(CSSParser *parser) {
  CSSAST *ast = init_css_ast(CSS_AST_STATEMENT);
  ast->value_str = parser->token->value ? strdup(parser->token->value) : 0;
  css_parser_eat(parser, TOKEN_STATEMENT);

  ast->right = css_parser_parse_term(parser);

  return ast;
}

CSSAST *css_parser_parse_expr(CSSParser *parser) {
  if (parser->token->type == TOKEN_ID ||
      parser->token->type == TOKEN_COLON_COLON ||
      parser->token->type == TOKEN_COLON) {
    return css_parser_parse_rule(parser);
  }

  if (parser->token->type == TOKEN_STATEMENT) {
    return css_parser_parse_statement(parser);
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
    while (parser->token->type == TOKEN_SEMI) {
      css_parser_eat(parser, TOKEN_SEMI);
    }
    list_append(ast->children, css_parser_parse_expr(parser));
  }

  return ast;
}

CSSAST *css_parser_parse(CSSParser *parser) {
  if (parser->token->type == TOKEN_LBRACE) {
    CSSAST *ast = init_css_ast(CSS_AST_RULE);
    css_parser_parse_rule_body(parser, ast);

    return ast;
  }
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
