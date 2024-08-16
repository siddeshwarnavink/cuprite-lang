#include <cstdio>
#include <gtest/gtest.h>

extern "C" {
#include <glib.h>

#include "core/lexer.h"
#include "core/token.h"
}

#include "../helpers/token_list_helpers.h"

TEST(LexerTest, TestArithmetic) {
  token_list list;
  token_list_create(&list);

  char stm1[] = "\t1 +2 + 3";
  parse_line(&list, stm1);

  token_list_pp(list);

  EXPECT_EQ(list->size, 5);

  check_token_list(&list, 1, token_num_int, "1");
  check_token_list(&list, 2, token_plus);
  check_token_list(&list, 3, token_num_int, "2");
  check_token_list(&list, 4, token_plus);
  check_token_list(&list, 5, token_num_int, "3");

  token_list_clear(&list);

  char stm2[] = "1.414 * 2.23";
  parse_line(&list, stm2);

  token_list_pp(list);

  EXPECT_EQ(list->size, 3);

  check_token_list(&list, 1, token_num_float, "1.414");
  check_token_list(&list, 2, token_asterisk);
  check_token_list(&list, 3, token_num_float, "2.23");

  token_list_clear(&list);

  char stm3[] = "(1.14 + 2.23) / 4";
  parse_line(&list, stm3);

  token_list_pp(list);

  EXPECT_EQ(list->size, 7);

  check_token_list(&list, 1, token_oparentheses);
  check_token_list(&list, 2, token_num_float, "1.14");
  check_token_list(&list, 3, token_plus);
  check_token_list(&list, 4, token_num_float, "2.23");
  check_token_list(&list, 5, token_cparentheses);
  check_token_list(&list, 6, token_fslash);
  check_token_list(&list, 7, token_num_int, "4");

  token_list_destroy(&list);
}

TEST(LexerTest, TestDeclarataion) {
  token_list list;
  token_list_create(&list);

  char stm1[] = "\ta = 10";
  parse_line(&list, stm1);

  token_list_pp(list);

  EXPECT_EQ(list->size, 3);

  check_token_list(&list, 1, token_identf, "a");
  check_token_list(&list, 2, token_equal);
  check_token_list(&list, 3, token_num_int, "10");

  token_list_clear(&list);

  char stm2[] = "\tname = \"Siddeshwar\"";
  parse_line(&list, stm2);

  token_list_pp(list);

  EXPECT_EQ(list->size, 3);

  check_token_list(&list, 1, token_identf, "name");
  check_token_list(&list, 2, token_equal);
  check_token_list(&list, 3, token_str, "Siddeshwar");

  token_list_clear(&list);

  token_list_destroy(&list);
}

TEST(LexerTest, TestOpratorBetween) {
  token_list list;
  token_list_create(&list);

  char stm1[] = "hello,world";
  parse_line(&list, stm1);

  token_list_pp(list);

  EXPECT_EQ(list->size, 3);

  check_token_list(&list, 1, token_identf, "hello");
  check_token_list(&list, 2, token_comma);
  check_token_list(&list, 3, token_identf, "world");

  token_list_clear(&list);

  char stm2[] = "\"hello\",\"world\"";
  parse_line(&list, stm2);

  token_list_pp(list);

  EXPECT_EQ(list->size, 3);

  check_token_list(&list, 1, token_str, "hello");
  check_token_list(&list, 2, token_comma);
  check_token_list(&list, 3, token_str, "world");

  token_list_clear(&list);

  token_list_destroy(&list);
}
