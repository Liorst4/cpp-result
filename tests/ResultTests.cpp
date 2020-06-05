#include <memory>

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

GTEST_TEST(resultTest, move) {
  {
    auto success = Result<std::string, int>{Ok("Hello world")};
    std::string s = std::move(success.unwrap());
    ASSERT_EQ(s, "Hello world");
    ASSERT_NE(success.unwrap(), "Hello world");
    ASSERT_EQ(success.unwrap(), "");
  }

  {
    auto failure = Result<int, std::string>{Err("Hello world")};
    std::string s = std::move(failure.unwrap_err());
    ASSERT_EQ(s, "Hello world");
    ASSERT_NE(failure.unwrap_err(), "Hello world");
    ASSERT_EQ(failure.unwrap_err(), "");
  }

  {
    auto x = Result<std::unique_ptr<int>, int>{Ok(std::make_unique<int>(5))};
    ASSERT_EQ(5, *x.unwrap());
    std::unique_ptr<int> y = std::move(x.unwrap());
    ASSERT_EQ(5, *y);
    ASSERT_EQ(nullptr, x.unwrap());
 }
}

GTEST_TEST(resultTest, comparison) {
  const auto x = Result<int, int>{Ok(5)};
  ASSERT_EQ(x, x);
  const auto y = Result<int, int>{Err(5)};
  ASSERT_NE(x, y);
  const auto z = Result<int, int>{Ok(6)};
  ASSERT_NE(x, z);
  ASSERT_NE(z, y);
  ASSERT_EQ(z, z);
  const auto k = Result<int, int>{Err(6)};
  ASSERT_NE(y, k);
}

GTEST_TEST(resultTest, and_) {
  const auto r = Result<int, int>{Err(7)};
  const auto l = Result<int, int>{Ok(7)};
  const auto h = Result<int, int>{Err(8)};
  ASSERT_EQ(r.and_(l), r);
  ASSERT_EQ(l.and_(r), r);
  ASSERT_NE(r.and_(h), h);
}

GTEST_TEST(resultTest, and_then) {
  const auto e = Result<int, int>{Err(7)};
  const auto o = Result<int, int>{Ok(6)};

  const auto x =
      e.and_then<int>([](const int &) -> Result<int, int> { return {Ok(5)}; });
  ASSERT_TRUE(x.is_err());

  const auto y =
      o.and_then<int>([](const int &) -> Result<int, int> { return {Ok(5)}; });
  ASSERT_TRUE(y.is_ok());
  ASSERT_EQ(5, y.unwrap());
}

// TODO: Test or_
// TODO: Test or_else
// TODO: Test TRY_ASSIGNMENT.
// TODO: Test unwrap_or
// TODO: Test unwrap_or_else
// TODO: Test unwrap_or_default

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
