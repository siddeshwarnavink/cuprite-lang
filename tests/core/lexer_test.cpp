#include <cstdio>
#include <gtest/gtest.h>

extern "C" {
#include "../../src/core/lexer.h"
}

TEST(LexerTest, TestArithmetic) {
  token_list list;
  token_list_create(&list);

  char stm1[] = "\t1 +2 + 3";
  parse_line(&list, stm1);
  EXPECT_EQ(list->size, 5);
  EXPECT_EQ(list->tokens[0]->type, token_num_int);
  EXPECT_STREQ(str_val(&(list->tokens[0]->value)), "1");
  EXPECT_EQ(list->tokens[1]->type, token_plus);
  EXPECT_EQ(list->tokens[2]->type, token_num_int);
  EXPECT_STREQ(str_val(&(list->tokens[2]->value)), "2");
  EXPECT_EQ(list->tokens[3]->type, token_plus);
  EXPECT_EQ(list->tokens[4]->type, token_num_int);
  EXPECT_STREQ(str_val(&(list->tokens[4]->value)), "3");

  token_list_clear(&list);

  char stm2[] = "1.414 + 2.23";
  parse_line(&list, stm2);
  EXPECT_EQ(list->size, 3);
  EXPECT_EQ(list->tokens[0]->type, token_num_float);
  EXPECT_STREQ(str_val(&(list->tokens[0]->value)), "1.414");
  EXPECT_EQ(list->tokens[1]->type, token_plus);
  EXPECT_EQ(list->tokens[2]->type, token_num_float);
  EXPECT_STREQ(str_val(&(list->tokens[2]->value)), "2.23");

  token_list_destroy(&list);
}
