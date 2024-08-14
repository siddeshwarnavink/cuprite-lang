#include "token_list_helpers.h"

void check_token_list(token_list *list, unsigned int index,
                      token_type expected_type, const char *expected_value) {
  GList *nth_element = g_list_nth((*list)->tokens, index - 1);
  if (nth_element != NULL) {
    token t = (token)nth_element->data;
    EXPECT_EQ(t->type, expected_type);

    if (expected_value) {
      EXPECT_STREQ(str_val(&t->value), expected_value);
    }
  } else {
    FAIL() << "Token at index " << index << " is NULL";
  }
}

void token_list_pp(token_list list) {
  GList *iter;
  for (iter = list->tokens; iter != NULL; iter = iter->next) {
    token tok = (token)iter->data;
    token_pp(tok);
  }
}
