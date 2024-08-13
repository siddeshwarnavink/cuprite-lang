#include "token_list_helpers.h"

void check_token_list(token_list *list, unsigned int index,
                      token_type expected_type, const char *expected_value) {
  ASSERT_LT(index, (*list)->size) << "Index out of range";
  token t = (token)g_list_nth((*list)->tokens, index);
  EXPECT_EQ(t->type, expected_type);

  if (expected_value) {
    EXPECT_STREQ(str_val(&t->value), expected_value);
  }
}
