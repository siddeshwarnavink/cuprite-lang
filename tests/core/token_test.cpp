#include <gtest/gtest.h>

extern "C" {
#include "core/token.h"
}

#include "../helpers/token_list_helpers.h"

TEST(TokenTest, TestTokenList) {
  token_list list;
  token_list_create(&list);

  token tok1;
  token_create(&tok1, token_num_int, "69");
  token_list_append(&list, &tok1);
  check_token_list(&list, 1, token_num_int, "69");

  token tok2;
  token_create(&tok2, token_num_int, "420");
  token_list_append(&list, &tok2);
  check_token_list(&list, 2, token_num_int, "420");

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
  check_token_list(&list, 1, token_num_int, "420");

  token_list_destroy(&list);
}
