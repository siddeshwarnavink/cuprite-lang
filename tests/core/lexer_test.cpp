#include <cstdio>
#include <gtest/gtest.h>

extern "C" {
#include "core/lexer.h"
#include "core/token.h"
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

  char stm2[] = "1.414 * 2.23";
  parse_line(&list, stm2);
  EXPECT_EQ(list->size, 3);
  EXPECT_EQ(list->tokens[0]->type, token_num_float);
  EXPECT_STREQ(str_val(&(list->tokens[0]->value)), "1.414");
  EXPECT_EQ(list->tokens[1]->type, token_asterisk);
  EXPECT_EQ(list->tokens[2]->type, token_num_float);
  EXPECT_STREQ(str_val(&(list->tokens[2]->value)), "2.23");
  token_list_clear(&list);

  char stm3[] = "(1.14 + 2.23) / 4";
  parse_line(&list, stm3);
  EXPECT_EQ(list->size, 7);
  EXPECT_EQ(list->tokens[0]->type, token_oparentheses);
  EXPECT_EQ(list->tokens[1]->type, token_num_float);
  EXPECT_STREQ(str_val(&(list->tokens[1]->value)), "1.14");
  EXPECT_EQ(list->tokens[2]->type, token_plus);
  EXPECT_EQ(list->tokens[3]->type, token_num_float);
  EXPECT_STREQ(str_val(&(list->tokens[3]->value)), "2.23");
  EXPECT_EQ(list->tokens[4]->type, token_cparentheses);
  EXPECT_EQ(list->tokens[5]->type, token_fslash);
  EXPECT_STREQ(str_val(&(list->tokens[6]->value)), "4");
  EXPECT_EQ(list->tokens[6]->type, token_num_int);

  token_list_destroy(&list);
}

TEST(LexerTest, TestDeclarataion) {
  token_list list;
  token_list_create(&list);

  char stm1[] = "\ta = 10";
  parse_line(&list, stm1);
  EXPECT_EQ(list->size, 3);
  EXPECT_EQ(list->tokens[0]->type, token_identf);
  EXPECT_STREQ(str_val(&(list->tokens[0]->value)), "a");
  EXPECT_EQ(list->tokens[1]->type, token_equal);
  EXPECT_EQ(list->tokens[2]->type, token_num_int);
  EXPECT_STREQ(str_val(&(list->tokens[2]->value)), "10");
  token_list_clear(&list);

  char stm2[] = "\tname = \"Siddeshwar\"";
  parse_line(&list, stm2);
  EXPECT_EQ(list->size, 3);
  EXPECT_EQ(list->tokens[0]->type, token_identf);
  EXPECT_STREQ(str_val(&(list->tokens[0]->value)), "name");
  EXPECT_EQ(list->tokens[1]->type, token_equal);
  EXPECT_EQ(list->tokens[2]->type, token_str);
  EXPECT_STREQ(str_val(&(list->tokens[2]->value)), "Siddeshwar");
  token_list_clear(&list);

  token_list_destroy(&list);
}
