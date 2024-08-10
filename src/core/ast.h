#ifndef AST_H
#define AST_H

#include "token.h"

/**
 * @file ast.h
 */

typedef enum {
  ast_declare,
  ast_arithmetic_add,
  ast_arithmetic_subtract,
  ast_arithmetic_multiply,
  ast_arithmetic_divide,
  ast_val_int,
  ast_val_float,
  ast_str
} ast_node_type;

typedef struct sAstNode *ast_node;

typedef struct sAstVarDeclare {
  str name;
  ast_node value;
} *ast_var_declare;

typedef struct sAstArithmeticData {
  ast_node left;
  ast_node right;
} *ast_arithmetic_data;

typedef union uAstData {
  ast_var_declare var_declare;
  ast_arithmetic_data arithmetic;
  int val_int;
  float val_float;
  str val_str;
} *ast_data;

typedef struct sAstNode {
  ast_node_type type;
  ast_data data;
} sAstNode;

/**
 * @brief Create a AST node.
 * @param node Node.
 * @param type Node type.
 * @param data Node data.
 */
void ast_create_node(ast_node *node, ast_node_type type, ast_data data);

/**
 * @brief Destroy a AST node.
 * @param node Node.
 */
void ast_destroy_node(ast_node *node);

/**
 * @brief Parse variable declaration
 * @param tokens Tokens list.
 */
ast_node ast_parse_variable_declaration(token_list tokens);

/**
 * @brief Parse expression
 * @param tokens Tokens list.
 */
ast_node ast_parse_expression(token_list tokens);

/**
 * @brief Parse tokens to create AST.
 * @param tokens Tokens list.
 */
void ast_parse_tokens(token_list tokens);

/**
 * @brief Pretty-print the AST.
 * @param head Root node of AST.
 */
void ast_pp(ast_node head);

#endif
