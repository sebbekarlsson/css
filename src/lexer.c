#include <ctype.h>
#include <lexer.h>
#include <mem.h>
#include <stdio.h>
#include <string.h>
#include <utils.h>

#define LEXER_TOK(lexer, t)                                                    \
  {                                                                            \
    char *charstr = char_to_string(lexer->c);                                  \
    CSSToken *tok = init_css_token(t, charstr);                                \
    free(charstr);                                                             \
    css_lexer_advance(lexer);                                                  \
    return tok;                                                                \
  }

CSSLexer *init_css_lexer(char *value) {
  CSSLexer *lexer = NEW(CSSLexer);
  lexer->value = strdup(value);
  lexer->i = 0;
  lexer->c = lexer->value[lexer->i];
  lexer->len = strlen(lexer->value);

  return lexer;
}

void css_lexer_advance(CSSLexer *lexer) {
  if (lexer->i < lexer->len) {
    lexer->i += 1;
    lexer->c = lexer->value[lexer->i];
  } else {
    printf("Lexer out of range.\n");
  }
}

void css_lexer_skip_whitespace(CSSLexer *lexer) {
  while ((lexer->c == ' ' || lexer->c == '\r' || lexer->c == '\n' ||
          lexer->c == '\t') &&
         !(LEXER_DONE(lexer))) {
    css_lexer_advance(lexer);
  }
}

CSSToken *css_lexer_parse_id(CSSLexer *lexer) {
  char *value = 0;

  while ((isalnum(lexer->c) || lexer->c == '-') && !LEXER_DONE(lexer)) {
    STR_APPEND_CHAR(value, lexer->c);
    css_lexer_advance(lexer);
  }

  CSSToken *token = init_css_token(TOKEN_ID, value);

  free(value);

  return token;
}

CSSToken *css_lexer_parse_string(CSSLexer *lexer) {
  char *value = 0;
  char delim = lexer->c;
  css_lexer_advance(lexer);

  while (lexer->c != delim && !LEXER_DONE(lexer)) {
    STR_APPEND_CHAR(value, lexer->c);
    css_lexer_advance(lexer);
  }

  CSSToken *token = init_css_token(TOKEN_STR, value);

  free(value);

  return token;
}

CSSToken *css_lexer_parse_number(CSSLexer *lexer) {
  char *value = 0;

  int type = TOKEN_INT;

  while ((isdigit(lexer->c)) && !LEXER_DONE(lexer)) {
    STR_APPEND_CHAR(value, lexer->c);
    css_lexer_advance(lexer);
  }

  if (lexer->c == '.') {
    type = TOKEN_FLOAT;
    STR_APPEND_CHAR(value, lexer->c);
    css_lexer_advance(lexer);

    while ((isdigit(lexer->c)) && !LEXER_DONE(lexer)) {
      STR_APPEND_CHAR(value, lexer->c);
      css_lexer_advance(lexer);
    }
  }

  CSSToken *token = init_css_token(type, value);

  free(value);

  return token;
}

CSSToken *css_lexer_parse_hash(CSSLexer *lexer) {
  char *value = 0;
  char delim = lexer->c;
  STR_APPEND_CHAR(value, lexer->c);
  css_lexer_advance(lexer);

  while (lexer->c != delim && (isdigit(lexer->c) || isalnum(lexer->c)) && !LEXER_DONE(lexer)) {
    STR_APPEND_CHAR(value, lexer->c);
    css_lexer_advance(lexer);
  }

  CSSToken *token = init_css_token(TOKEN_STR, value);

  free(value);

  return token;
}

CSSToken *css_lexer_next_token(CSSLexer *lexer) {
  while (!(LEXER_DONE(lexer))) {
    css_lexer_skip_whitespace(lexer);

    if (isdigit(lexer->c))
      return css_lexer_parse_number(lexer);
    if (isalnum(lexer->c))
      return css_lexer_parse_id(lexer);

    switch (lexer->c) {
    case '\'':
    case '"':
      return css_lexer_parse_string(lexer);
    case '#':
      return css_lexer_parse_hash(lexer);
    case '{':
      LEXER_TOK(lexer, TOKEN_LBRACE);
      break;
    case '}':
      LEXER_TOK(lexer, TOKEN_RBRACE);
      break;
    case ',':
      LEXER_TOK(lexer, TOKEN_COMMA);
      break;
    case ';':
      LEXER_TOK(lexer, TOKEN_SEMI);
      break;
    case ':':
      LEXER_TOK(lexer, TOKEN_COLON);
      break;
    case '.':
      LEXER_TOK(lexer, TOKEN_DOT);
      break;
    default: {
      printf("Lexer: Unexpected character `%c`\n", lexer->c);
      exit(1);
    } break;
    }
  }

  return init_css_token(TOKEN_EOF, "\0");
}

void css_lexer_free(CSSLexer *lexer) {
  if (lexer->value)
    free(lexer->value);

  free(lexer);
}
