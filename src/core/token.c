#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "utils/str.h"

static const char *_get_token_label(token_type type);
static bool _has_data(token_type type);

void token_list_create(token_list *list) {
  *list = malloc(sizeof(struct sToken_list));
  if (*list == NULL) {
    perror("Failed to allocate memory for token_list");
    exit(EXIT_FAILURE);
  }
  (*list)->tokens = NULL;
  (*list)->size = 0;
}

void token_list_append(token_list *list, token *tok) {
  if ((*list)->tokens == NULL) {
    (*list)->tokens = malloc(sizeof(token));
  } else {
    token *new_tokens =
        realloc((*list)->tokens, ((*list)->size + 1) * sizeof(token));
    if (new_tokens == NULL) {
      perror("Failed to allocate memory for append token_list");
      exit(EXIT_FAILURE);
    }
    (*list)->tokens = new_tokens;
  }

  (*list)->tokens[(*list)->size] = *tok;
  (*list)->size++;
}

void token_list_clear(token_list *list) {
  token_list_destroy(list);
  token_list_create(list);
}

void token_list_destroy(token_list *list) {
  if (*list != NULL) {
    if ((*list)->tokens != NULL) {
      for (unsigned int i = 0; i < (*list)->size; i++) {
        token tok = (*list)->tokens[i];
        if (tok != NULL) {
          if (_has_data(tok->type)) {
            str my_str = tok->value;
            str_destroy(&my_str);
          }
          free(tok);
          (*list)->tokens[i] = NULL;
        }
      }
      free((*list)->tokens);
    }
    free(*list);
    *list = NULL;
  }
}

void token_create(token *token, token_type type, char *value) {
  (*token) = malloc(sizeof(struct sToken));
  if (*token == NULL) {
    perror("Failed to allocate memory for token");
    exit(EXIT_FAILURE);
  }

  (*token)->type = type;

  if (value != NULL && _has_data(type)) {
    str_create(&((*token)->value), value);
  } else {
    (*token)->value = NULL;
  }
}

void token_pp(token tok) {
  if (tok != NULL) {
    if (_has_data(tok->type)) {
      printf("Token(type:\"%s\", value: \"%s\")\n", _get_token_label(tok->type),
             tok->value != NULL ? str_val(&(tok->value)) : "<NULL>");
    } else {
      printf("Token(type:\"%s\")\n", _get_token_label(tok->type));
    }
  }
}

void token_destroy(token *token) {
  str my_str = (*token)->value;
  str_destroy(&my_str);
  free(*token);
  *token = NULL;
}

token token_cpy(token tok) {
  token tok_new;
  token_create(&tok_new, tok->type,
               _has_data(tok->type) ? str_val(&(tok->value)) : NULL);
  return tok_new;
}

static const char *_get_token_label(token_type type) {
  switch (type) {
  case token_identf:
    return "IDENTIFIER";
  case token_num_int:
    return "INT";
  case token_num_float:
    return "FLOAT";
  case token_oparentheses:
    return "(";
  case token_cparentheses:
    return ")";
  case token_plus:
    return "+";
  case token_hyphen:
    return "-";
  case token_asterisk:
    return "*";
  case token_fslash:
    return "/";
  case token_percent:
    return "%";
  case token_equal:
    return "=";
  case token_eos:
    return "EoS";
  case token_eof:
    return "EoF";
  default:
    return "UNKNOWN";
  }
}

static bool _has_data(token_type type) {
  switch (type) {
  case token_num_int:
  case token_num_float:
  case token_identf:
    return true;
  default:
    return false;
  }
}
