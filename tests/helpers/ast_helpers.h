#ifndef AST_HELPERS_H
#define AST_HELPERS_H

#include <gtest/gtest.h>

extern "C" {
#include "core/ast.h"
}

using exp_cb = std::function<void(ast_expression_data)>;

/*
 * Check identifier
 */
void check_identf(ast_node node, const char *value);

void check_int(ast_node node, const int value);

void check_float(ast_node node, const float value);

/*
 * Check expression
 */
void check_exp(ast_node node, ast_node_type type, exp_cb cb);

#endif
