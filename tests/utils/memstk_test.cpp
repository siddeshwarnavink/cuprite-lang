#include <cstdlib>
#include <gtest/gtest.h>

extern "C" {
#include "core/token.h"
#include "utils/memstk.h"
}

TEST(MemstkTest, TestSimpleCleanup) {
  memstk_init();

  int *num = (int *)malloc(sizeof(int));
  memstk_push((void **)&num, free);

  *num = 69;

  memstk_clean();
  ASSERT_EQ(num, nullptr);
}

TEST(MemstkTest, TestComplexCleanup) {
  memstk_init();

  token_list list = nullptr;
  token_list_create(&list);

  memstk_push((void **)&list, [](void *itm) -> void {
    token_list l = (token_list)itm;
    token_list_destroy(&l);
  });

  memstk_clean();

  ASSERT_EQ(list, nullptr);
}
