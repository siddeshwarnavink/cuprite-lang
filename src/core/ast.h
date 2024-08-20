#ifndef AST_H
#define AST_H

#include <glib.h>
#include <stdbool.h>

#include "token.h"
#include "utils/memstk.h"

#define ASTDATA_CLEANUP(type)                  \
    static void _##type##_cleanup(void *itm) { \
        ast_data data = (ast_data)itm;         \
        ast_destroy_node_data(&data, type);    \
    }

/**
 * @file ast.h
 */

typedef enum {
    ast_undef,
    ast_identf,
    ast_declare,
    ast_arithmetic_add,
    ast_arithmetic_subtract,
    ast_arithmetic_multiply,
    ast_arithmetic_divide,
    ast_val_int,
    ast_val_float,
    ast_str,
    ast_func_call,
    ast_bool,
    ast_cond_block,
    ast_cond_is,
    ast_cond_is_not,
    ast_cond_greater,
    ast_cond_greater_eq,
    ast_cond_lesser,
    ast_cond_lesser_eq,
    ast_logical_and,
    ast_logical_or,
    ast_logical_not
} ast_node_type;

typedef struct sAstNode *ast_node;

typedef struct sAstVarDeclare {
    str name;
    ast_node value;
} *ast_var_declare;

typedef struct sAstConditionBlock {
    ast_node expression;
    GList *statements;
} *ast_condition_block;

typedef struct sAstFCall {
    str name;
    GList *args;
} *ast_fcall;

typedef struct sAstExpressionData {
    ast_node left;
    ast_node right;
} *ast_expression_data;

typedef union uAstData {
    ast_var_declare var_declare;
    ast_expression_data expression;
    ast_fcall fcall;
    int val_int;
    float val_float;
    str val_str;
    bool val_bool;
} *ast_data;

typedef struct sAstNode {
    ast_node_type type;
    ast_data data;
    memstk_node *memstk_node, *data_memstk_node;
} sAstNode;

/**
 * @brief Create a AST node.
 * @param node Node.
 * @param type Node type.
 * @param data Node data.
 */
void ast_create_node(ast_node *node, ast_node_type type, ast_data data);

/**
 * @brief Create a AST node data.
 * @param data Node data.
 * @param type Node type.
 */
memstk_node *ast_create_node_data(ast_data *data, ast_node_type type);

/**
 * @brief Destroy a AST node.
 * @param node Node.
 */
void ast_destroy_node(ast_node *node);

/**
 * @brief Destroy a AST node data
 * @param data Node data
 * @param type Node type.
 */
void ast_destroy_node_data(ast_data *data, ast_node_type type);

/**
 * @brief Parse variable declaration
 * @param tokens Tokens list.
 */
ast_node ast_parse_variable_declaration(token_list tokens);

/**
 * @brief Parse condition block.
 * @param tokens Tokens list.
 */
ast_node ast_parse_condition_block(token_list tokens, GList *token_iter);

/**
 * @brief Parse function call
 * @param tokens Tokens list.
 */
ast_node ast_parse_function_call(token_list tokens);

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
