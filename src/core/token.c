#include <stdio.h>
#include <stdlib.h>

#include "token.h"

void create_token_list(token_list *list) {
  *list = malloc(sizeof(struct sToken_list));
  if (*list == NULL) {
    perror("Failed to allocate memory for token_list");
    exit(EXIT_FAILURE);
  }
  (*list)->tokens = NULL;
  (*list)->size = 0;
}

void append_token_list(token_list *list, token *tok) {
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

void destroy_token_list(token_list *list) {
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
  case token_plus:
    return "+";
  default:
    return "UNKNOWN";
  }
}

void create_token(token *token, token_type type, char *value) {
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
