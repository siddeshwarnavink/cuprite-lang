#include "ast.h"
#include "token.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool _operator_token(token tok);
static int _arithmetic_operator_precedence(token op);

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

  if ((*node)->data == NULL) {
    perror("Failed to allocate memory for node data");
    exit(EXIT_FAILURE);
  }

  (*node)->data = data;
}

void ast_destroy_node(ast_node *node) {
  free((*node)->data);
  free(*node);
  *node = NULL;
}

void ast_parse_tokens(ast_node *head, token_list *tokens) {
  token_stack operator_stack = NULL;
  token_stack operand_stack = NULL;

  for (unsigned int i = 0; i < (*tokens)->size; i++) {
    token tok = (*tokens)->tokens[i];

    if (!_operator_token(tok) &&
        ((i + 1 < (*tokens)->size &&
          _operator_token((*tokens)->tokens[i + 1])) ||
         (i - 1 >= 0 && _operator_token((*tokens)->tokens[i - 1])))) {
      if (operand_stack == NULL) {
        token_stack_create(&operand_stack, tok);
      } else {
        token_stack_push(&operand_stack, tok);
      }
    }

    else {
      while (_arithmetic_operator_precedence(token_stack_top(
                 &operator_stack)) <= _arithmetic_operator_precedence(tok)) {
        token opr1 = token_stack_pop(&operand_stack);
        token opr2 = token_stack_pop(&operand_stack);
        token opt = token_stack_pop(&operator_stack);
        ast_node node;

        ast_node opr1_node;
        ast_node opr2_node;

        if (opr1->type == token_num_int && opr2->type == token_num_int) {
          ast_data opr1_ndata = (ast_data)malloc(sizeof(int));
          opr1_ndata->val_int = atoi(str_val(&(opr1->value)));
          ast_create_node(&opr1_node, ast_val_int, opr1_ndata);

          ast_data opr2_ndata = (ast_data)malloc(sizeof(int));
          opr2_ndata->val_int = atoi(str_val(&(opr2->value)));
          ast_create_node(&opr2_node, ast_val_int, opr2_ndata);
        } else if (opr1->type == token_num_float &&
                   opr2->type == token_num_float) {
          ast_data opr1_ndata = (ast_data)malloc(sizeof(float));
          opr1_ndata->val_float = atof(str_val(&(opr1->value)));
          ast_create_node(&opr1_node, ast_val_float, opr1_ndata);

          ast_data opr2_ndata = (ast_data)malloc(sizeof(float));
          opr2_ndata->val_float = atof(str_val(&(opr2->value)));
          ast_create_node(&opr2_node, ast_val_float, opr2_ndata);
        } else {
          // TODO: handle error
        }

        ast_data *noded = (ast_data *)malloc(sizeof(struct sAstArithmeticData));
        (*noded)->arithmetic->left = opr1_node;
        (*noded)->arithmetic->left = opr2_node;

        switch (opt->type) {
        case token_plus:
          ast_create_node(&node, ast_arithmetic_add, *noded);
          break;
        case token_hyphen:
          ast_create_node(&node, ast_arithmetic_subtract, *noded);
          break;
        case token_asterisk:
          ast_create_node(&node, ast_arithmetic_multiply, *noded);
          break;
        case token_fslash:
          ast_create_node(&node, ast_arithmetic_divide, *noded);
          break;
        default:
          // TODO: handle error
          break;
        }
      }
    }
  }
}

static bool _operator_token(token tok) {
  return tok->type == token_plus || tok->type == token_hyphen ||
         tok->type == token_asterisk || tok->type == token_fslash ||
         tok->type == token_percent;
}

static int _arithmetic_operator_precedence(token op) {
  switch (op->type) {
  case token_fslash:
    return 1;
  case token_asterisk:
    return 2;
  case token_percent:
    return 3;
  case token_plus:
    return 4;
  case token_hyphen:
    return 5;
  default:
    return 69;
  }
}
