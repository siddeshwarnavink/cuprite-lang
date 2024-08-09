#include <cstdio>
#include <cstdlib>
#include <gtest/gtest.h>

extern "C" {
#include "../../src/core/ast.h"
#include "../../src/core/lexer.h"
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

TEST(AstTest, TestArithmetic) {
  token_list list;
  token_list_create(&list);

  char stm1[] = "1+2*3";
  parse_line(&list, stm1);

  ast_parse_tokens(list);

  EXPECT_TRUE(true);

  token_list_destroy(&list);
}
