#include <cstdio>
#include <cstdlib>
#include <gtest/gtest.h>

extern "C" {
#include "core/ast.h"
#include "core/lexer.h"
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

  const unsigned int STMS_SIZE = 4;
  char stms[STMS_SIZE][20] = {"1+2*3", "1*2+3", "1+2+3", "1+2*2/3"};

  for (unsigned int i = 0; i < STMS_SIZE; i++) {
    printf("input: \"%s\"\n", stms[i]);
    parse_line(&list, stms[i]);
    ast_parse_tokens(list);

    EXPECT_TRUE(true);
    token_list_clear(&list);
  }

  token_list_destroy(&list);
}
