#ifndef TOKEN_LIST_HELPERS_H
#define TOKEN_LIST_HELPERS_H

#include <gtest/gtest.h>

extern "C" {
#include "core/token.h"
}

void check_token_list(token_list *list, unsigned int index,
                      token_type expected_type,
                      const char *expected_value = nullptr);
void token_list_pp(token_list list);

#endif
