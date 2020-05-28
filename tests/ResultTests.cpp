#include <gtest/gtest.h>

#include <result/result.hpp>

GTEST_TEST(resultTest, test_ok) {
  const auto r = Result<int, int>{Ok(5)};
  ASSERT_TRUE(r.is_ok());
  ASSERT_FALSE(r.is_err());

  const auto o = r.ok();
  ASSERT_TRUE(o.has_value());

  const auto e = r.err();
  ASSERT_FALSE(e.has_value());

  ASSERT_EQ(r.unwrap(), 5);
  ASSERT_THROW(r.unwrap_err(), std::bad_variant_access);
}

GTEST_TEST(resultTest, test_err) {
  const auto r = Result<int, int>{Err(5)};
  ASSERT_FALSE(r.is_ok());
  ASSERT_TRUE(r.is_err());

  const auto o = r.ok();
  ASSERT_FALSE(o.has_value());

  const auto e = r.err();
  ASSERT_TRUE(e.has_value());

  ASSERT_EQ(r.unwrap_err(), 5);
  ASSERT_THROW(r.unwrap(), std::bad_variant_access);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
