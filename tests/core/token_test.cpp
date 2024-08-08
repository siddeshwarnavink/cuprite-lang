#include <gtest/gtest.h>

extern "C" {
#include "../../src/core/token.h"
}

TEST(TokenTest, TestTokenList) {
  token_list list;
  token_list_create(&list);

  token tok1;
  token_create(&tok1, token_num_int, "69");
  token_list_append(&list, &tok1);
  EXPECT_STREQ(list->tokens[0]->value->data, "69");

  token tok2;
  token_create(&tok2, token_num_int, "420");
  token_list_append(&list, &tok2);
  EXPECT_STREQ(list->tokens[1]->value->data, "420");

  token_list_destroy(&list);

  EXPECT_EQ(list, nullptr);
}

TEST(TokenTest, TestTokenListClear) {
  token_list list;
  token_list_create(&list);

  token tok1;
  token_create(&tok1, token_num_int, "69");
  token_list_append(&list, &tok1);

  token_list_clear(&list);

  token tok2;
  token_create(&tok2, token_num_int, "420");
  token_list_append(&list, &tok2);

  EXPECT_EQ(list->size, 1);
  EXPECT_STREQ(list->tokens[0]->value->data, "420");
}

TEST(TokenTest, TestTokenStack) {
  token tok1;
  token_create(&tok1, token_num_int, "69");
  token tok2;
  token_create(&tok2, token_num_int, "420");

  token_stack stack = NULL;
  ASSERT_TRUE(token_stack_empty(&stack));

  token_stack_create(&stack, tok1);
  ASSERT_STREQ(str_val(&token_stack_top(&stack)->value), "69");
  ASSERT_FALSE(token_stack_empty(&stack));

  token_stack_push(&stack, tok2);
  ASSERT_STREQ(str_val(&(token_stack_pop(&stack)->value)), "420");

  token_stack_pop(&stack);
  ASSERT_TRUE(token_stack_empty(&stack));

  // cleanup
  token_list list;
  token_list_create(&list);
  token_list_append(&list, &tok1);
  token_list_append(&list, &tok2);
  token_list_destroy(&list);
}
