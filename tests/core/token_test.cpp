#include <gtest/gtest.h>

extern "C" {
#include "core/token.h"
}

TEST(TokenTest, TestTokenList) {
  token_list list;
  create_token_list(&list);

  token tok;
  create_token(&tok, token_num_int, "69");
  append_token_list(&list, &tok);

  EXPECT_STREQ(list->tokens[0]->value->data, "69");

  destroy_token_list(&list);

  EXPECT_EQ(list, nullptr);
}
