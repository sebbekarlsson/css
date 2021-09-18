#ifndef XCSS_LEXER_H
#define XCSS_LEXER_H
#include <token.h>
typedef struct {
  char *value;
  char c;
  long int i;
  long unsigned int len;
} CSSLexer;

#define LEXER_DONE(Clexer) (lexer->c == '\0' || lexer->i >= lexer->len)

CSSLexer *init_css_lexer(char *value);

void css_lexer_advance(CSSLexer *lexer);

CSSToken *css_lexer_next_token(CSSLexer *lexer);

void css_lexer_free(CSSLexer *lexer);
#endif
