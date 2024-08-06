#include <stdio.h>
#include <stdlib.h>

#include "token.h"

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
  for (unsigned int i = 0; i < (*list)->size; i++) {
    str my_str = (*list)->tokens[i]->value;
    str_destroy(&my_str);
    free((*list)->tokens[i]);
  }

  token *new_tokens = realloc((*list)->tokens, sizeof(token));
  if (new_tokens == NULL) {
    perror("Failed to allocate memory for clear token_list");
    exit(EXIT_FAILURE);
  }
  (*list)->tokens = new_tokens;
  (*list)->size = 0;
}

void token_list_destroy(token_list *list) {
  if (*list != NULL) {
    if ((*list)->tokens != NULL) {
      for (unsigned int i = 0; i < (*list)->size; i++) {
        str my_str = (*list)->tokens[i]->value;
        str_destroy(&my_str);
        free((*list)->tokens[i]);
      }
      free((*list)->tokens);
    }
    free(*list);
    *list = NULL;
  }
}

static const char *_get_token_label(token_type type) {
  switch (type) {
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
  case token_eof:
    return "EoF";
  default:
    return "UNKNOWN";
  }
}

void token_create(token *token, token_type type, char *value) {
  (*token) = malloc(sizeof(struct sToken));
  if (*token == NULL) {
    perror("Failed to allocate memory for token");
    exit(EXIT_FAILURE);
  }

  (*token)->type = type;

  if (value != NULL) {
    str_create(&((*token)->value), value);
  }
}

void token_pp(token *token) {
  printf("Token(type:\"%s\", value: \"%s\")\n",
         _get_token_label((*token)->type),
         (*token)->value != NULL ? str_val(&((*token)->value)) : "<NULL>");
}
