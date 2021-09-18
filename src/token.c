#include <mem.h>
#include <string.h>
#include <token.h>

CSSToken *init_css_token(int type, char *value) {
  CSSToken *token = NEW(CSSToken);
  token->type = type;
  token->value = strdup(value);

  return token;
}

void css_token_free(CSSToken *token) {
  if (token->value)
    free(token->value);

  free(token);
}

CSSToken *css_token_clone(CSSToken *token) {
  return init_css_token(token->type, token->value);
}
