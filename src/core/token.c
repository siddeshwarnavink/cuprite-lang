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
    (*list)->tokens = malloc(sizeof(struct sToken));
    if ((*list)->tokens == NULL) {
      perror("Failed to allocate memory for append token_list");
      exit(EXIT_FAILURE);
    }

    (*list)->tokens[0] = *tok;
    (*list)->size = 1;
  } else {
    token_list *new_list = realloc(list, sizeof(struct sToken));
    if (new_list == NULL) {
      perror("Failed to allocate memory for append token_list");
      exit(EXIT_FAILURE);
    }

    (*list)->tokens[(*list)->size] = *tok;
    (*list)->size++;
  }
}

void destroy_token_list(token_list *list) {
  if (*list != NULL) {
    if ((*list)->tokens != NULL) {
      for (unsigned int i = 0; i < (*list)->size; i++) {
        free((*list)->tokens[i]->value);
        free((*list)->tokens[i]);
      }
      free((*list)->tokens);
    }
    free(*list);
    *list = NULL;
  }
}

void create_token(token *token, token_type type, char *value) {
  (*token) = malloc(sizeof(struct sToken));
  if (*token == NULL) {
    perror("Failed to allocate memory for token");
    exit(EXIT_FAILURE);
  }

  if (value != NULL) {
    str_create(&(*token)->value, value);
  }
}
