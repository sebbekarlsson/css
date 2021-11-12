#ifndef XCSS_TOKEN_H
#define XCSS_TOKEN_H
typedef struct {
  char *value;

  enum {
    TOKEN_ID,
    TOKEN_SEMI,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_STR,
    TOKEN_COMMA,
    TOKEN_EQUALS,
    TOKEN_AND,
    TOKEN_TILDE,
    TOKEN_PLUS,
    TOKEN_COLON,
    TOKEN_COLON_COLON,
    TOKEN_DOT,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_DOUBLE,
    TOKEN_HASH,
    TOKEN_DIV,
    TOKEN_STATEMENT,
    TOKEN_GT,
    TOKEN_LT,
    TOKEN_PERCENTAGE,
    TOKEN_MINUS,
    TOKEN_IMPORTANT,
    TOKEN_CLASSNAME,
    TOKEN_URL,
    TOKEN_EOF
  } type;
} CSSToken;

CSSToken *init_css_token(int type, char *value);

CSSToken *css_token_clone(CSSToken *token);

void css_token_free(CSSToken *token);
#endif
