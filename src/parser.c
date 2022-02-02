#include <mem.h>
#include <parser.h>
#include <stdio.h>
#include <string.h>
#include <utils.h>

#ifndef __FUNCTION_NAME__
#ifdef WIN32 // WINDOWS
#define __FUNCTION_NAME__ __FUNCTION__
#else //*NIX
#define __FUNCTION_NAME__ __func__
#endif
#endif


static unsigned int safe_strcmp(const char* a, const char* b) {
  if (!a || !b) return 1;
  return strcmp(a, b);
}

#define PRINT_LEXER_INFO()                      \
  {  char buff[256]; css_lexer_get_position_string(parser->lexer, buff); printf("%s %s\n", buff, parser->token->value); }

#define PARSER_DONE(parser) (parser->token->type == TOKEN_EOF)

#define PARSER_HAS_NEXT(parser) (!PARSER_DONE(parser))


static void noop() {}

#ifdef _DEBUG
#define _DBG(...) printf("%s\n", __FUNCTION_NAME__)
#else
#define _DBG(msg) noop()
#endif

CSSParser *init_css_parser(CSSLexer *lexer) {
  CSSParser *parser = NEW(CSSParser);
  parser->lexer = lexer;
  parser->trash = init_css_list(sizeof(CSSToken *));
  parser->token = css_lexer_next_token(lexer);
  // css_list_append(parser->trash, parser->token);

  return parser;
}

void css_parser_eat(CSSParser *parser, int token_type) {
  if (parser->token->type != token_type) {
    char buff[256];
    css_lexer_get_position_string(parser->lexer, buff);
    printf("(CSS) Parser (%s): Unexpected token: `%s` (%d), was expecting (%d)\n",
           buff, parser->token->value, parser->token->type, token_type);
    exit(1);
  } else {
    if (parser->token != 0) {
      css_token_free(parser->token);
    }
    parser->token = css_lexer_next_token(parser->lexer);
    // css_list_append(parser->trash, parser->token);
  }
}

ECSSUnit css_parser_parse_unit(CSSParser *parser) {
  _DBG();
  ECSSUnit unit = CSS_UNIT_PX;
  if (parser->token->type == TOKEN_ID ||
      parser->token->type == TOKEN_PERCENTAGE) {

    if (safe_strcmp(parser->token->value, "%") == 0) unit = CSS_UNIT_PERCENT;
    else if (safe_strcmp(parser->token->value, "px") == 0) unit = CSS_UNIT_PX;
    else if (safe_strcmp(parser->token->value, "rem") == 0) unit = CSS_UNIT_REM;
    else if (safe_strcmp(parser->token->value, "em") == 0) unit = CSS_UNIT_EM;
    else if (safe_strcmp(parser->token->value, "fr") == 0) unit = CSS_UNIT_FR;
    else if (safe_strcmp(parser->token->value, "vw") == 0) unit = CSS_UNIT_VW;
    else if (safe_strcmp(parser->token->value, "vh") == 0) unit = CSS_UNIT_VH;
    else if (safe_strcmp(parser->token->value, "pt") == 0) unit = CSS_UNIT_PT;
    else if (safe_strcmp(parser->token->value, "deg") == 0) unit = CSS_UNIT_DEG;
    css_parser_eat(parser, parser->token->type);
  }

  return unit;
}

CSSAST *css_parser_parse_id(CSSParser *parser) {
  _DBG();
  char *value = SSTRDUP(parser->token->value);
  CSSAST *ast = init_css_ast(parser->token->type == TOKEN_CLASSNAME ? CSS_AST_CLASSNAME : CSS_AST_ID);

  if (parser->token->type == TOKEN_CLASSNAME)
    css_parser_eat(parser, TOKEN_CLASSNAME);
  else
    css_parser_eat(parser, TOKEN_ID);

  ast->value_str = value;

  return ast;
}

CSSAST *css_parser_parse_string(CSSParser *parser) {
  _DBG();
  char *value = SSTRDUP(parser->token->value);
  CSSAST *ast = init_css_ast(CSS_AST_STR);
  ast->value_str = value;
  css_parser_eat(parser, parser->token->type == TOKEN_STR ? TOKEN_STR : TOKEN_HASH);

  CSSAST *left = ast;
  while (left &&
         (parser->token->type == TOKEN_ID || parser->token->type == TOKEN_STR ||
          parser->token->type == TOKEN_INT ||
          parser->token->type == TOKEN_FLOAT || parser->token->type == TOKEN_HASH)) {
    if (!(left->children)) {
      left->children = init_css_list(sizeof(CSSAST *));
    }

    css_list_append(left->children, css_parser_parse_factor(parser));
  }

  return ast;
}
CSSAST *css_parser_parse_int(CSSParser *parser) {
  _DBG();
  int value = SSTRINT(parser->token->value);
  CSSAST *ast = init_css_ast(CSS_AST_INT);
  ast->value_int = value;

  if (parser->token->type == TOKEN_ESCAPED_INT) {

    css_parser_eat(parser, TOKEN_ESCAPED_INT);
    ast->escaped = 1;
  } else {
    css_parser_eat(parser, TOKEN_INT);
  }

  ast->unit = css_parser_parse_unit(parser);

  return ast;
}
CSSAST *css_parser_parse_float(CSSParser *parser) {
  _DBG();
  float value = SSTRFLOAT(parser->token->value);
  CSSAST *ast = init_css_ast(CSS_AST_FLOAT);
  ast->value_float = value;
  css_parser_eat(parser, TOKEN_FLOAT);

  ast->unit = css_parser_parse_unit(parser);

  return ast;
}
CSSAST *css_parser_parse_double(CSSParser *parser) {
  _DBG();
  float value = SSTRFLOAT(parser->token->value);
  CSSAST *ast = init_css_ast(CSS_AST_DOUBLE);
  css_parser_eat(parser, TOKEN_DOUBLE);
  ast->value_float = value;

  ast->unit = css_parser_parse_unit(parser);

  return ast;
}

CSSAST *css_parser_parse_array(CSSParser *parser) {
  _DBG();
  CSSAST *ast = init_css_ast(CSS_AST_ARRAY);
  ast->children = init_css_list(sizeof(CSSAST *));
  css_parser_eat(parser, TOKEN_LBRACKET);

  css_list_append(ast->children, css_parser_parse_term(parser));
  css_parser_eat(parser, TOKEN_RBRACKET);

  return ast;
}

CSSAST *css_parser_parse_factor(CSSParser *parser) {
  _DBG();
  CSSAST *left = 0;

  if (parser->token->type == TOKEN_LPAREN) {
    css_parser_eat(parser, TOKEN_LPAREN);
    CSSAST* j = css_parser_parse_term(parser);
    css_parser_eat(parser, TOKEN_RPAREN);
    return j;
  }

  if (parser->token->type == TOKEN_URL) {
    CSSAST* acall = init_css_ast(CSS_AST_CALL);
    acall->value_str = strdup(parser->token->value);
    acall->args = init_css_list(sizeof(CSSAST *));
    css_parser_eat(parser, TOKEN_URL);
    css_parser_eat(parser, TOKEN_LPAREN);
    char* junk = 0;
    CSSAST* strast = 0;
    if (parser->token->type == TOKEN_STR && (parser->lexer->c == '"' || parser->lexer->c =='\'')) {
      strast = css_parser_parse_string(parser);
    } else {
      strast = init_css_ast(CSS_AST_STR);
      if (parser->token->type == TOKEN_JUNK || parser->token->type == TOKEN_STR) {
        str_append(&junk, parser->token->value);
        //css_parser_eat(parser, parser->token->type);
      }
      char* junk2 = css_lexer_parse_string_until(parser->lexer, ')');
      if (junk2) {

        str_append(&junk, parser->token->value);
        str_append(&junk, junk2);

        free(junk2);
        junk2 = 0;
      }
      strast->value_str = junk;
    }
    parser->token = css_lexer_next_token(parser->lexer);
    css_parser_eat(parser, TOKEN_RPAREN);

    css_list_append(acall->args, strast);

    return acall;
  }

  if (parser->token->type == TOKEN_COLON) {
    css_parser_eat(parser, TOKEN_COLON);
    CSSAST* pseudo = css_parser_parse_factor(parser);
    pseudo->type = CSS_AST_PSEUDO_CLASS;
    return pseudo;
  }

  if (parser->token->type == TOKEN_LBRACE) {
    CSSAST *rule = init_css_ast(CSS_AST_RULE);
    css_parser_eat(parser, TOKEN_LBRACE);
    css_parser_parse_rule_body(parser, rule);
    css_parser_eat(parser, TOKEN_RBRACE);

    return rule;
  }
  if (parser->token->type == TOKEN_LBRACKET) {
    left = css_parser_parse_array(parser);
    return left;
  }

  while (
         parser->token->type == TOKEN_COLON_COLON ||
         parser->token->type == TOKEN_MINUS ||
         parser->token->type == TOKEN_PLUS || parser->token->type == TOKEN_DIV) {
    left = init_css_ast(CSS_AST_UNOP);
    left->token = css_token_clone(parser->token);
    css_parser_eat(parser, parser->token->type);
    left->right = css_parser_parse_term(parser);

    return left;
  }

  switch (parser->token->type) {
    case TOKEN_STR:
    case TOKEN_HASH:
    left = css_parser_parse_string(parser);
    break;
    case TOKEN_INT:
    case TOKEN_ESCAPED_INT:
    left = css_parser_parse_int(parser);
    break;
  case TOKEN_FLOAT:
    left = css_parser_parse_float(parser);
    break;
  case TOKEN_DOUBLE:
    left = css_parser_parse_double(parser);
    break;
  case TOKEN_ID:
  case TOKEN_CLASSNAME:
    left = css_parser_parse_id(parser);
    break;
  default: { left = 0; } break;
  }

  if (left && parser->token->type == TOKEN_LPAREN) {
    CSSAST *acall = init_css_ast(CSS_AST_CALL);
    if (left->value_str) acall->value_str = strdup(left->value_str);

    acall->left = left;
    acall->args = init_css_list(sizeof(CSSAST *));
    css_parser_eat(parser, TOKEN_LPAREN);

    if (parser->token->type != TOKEN_RPAREN) {
      css_list_append(acall->args, css_parser_parse_term(parser));

      while (parser->token->type != TOKEN_RPAREN) {
        if (parser->token->type == TOKEN_COMMA)
          css_parser_eat(parser, TOKEN_COMMA);

        css_list_append(acall->args, css_parser_parse_term(parser));
      }
    }
    css_parser_eat(parser, TOKEN_RPAREN);

    left = acall;

  }

  while (left && parser->token->type == TOKEN_LBRACKET) {
    CSSAST *acall = init_css_ast(CSS_AST_ARRAY);
    acall->left = left;
    acall->children = init_css_list(sizeof(CSSAST *));
    css_parser_eat(parser, TOKEN_LBRACKET);

    if (parser->token->type != TOKEN_RBRACKET) {
      css_list_append(acall->children, css_parser_parse_term(parser));

      while (parser->token->type != TOKEN_RBRACKET &&
             parser->token->type != TOKEN_RBRACE) {
      PRINT_LEXER_INFO();
        css_list_append(acall->children, css_parser_parse_term(parser));
      }
    }
    css_parser_eat(parser, TOKEN_RBRACKET);

    left = acall;
  }

  if (!left && (parser->token->type == TOKEN_STAR || parser->token->type == TOKEN_TILDE)) {
    int type = parser->token->type;
    CSSAST* ast = init_css_ast(CSS_AST_ID);
    ast->value_str = strdup(parser->token->value);
    css_parser_eat(parser, type);
    return ast;
  }

  return left;
}

CSSAST *css_parser_parse_selector(CSSParser *parser) {
  _DBG();
  CSSAST *left = css_parser_parse_term(parser);

  return left;
}
CSSAST *css_parser_parse_decl(CSSParser *parser) {
  _DBG();
  CSSAST *ast = init_css_ast(CSS_AST_DECL);
    if (!ast->keyvalue) {
      ast->keyvalue = NEW_MAP();
    }

  ast->left = css_parser_parse_factor(parser);//css_parser_parse_id(parser);
                                              //
                                              //
  if (parser->token->type != TOKEN_LBRACE)
    css_parser_eat(parser, TOKEN_COLON);
  ast->right = css_parser_parse_term(parser);


    if (ast->right->value_str != 0) {
        map_set(ast->keyvalue, ast->right->value_str, ast->right);
      }

  if (parser->token->type == TOKEN_COMMA) {
    CSSAST *right = ast->right;

    if (right->children == 0)
      right->children = init_css_list(sizeof(CSSAST *));

    while (parser->token->type == TOKEN_COMMA) {
      css_parser_eat(parser, TOKEN_COMMA);
      css_list_append(right->children, css_parser_parse_term(parser));
    }
  }



  // while (parser->token->type == TOKEN_SEMI)
  //  css_parser_eat(parser, parser->token->type);

  if ((parser->token->type == TOKEN_ID || parser->token->type == TOKEN_FLOAT ||
       parser->token->type == TOKEN_STR || parser->token->type == TOKEN_URL || parser->token->type == TOKEN_INT || parser->token->type == TOKEN_ESCAPED_INT || parser->token->type == TOKEN_HASH || parser->token->type == TOKEN_COMMA) &&
      ast->right != 0) {
    CSSAST *right = ast->right;

    if (right->children == 0) {
      right->children = init_css_list(sizeof(CSSAST *));
    }

    while (
        parser->token->type == TOKEN_ID || parser->token->type == TOKEN_URL || parser->token->type == TOKEN_FLOAT ||
        parser->token->type == TOKEN_STR || parser->token->type == TOKEN_INT || parser->token->type == TOKEN_ESCAPED_INT || parser->token->type == TOKEN_HASH || parser->token->type == TOKEN_COMMA) {
      if (parser->token->type == TOKEN_COMMA) {
        css_parser_eat(parser, TOKEN_COMMA);
      }
      CSSAST* child = css_parser_parse_term(parser);
      css_list_append(right->children, child);

      if (child->value_str != 0) {
        map_set(ast->keyvalue, child->value_str, child);
      }

    }
  }

  if (parser->token->type == TOKEN_IMPORTANT) {
    css_parser_eat(parser, TOKEN_IMPORTANT);
    ast->is_important = 1;

    if (ast->left)
      ast->left->is_important = 1;

    if (ast->right)
      ast->right->is_important = 1;
  }

  if (parser->token->type == TOKEN_COMMA) {
    CSSAST *right = ast->right;

    if (right->children == 0)
      right->children = init_css_list(sizeof(CSSAST *));

    while (parser->token->type == TOKEN_COMMA) {
      css_parser_eat(parser, TOKEN_COMMA);
      css_list_append(right->children, css_parser_parse_term(parser));
    }
  }

  return ast;
}

void css_parser_parse_rule_body(CSSParser *parser, CSSAST *ast) {
  _DBG();
  if (!ast->children) {
    ast->children = init_css_list(sizeof(CSSAST *));
  }

  if (parser->token->type != TOKEN_RBRACE) {
    CSSAST *decl = css_parser_parse_decl(parser);

    if (decl && decl->left && decl->left->value_str && decl->right)
      map_set(ast->keyvalue, decl->left->value_str, decl->right);

    css_list_append(ast->children, decl);
  }

  while (parser->token->type == TOKEN_SEMI) {
    css_parser_eat(parser, TOKEN_SEMI);

    if (parser->token->type == TOKEN_EOF)
      break;

    if (parser->token->type != TOKEN_RBRACE) {
      CSSAST *decl = css_parser_parse_decl(parser);

      if (decl && decl->left && decl->left->value_str && decl->right)
        map_set(ast->keyvalue, decl->left->value_str, decl->right);

      css_list_append(ast->children, decl);
    }
  }
}

CSSAST *css_parser_parse_rule(CSSParser *parser) {
  _DBG();
  CSSAST *ast = init_css_ast(CSS_AST_RULE);

  unsigned int is_media = safe_strcmp(parser->token->value, "@media") == 0 || parser->token->special;


  if (parser->token->type != TOKEN_LBRACE) {
    ast->rule_selectors = init_css_list(sizeof(CSSAST *));




    CSSAST* selector = css_parser_parse_selector(parser);

    if (selector && selector->value_str != 0) {
      css_list_append(ast->rule_selectors, selector);
    }

    while (parser->token->type == TOKEN_COMMA || parser->token->type == TOKEN_HASH ||
           parser->token->type == TOKEN_ID || parser->token->type == TOKEN_GT || parser->token->type == TOKEN_COLON || parser->token->type == TOKEN_STAR || parser->token->type == TOKEN_TILDE || parser->token->type == TOKEN_INT || parser->token->type == TOKEN_LBRACKET) {
      if (parser->token->type == TOKEN_COMMA) {
        css_parser_eat(parser, TOKEN_COMMA);
      }
        selector = css_parser_parse_selector(parser);
        if (!selector) continue;
        if (!selector->value_str) continue;

        css_list_append(ast->rule_selectors, selector);
      }
  }

  if (parser->token->type == TOKEN_LBRACE) {
    css_parser_eat(parser, TOKEN_LBRACE);

    if (is_media) {
      ast->children = init_css_list(sizeof(CSSAST *));

      if (!PARSER_DONE(parser)) {
        css_list_append(ast->children, css_parser_parse_expr(parser));
      }

      while (PARSER_HAS_NEXT(parser) && parser->token->type != TOKEN_RBRACE) {
        while (parser->token->type == TOKEN_SEMI) {
          css_parser_eat(parser, TOKEN_SEMI);
        }
        css_list_append(ast->children, css_parser_parse_expr(parser));
      }
    } else {
      css_parser_parse_rule_body(parser, ast);
    }
    css_parser_eat(parser, TOKEN_RBRACE);
  }

  return ast;
}

CSSAST *css_parser_parse_rule_anon(CSSParser *parser) {
  _DBG();
  CSSAST *ast = init_css_ast(CSS_AST_RULE);
  css_parser_parse_rule_body(parser, ast);

  return ast;
}

CSSAST *css_parser_parse_term(CSSParser *parser) {
  _DBG();



  if (parser->token->type == TOKEN_LBRACE) {
    CSSAST *rule = init_css_ast(CSS_AST_RULE);
    css_parser_eat(parser, TOKEN_LBRACE);
    css_parser_parse_rule_body(parser, rule);
    css_parser_eat(parser, TOKEN_RBRACE);

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
       parser->token->type == TOKEN_PLUS || parser->token->type == TOKEN_DIV || parser->token->type == TOKEN_STAR || parser->token->type == TOKEN_TILDE ||
       parser->token->type == TOKEN_COLON_COLON || parser->token->type == TOKEN_SQUARED ||
       parser->token->type == TOKEN_COLON || parser->token->type == TOKEN_EQUALS ||
       parser->token->type == TOKEN_GT || parser->token->type == TOKEN_TILDE)) {
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
  _DBG();
  CSSAST *ast = init_css_ast(CSS_AST_STATEMENT);
  ast->value_str = parser->token->value ? strdup(parser->token->value) : 0;
  css_parser_eat(parser, TOKEN_STATEMENT);


  if (parser->token->type == TOKEN_LBRACE) {
    if (ast->value_str) {
      CSSAST* selector = init_css_ast(CSS_AST_ID);
      selector->value_str = strdup(ast->value_str);
      ast->rule_selectors = init_css_list(sizeof(CSSAST*));
      css_list_append(ast->rule_selectors, selector);
    }
    ast->type = CSS_AST_RULE;
    css_parser_eat(parser, TOKEN_LBRACE);
    css_parser_parse_rule_body(parser, ast);
    css_parser_eat(parser, TOKEN_RBRACE);
  } else {
    ast->right = css_parser_parse_term(parser);
  }

  return ast;
}

CSSAST *css_parser_parse_expr(CSSParser *parser) {
  _DBG();
  if (parser->token->type == TOKEN_ID || parser->token->type == TOKEN_CLASSNAME ||
      parser->token->type == TOKEN_COLON_COLON || parser->token->type == TOKEN_STAR || parser->token->type == TOKEN_TILDE ||
      parser->token->type == TOKEN_COLON || parser->token->type == TOKEN_HASH || parser->token->type == TOKEN_INT || parser->token->special || parser->token->type == TOKEN_LBRACKET) {
    return css_parser_parse_rule(parser);
  }

  if (parser->token->type == TOKEN_STATEMENT) {
    return css_parser_parse_statement(parser);
  }

  CSSAST *left = css_parser_parse_term(parser);

  return left;
}

CSSAST *css_parser_parse_compound(CSSParser *parser) {
  _DBG();
  CSSAST *ast = init_css_ast(CSS_AST_COMPOUND);
  ast->children = init_css_list(sizeof(CSSAST *));

  if (!PARSER_DONE(parser)) {
    css_list_append(ast->children, css_parser_parse_expr(parser));
  }

  while (PARSER_HAS_NEXT(parser)) {
    while (parser->token->type == TOKEN_SEMI) {
      css_parser_eat(parser, TOKEN_SEMI);
    }
    css_list_append(ast->children, css_parser_parse_expr(parser));
  }

  return ast;
}

CSSAST *css_parser_parse(CSSParser *parser) {
  _DBG();
  if (parser->token->type == TOKEN_LBRACE) {
    CSSAST *ast = init_css_ast(CSS_AST_RULE);
    css_parser_eat(parser, TOKEN_LBRACE);
    css_parser_parse_rule_body(parser, ast);
    css_parser_eat(parser, TOKEN_RBRACE);

    return ast;
  }
  return css_parser_parse_compound(parser);
}

void css_parser_free(CSSParser *parser) {
  _DBG();
  if (parser->token)
    css_token_free(parser->token);
  if (parser->trash) {
    for (int i = 0; i < parser->trash->size; i++) {
      css_token_free(css_list_at(parser->trash, i));
    }

    css_list_free(parser->trash);
  }
  free(parser);
}
