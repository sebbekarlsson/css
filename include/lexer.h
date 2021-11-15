#ifndef XCSS_LEXER_H
#define XCSS_LEXER_H
#include <token.h>
typedef struct {
  char *value;
  char c;
  long int i;
  long unsigned int len;
  long unsigned int row;
  long unsigned int col;
} CSSLexer;

#define LEXER_DONE(Clexer) (lexer->c == '\0' || lexer->i >= lexer->len)

CSSLexer *init_css_lexer(char *value);

void css_lexer_advance(CSSLexer *lexer);

CSSToken *css_lexer_next_token(CSSLexer *lexer);

void css_lexer_free(CSSLexer *lexer);

char css_lexer_peek(CSSLexer *lexer, long int index);

char *css_lexer_parse_string_until(CSSLexer *lexer, char c);

void css_lexer_skip_invalid_chars(CSSLexer* lexer);


void css_lexer_get_position_string(CSSLexer* lexer, char* dest);
#endif
