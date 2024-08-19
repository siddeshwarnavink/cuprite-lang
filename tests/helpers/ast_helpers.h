#ifndef AST_HELPERS_H
#define AST_HELPERS_H

#include <gtest/gtest.h>

extern "C" {
#include "core/ast.h"
}

void check_identf(ast_node node, const char *value);
void check_int(ast_node node, const int value);

#endif
