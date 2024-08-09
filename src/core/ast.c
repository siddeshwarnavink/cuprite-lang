#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "ast.h"
#include "token.h"
#include "utils/str.h"

static bool _operator_token(token tok);
static int _arithmetic_operator_precedence(token_type type);
static void _extract_from_token(ast_node *node, token tok);
static void _make_arithmetic_node(ast_node *node, GQueue *operator_stack,
                                  GQueue *operand_stack);
static void _operand_stack_cleanup(void *itm);
static void _operator_stack_cleanup(void *itm);
static void _arithmetic_pp(char *label, ast_node node);

void ast_create_node(ast_node *node, ast_node_type type, ast_data data) {
  (*node) = malloc(sizeof(struct sAstNode));
  if (*node == NULL) {
    perror("Failed to allocate memory for node");
    exit(EXIT_FAILURE);
  }

  (*node)->type = type;
  (*node)->data = data;
}

void ast_destroy_node(ast_node *node) {
  if ((*node)->type == ast_arithmetic_add ||
      (*node)->type == ast_arithmetic_subtract ||
      (*node)->type == ast_arithmetic_multiply ||
      (*node)->type == ast_arithmetic_divide) {
    ast_destroy_node(&((*node)->data->arithmetic->left));
    ast_destroy_node(&((*node)->data->arithmetic->right));
    free((*node)->data->arithmetic);
  }
  free((*node)->data);
  free(*node);
  *node = NULL;
}

void ast_parse_tokens(token_list tokens) {
  if (tokens == NULL) {
    perror("No tokens to parse");
    exit(EXIT_FAILURE);
  }

  GQueue *operator_stack = g_queue_new(), *operand_stack = g_queue_new();

  for (unsigned int i = 0; i < tokens->size; i++) {
    token tok = tokens->tokens[i];

    // check if operand
    if (!_operator_token(tok)) {
      ast_node node;
      _extract_from_token(&node, tok);
      g_queue_push_tail(operand_stack, node);
    }

    // its operator
    else if (_operator_token(tok)) {
      token top;
      while ((top = (token)g_queue_peek_tail(operator_stack)) != NULL &&
             _arithmetic_operator_precedence(top->type) <=
                 _arithmetic_operator_precedence(tok->type)) {
        ast_node node;
        _make_arithmetic_node(&node, operator_stack, operand_stack);
        g_queue_push_tail(operand_stack, node);
      }
      g_queue_push_tail(operator_stack, tok);
    }
  }

  // process rest of stack
  while (!g_queue_is_empty(operator_stack)) {
    ast_node node;
    _make_arithmetic_node(&node, operator_stack, operand_stack);
    g_queue_push_tail(operand_stack, node);
  }

  while (!g_queue_is_empty(operand_stack)) {
    ast_node top = g_queue_pop_tail(operand_stack);

    printf("AST: ");
    ast_pp(top);
    printf("\n");

    ast_destroy_node(&top);
  }

  g_queue_free_full(operand_stack, _operand_stack_cleanup);
  g_queue_free_full(operator_stack, _operator_stack_cleanup);
}

void ast_pp(ast_node head) {
  switch (head->type) {
  case ast_val_int:
    printf("%d", head->data->val_int);
    break;
  case ast_val_float:
    printf("%.2f", head->data->val_float);
    break;
  case ast_arithmetic_add:
    _arithmetic_pp("add", head);
    break;
  case ast_arithmetic_subtract:
    _arithmetic_pp("sub", head);
    break;
  case ast_arithmetic_multiply:
    _arithmetic_pp("mul", head);
    break;
  case ast_arithmetic_divide:
    _arithmetic_pp("div", head);
    break;
  default:
    printf("<Unknown>");
  }
}

static void _arithmetic_pp(char *label, ast_node node) {
  printf("(%s ", label);
  ast_pp(node->data->arithmetic->left);
  printf(", ");
  ast_pp(node->data->arithmetic->right);
  printf(")");
}

static bool _operator_token(token tok) {
  return tok->type == token_plus || tok->type == token_hyphen ||
         tok->type == token_asterisk || tok->type == token_fslash ||
         tok->type == token_percent;
}

static int _arithmetic_operator_precedence(token_type type) {
  switch (type) {
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

static void _extract_from_token(ast_node *node, token tok) {
  *node = malloc(sizeof(ast_node));
  if (*node == NULL) {
    perror("Failed to allocate memory for node");
    exit(EXIT_FAILURE);
  }

  switch (tok->type) {
  case token_num_int:
    (*node)->type = ast_val_int;
    (*node)->data = (ast_data)malloc(sizeof(int));
    (*node)->data->val_int = atoi(str_val(&(tok->value)));
    break;
  case token_num_float:
    (*node)->type = ast_val_float;
    (*node)->data = (ast_data)malloc(sizeof(float));
    (*node)->data->val_float = atof(str_val(&(tok->value)));
    break;
  default:
    free(*node);
    *node = NULL;
  }
}

static void _make_arithmetic_node(ast_node *node, GQueue *operator_stack,
                                  GQueue *operand_stack) {
  ast_node opr1 = g_queue_pop_tail(operand_stack);
  ast_node opr2 = g_queue_pop_tail(operand_stack);
  token opt = g_queue_pop_tail(operator_stack);

  if (opr1 == NULL || opr2 == NULL || opt == NULL) {
    return;
  }

  ast_data node_d = (ast_data)malloc(sizeof(union uAstData));
  if (node_d == NULL) {
    perror("Failed to allocate memory for ast_data");
    exit(EXIT_FAILURE);
  }

  node_d->arithmetic =
      (ast_arithmetic_data)malloc(sizeof(struct sAstArithmeticData));
  if (node_d->arithmetic == NULL) {
    free(node_d);
    perror("Failed to allocate memory for ast_arithmetic_data");
    exit(EXIT_FAILURE);
  }

  node_d->arithmetic->left = opr2;
  node_d->arithmetic->right = opr1;
  switch (opt->type) {
  case token_plus:
    ast_create_node(node, ast_arithmetic_add, node_d);
    break;
  case token_hyphen:
    ast_create_node(node, ast_arithmetic_subtract, node_d);
    break;
  case token_asterisk:
    ast_create_node(node, ast_arithmetic_multiply, node_d);
    break;
  case token_fslash:
    ast_create_node(node, ast_arithmetic_divide, node_d);
    break;
  default:
    // TODO: handle error
    break;
  }

  token_destroy(&opt);
}

static void _operand_stack_cleanup(void *itm) {
  ast_node node = itm;
  ast_destroy_node(&node);
}

static void _operator_stack_cleanup(void *itm) {
  token tok = itm;
  token_destroy(&tok);
}
