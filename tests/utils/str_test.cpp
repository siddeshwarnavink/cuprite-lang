#include <gtest/gtest.h>

extern "C" {
#include "../../src/utils/str.h"
}

TEST(StrTest, TestStringCreate) {
  const char *s = "hello";

  str *st;
  str_create(st, s);

  EXPECT_STREQ(str_val(st), s);

  str_destroy(st);
}

TEST(StrTest, TestStringDestroy) {
  const char *s = "hello";

  str st;
  str_create(&st, s);
  str_destroy(&st);

  ASSERT_EQ(st, nullptr);
}

TEST(StrTest, TestStringAppend) {
  const char *s1 = "hello";
  const char *s2 = "world";

  str *st;
  str_create(st, s1);
  str_append(st, s2);

  EXPECT_STREQ(str_val(st), "helloworld");
  EXPECT_EQ(str_size(st), 10);

  str_append_ch(st, '!');
  EXPECT_STREQ(str_val(st), "helloworld!");
  EXPECT_EQ(str_size(st), 11);

  str_destroy(st);
}

TEST(StrTest, TestStringClear) {
  const char *s = "hello";

  str *st;
  str_create(st, s);
  str_clear(st);

  EXPECT_STRNE(str_val(st), s);
  EXPECT_EQ(str_size(st), 0);

  str_destroy(st);
}
