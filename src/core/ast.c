#include "ast.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

void ast_create_node(ast_node *node, ast_node_type type, ast_data data) {
  (*node) = malloc(sizeof(struct sAstNode));
  if (*node == NULL) {
    perror("Failed to allocate memory for node");
    exit(EXIT_FAILURE);
  }

  (*node)->type = type;

  switch (type) {
  case ast_arithmetic_add:
  case ast_arithmetic_subtract:
  case ast_arithmetic_multiply:
  case ast_arithmetic_divide:
    (*node)->data = malloc(sizeof(struct sAstArithmeticData));
    break;
  case ast_val_int:
    (*node)->data = malloc(sizeof(int));
    break;
  case ast_val_float:
    (*node)->data = malloc(sizeof(float));
    break;
  }
  (*node)->data = data;

  if ((*node)->data == NULL) {
    perror("Failed to allocate memory for node data");
    exit(EXIT_FAILURE);
  }
}

void ast_destroy_node(ast_node *node) {
  free((*node)->data);
  free(*node);
  *node = NULL;
}

void ast_parse_tokens(ast_node *head, token_list *tokens) {
  for (unsigned int i = 0; i < (*tokens)->size; i++) {
    token tok = (*tokens)->tokens[i];

    //...
  }
}
