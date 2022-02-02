#include <ctype.h>
#include <lexer.h>
#include <mem.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
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
  lexer->row = 1;

  return lexer;
}

void css_lexer_advance(CSSLexer *lexer) {
  if (lexer->i < lexer->len) {
    lexer->i += 1;
    lexer->c = lexer->value[lexer->i];

    if (lexer->c == '\n') {
      lexer->row += 1;
      lexer->col = 0;
    } else  {
      lexer->col += 1;
    }
  } else {
    printf("(CSS): Lexer out of range.\n");
  }
}

void css_lexer_skip_whitespace(CSSLexer *lexer) {
  while ((lexer->c == ' ' || lexer->c == '\r' || lexer->c == '\n' ||
          lexer->c == '\t' || lexer->c == 27) &&
         !(LEXER_DONE(lexer))) {
    css_lexer_advance(lexer);
  }
}

void css_lexer_skip_invalid_chars(CSSLexer* lexer) {
  while (lexer->c < 0 &&!(LEXER_DONE(lexer))) {
    printf("(CSS) Skipping unknown byte `%c` (%d)\n", lexer->c, lexer->c);
    css_lexer_advance(lexer);
  }
}


CSSToken *css_lexer_parse_id(CSSLexer *lexer) {
  char *value = 0;

  while ((isalnum(lexer->c) ||
          ((lexer->c == '-' && !isdigit(css_lexer_peek(lexer, 1) &&
                                        css_lexer_peek(lexer, 1) != ' '))) ||
          lexer->c == '!' || lexer->c == '_' || lexer->c == '.') &&
         !LEXER_DONE(lexer)) {
    STR_APPEND_CHAR(value, lexer->c);
    css_lexer_advance(lexer);
  }

  int type = TOKEN_ID;


  if (value) {
    if (value[0] == '.') {
      type = TOKEN_CLASSNAME;
    } else if (strcmp(value, "!important") == 0) {
      type = TOKEN_IMPORTANT;
    } else if (strcmp(value, "url") == 0) {
      type = TOKEN_URL;
    }
  }

  CSSToken *token = init_css_token(type, value);

  free(value);

  return token;
}

CSSToken *css_lexer_parse_statement(CSSLexer *lexer) {
  char *value = 0;

  STR_APPEND_CHAR(value, lexer->c);
  css_lexer_advance(lexer);

  while ((!(lexer->c == '\r' || lexer->c == '\n' || lexer->c == ' ' ||
            (lexer->c == ';' && lexer->c != '"' && lexer->c != '\''))) &&
         !LEXER_DONE(lexer)) {
    STR_APPEND_CHAR(value, lexer->c);
    css_lexer_advance(lexer);
  }

  CSSToken *token = init_css_token(TOKEN_STATEMENT, value);

  free(value);

  return token;
}

CSSToken *css_lexer_parse_string(CSSLexer *lexer) {
  char *value = 0;
  char delim = lexer->c;
  css_lexer_advance(lexer);

  while (!(lexer->c == delim && lexer->c != '\n' && lexer->c != '\r') &&
         !LEXER_DONE(lexer)) {
    STR_APPEND_CHAR(value, lexer->c);
    css_lexer_advance(lexer);
  }

  if (lexer->c == delim)
    css_lexer_advance(lexer);

  CSSToken *token = init_css_token(TOKEN_STR, value);

  free(value);

  return token;
}

CSSToken *css_lexer_parse_number(CSSLexer *lexer) {
  char *value = 0;
  int type = lexer->c == '\\' ? TOKEN_ESCAPED_INT : lexer->c == '.' ? TOKEN_FLOAT : TOKEN_INT;

  if (lexer->c == '\\')
    css_lexer_advance(lexer);

  if (lexer->c == '-') {
    STR_APPEND_CHAR(value, lexer->c);
    css_lexer_advance(lexer);
  }


  if (lexer->c == '.') {
    STR_APPEND_CHAR(value, '0');
    STR_APPEND_CHAR(value, lexer->c);
    css_lexer_advance(lexer);
  }


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

  while (lexer->c != delim && (isdigit(lexer->c) || isalnum(lexer->c)) &&
         !LEXER_DONE(lexer)) {
    STR_APPEND_CHAR(value, lexer->c);
    css_lexer_advance(lexer);
  }

  CSSToken *token = init_css_token(TOKEN_HASH, value);

  free(value);

  return token;
}

void css_lexer_skip_comment(CSSLexer *lexer) {

  if (lexer->c == '/') {
    if (css_lexer_peek(lexer, 1) == '/') {
      while (lexer->c != '\n' && lexer->c != 0)
        css_lexer_advance(lexer);
    } else if (css_lexer_peek(lexer, 1) == '*') {
      while (1) {
        if (lexer->c == '*' && css_lexer_peek(lexer, 1) == '/') {
          css_lexer_advance(lexer);
          css_lexer_advance(lexer);
          break;
        }

        css_lexer_advance(lexer);

        if (lexer->c == 0)
          break;
      }
    }
  }

  css_lexer_skip_whitespace(lexer);
}

CSSToken *_css_lexer_next_token(CSSLexer *lexer) {
  while (!(LEXER_DONE(lexer))) {
    css_lexer_skip_whitespace(lexer);

    if (lexer->c == '/') {
      while (css_lexer_peek(lexer, 1) == '/' ||
             css_lexer_peek(lexer, 1) == '*') {
        css_lexer_skip_comment(lexer);
        css_lexer_skip_whitespace(lexer);
        continue;
      }
    }

    if (lexer->c == '.' && isalnum(css_lexer_peek(lexer, 1))) {
      return css_lexer_parse_id(lexer);
    }

    css_lexer_skip_whitespace(lexer);
    css_lexer_skip_invalid_chars(lexer);

    if (isdigit(lexer->c) || lexer->c == '.' || (lexer->c == '-' && isdigit(css_lexer_peek(lexer, 1))))
      return css_lexer_parse_number(lexer);
    if ((isalnum(lexer->c) ||
         (lexer->c == '-' && css_lexer_peek(lexer, 1) != ' ' &&
          !isdigit(css_lexer_peek(lexer, 1))) ||
         lexer->c == '!' || lexer->c == '_'))
      return css_lexer_parse_id(lexer);

    if (lexer->c == '@')
      return css_lexer_parse_statement(lexer);

    if (lexer->c == ':' && css_lexer_peek(lexer, 1) == ':') {
      CSSToken *tok = init_css_token(TOKEN_COLON_COLON, (char *)"::");
      css_lexer_advance(lexer);
      css_lexer_advance(lexer);
      return tok;
    }

    if (lexer->c == '\\' && css_lexer_peek(lexer, 1) == '"') {
      css_lexer_advance(lexer);
      css_lexer_advance(lexer);

      char* str = 0;

      while (!LEXER_DONE(lexer)) {
        STR_APPEND_CHAR(str, lexer->c);
        css_lexer_advance(lexer);
        if (lexer->c == '\\' && css_lexer_peek(lexer, 1) == '"') break;
      }

      css_lexer_advance(lexer);
      css_lexer_advance(lexer);

      CSSToken* tok = init_css_token(TOKEN_STR, str);
      return tok;
    }

    if (lexer->c == '\\' && isdigit(css_lexer_peek(lexer, 1))) {
      return css_lexer_parse_number(lexer);
    }

    css_lexer_skip_whitespace(lexer);

    switch (lexer->c) {
    case '\'':
    case '"':
      return css_lexer_parse_string(lexer);
      break;
    case '#':
      return css_lexer_parse_hash(lexer);
      break;
    case '{':
      LEXER_TOK(lexer, TOKEN_LBRACE);
      break;
    case '}':
      LEXER_TOK(lexer, TOKEN_RBRACE);
      break;
    case '/':
      LEXER_TOK(lexer, TOKEN_DIV);
      break;
    case '*':
      LEXER_TOK(lexer, TOKEN_STAR);
      break;
    case '^':
      LEXER_TOK(lexer, TOKEN_SQUARED);
      break;
    case ',':
      LEXER_TOK(lexer, TOKEN_COMMA);
      break;
    case '~':
      LEXER_TOK(lexer, TOKEN_TILDE);
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
    case '>':
      LEXER_TOK(lexer, TOKEN_GT);
      break;
    case '<':
      LEXER_TOK(lexer, TOKEN_LT);
      break;
    case '(':
      LEXER_TOK(lexer, TOKEN_LPAREN);
      break;
    case ')':
      LEXER_TOK(lexer, TOKEN_RPAREN);
      break;
    case '[':
      LEXER_TOK(lexer, TOKEN_LBRACKET);
      break;
    case ']':
      LEXER_TOK(lexer, TOKEN_RBRACKET);
      break;
    case '=':
      LEXER_TOK(lexer, TOKEN_EQUALS);
      break;
    case '%':
      LEXER_TOK(lexer, TOKEN_PERCENTAGE);
      break;
    case '-':
      LEXER_TOK(lexer, TOKEN_MINUS);
      break;
    case '+':
      LEXER_TOK(lexer, TOKEN_PLUS);
      break;
    default: {
      if (lexer->c != '\0') {
//        LEXER_TOK(lexer, TOKEN_JUNK);
        char buff[256];
        css_lexer_get_position_string(lexer, buff);
        printf("(CSS) Lexer (%s): Unexpected character `%c` (%d)\n", buff, lexer->c, lexer->c);
        exit(1);
      }
    } break;
    }

    css_lexer_skip_whitespace(lexer);
  }

  return init_css_token(TOKEN_EOF, "\0");
}

CSSToken *css_lexer_next_token(CSSLexer *lexer) {
  CSSToken *tok = _css_lexer_next_token(lexer);

  if (tok->value[0] == '@' && strstr(tok->value, "font-face") == 0) {
    tok->special = 1;
  }
  if (strcmp(tok->value, "to") == 0)
    tok->type = TOKEN_STATEMENT;
  else if (strcmp(tok->value, "@media") == 0 ||
           strcmp(tok->value, "@page") == 0)
    tok->type = TOKEN_ID;

  return tok;
}

void css_lexer_free(CSSLexer *lexer) {
  if (lexer->value)
    free(lexer->value);

  free(lexer);
}

char css_lexer_peek(CSSLexer *lexer, long int index) {
  index = MAX(0, MIN(lexer->len - 1, lexer->i + index));

  return lexer->value[index];
}

char *css_lexer_parse_string_until(CSSLexer *lexer, char c) {
  char *s = 0;

  while (lexer->c != c) {
    STR_APPEND_CHAR(s, lexer->c);
    css_lexer_advance(lexer);
  }

  return s;
}


void css_lexer_get_position_string(CSSLexer* lexer, char* dest) {
  sprintf(dest, "%ld:%ld", lexer->row, lexer->col);
}
