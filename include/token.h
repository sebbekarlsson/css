#ifndef CSS_TOKEN_H
#define CSS_TOKEN_H
typedef struct {
  char *value;

  enum {
    TOKEN_ID,
    TOKEN_SEMI,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_STR,
    TOKEN_COMMA,
    TOKEN_AND,
    TOKEN_TILDE,
    TOKEN_PLUS,
    TOKEN_COLON,
    TOKEN_DOT,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_DOUBLE,
    TOKEN_EOF
  } type;
} CSSToken;

CSSToken *init_css_token(int type, char *value);

CSSToken *css_token_clone(CSSToken *token);

void css_token_free(CSSToken *token);
#endif
