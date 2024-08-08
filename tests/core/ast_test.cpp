#include <cstdio>
#include <cstdlib>
#include <gtest/gtest.h>

extern "C" {
#include "../../src/core/ast.h"
}

TEST(AstTest, TestNode) {
  ast_node n;

  ast_data d1 = (ast_data)malloc(sizeof(int));
  d1->val_int = 69;
  ast_create_node(&n, ast_val_int, d1);
  EXPECT_EQ(n->data->val_int, 69);
  ast_destroy_node(&n);
  EXPECT_EQ(n, nullptr);

  ast_data d2 = (ast_data)malloc(sizeof(float));
  d2->val_float = 69.42;
  ast_create_node(&n, ast_val_float, d2);
  EXPECT_FLOAT_EQ(n->data->val_float, 69.42);
  ast_destroy_node(&n);
  EXPECT_EQ(n, nullptr);
}
