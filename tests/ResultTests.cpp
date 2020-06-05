#include <memory>

#include <result/result.hpp>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

SCENARIO("OK", "[result]") {
  GIVEN("Result with Ok value of 5") {
    const auto r = Result<int, int>{Ok(5)};
    WHEN("Checking that its ok") {
      THEN("Its ok") { REQUIRE(r.is_ok()); }
    }
    WHEN("Checking that its an error") {
      THEN("Its not an error") { REQUIRE_FALSE(r.is_err()); }
    }
    WHEN("Taking its ok value") {
      const auto maybe = r.ok();
      THEN("It has that value") { REQUIRE(maybe.has_value()); }
    }
    WHEN("Taking its error value") {
      const auto maybe = r.err();
      THEN("It has no value") { REQUIRE_FALSE(maybe.has_value()); }
    }
    WHEN("Unwraping its ok value") {
      THEN("It can be accsessed") {
        int value = 0;
        REQUIRE_NOTHROW(value = r.unwrap());
        REQUIRE(value == 5);
      }
    }
    WHEN("Unwraping its error value") {
      THEN("It can't be accessed") {
        int value = 0;
        REQUIRE_THROWS(value = r.unwrap_err());
      }
    }
    WHEN("Calling unwrap_or") {
      THEN("Its the same as calling unwarp") {
        REQUIRE(r.unwrap_or(6) == r.unwrap());
      }
    }
    WHEN("Calling unwrap_or_default") {
      THEN("Its the same as calling unwarp") {
        REQUIRE(r.unwrap_or_default() == r.unwrap());
      }
    }
    WHEN("Calling and_then") {
      THEN("The given function is called") {
        REQUIRE(r.and_then<int>([](const int &) -> Result<int, int> {
                   return {Ok(3)};
                 }).unwrap() == 3);
      }
    }
    WHEN("Calling or_else") {
      THEN("The given function is not called") {
        REQUIRE(r.or_else<int>([](const int &) -> Result<int, int> {
                   return {Ok(3)};
                 }).unwrap() == 5);
      }
    }
    WHEN("Calling unwrap_or_else") {
      THEN("The given function is not called") {
        REQUIRE(r.unwrap_or_else(([](const int &) -> int { return 3; })) == 5);
      }
    }
  }
  GIVEN("A move only ok value") {
    auto success =
        Result<std::unique_ptr<int>, int>{Ok(std::make_unique<int>(5))};
    REQUIRE(5 == *success.unwrap());
    WHEN("Its moved") {
      std::unique_ptr<int> y = std::move(success.unwrap());
      THEN("The value moved") {
        REQUIRE(5 == *y);
        REQUIRE(nullptr == success.unwrap());
      }
    }
  }
}

SCENARIO("ERR", "[result]") {
  GIVEN("Result with Err value of 5") {
    const auto r = Result<int, int>{Err(5)};
    WHEN("Checking that its ok") {
      THEN("Its not ok") { REQUIRE_FALSE(r.is_ok()); }
    }
    WHEN("Checking that its an error") {
      THEN("Its an error") { REQUIRE(r.is_err()); }
    }
    WHEN("Taking its ok value") {
      const auto maybe = r.ok();
      THEN("It has no value") { REQUIRE_FALSE(maybe.has_value()); }
    }
    WHEN("Taking its error value") {
      const auto maybe = r.err();
      THEN("It has that value") { REQUIRE(maybe.has_value()); }
    }
    WHEN("Unwraping its ok value") {
      THEN("It can't be accessed") {
        int value = 0;
        REQUIRE_THROWS(value = r.unwrap());
      }
    }
    WHEN("Unwraping its error value") {
      THEN("It can be accsessed") {
        int value = 0;
        REQUIRE_NOTHROW(value = r.unwrap_err());
        REQUIRE(value == 5);
      }
    }
    WHEN("Calling unwrap_or") {
      THEN("The default value is returned") { REQUIRE(r.unwrap_or(6) == 6); }
    }
    WHEN("Calling unwrap_or_default") {
      THEN("The default value is returned") {
        REQUIRE(r.unwrap_or_default() == int{});
      }
    }
    WHEN("Calling and_then") {
      THEN("The given function is not called") {
        REQUIRE(r.and_then<int>([](const int &) -> Result<int, int> {
                   return {Ok(3)};
                 }).unwrap_err() == 5);
      }
    }
    WHEN("Calling or_else") {
      THEN("The given function is called") {
        REQUIRE(r.or_else<int>([](const int &) -> Result<int, int> {
                   return {Ok(3)};
                 }).unwrap() == 3);
      }
    }
    WHEN("Calling unwrap_or_else") {
      THEN("The given function is called") {
        REQUIRE(r.unwrap_or_else(([](const int &) -> int { return 3; })) == 3);
      }
    }
  }
  GIVEN("A move only err value") {
    auto failure =
        Result<int, std::unique_ptr<int>>{Err(std::make_unique<int>(5))};
    REQUIRE(5 == *failure.unwrap_err());
    WHEN("Its moved") {
      std::unique_ptr<int> y = std::move(failure.unwrap_err());
      THEN("The value moved") {
        REQUIRE(5 == *y);
        REQUIRE(nullptr == failure.unwrap_err());
      }
    }
  }
}

SCENARIO("Interaction", "[result]") {
  GIVEN("Two oks with the same values") {
    const auto a = Result<int, int>{Ok(5)};
    const auto b = Result<int, int>{Ok(5)};
    WHEN("Compared") {
      THEN("They are equal") { REQUIRE(a == b); }
      THEN("They are not unequal") { REQUIRE_FALSE(a != b); }
    }
  }
  GIVEN("Two oks with different values") {
    const auto a = Result<int, int>{Ok(5)};
    const auto b = Result<int, int>{Ok(6)};
    WHEN("Compared") {
      THEN("They are not equal") { REQUIRE_FALSE(a == b); }
      THEN("They are unequal") { REQUIRE(a != b); }
    }
    REQUIRE(a.and_(b) == b);
    REQUIRE(a.or_(b) == a);
    REQUIRE(b.and_(a) == a);
    REQUIRE(b.or_(a) == b);
  }
  GIVEN("Two errs with the same values") {
    const auto a = Result<int, int>{Err(5)};
    const auto b = Result<int, int>{Err(5)};
    WHEN("Compared") {
      THEN("They are equal") { REQUIRE(a == b); }
      THEN("They are not unequal") { REQUIRE_FALSE(a != b); }
    }
  }
  GIVEN("Two errs with different values") {
    const auto a = Result<int, int>{Err(5)};
    const auto b = Result<int, int>{Err(6)};
    WHEN("Compared") {
      THEN("They are not equal") { REQUIRE_FALSE(a == b); }
      THEN("They are unequal") { REQUIRE(a != b); }
    }
    REQUIRE(a.and_(b) == a);
    REQUIRE(a.or_(b) == b);
    REQUIRE(b.and_(a) == b);
    REQUIRE(b.or_(a) == a);
  }
  GIVEN("Ok and Err with same values") {
    const auto a = Result<int, int>{Ok(5)};
    const auto b = Result<int, int>{Err(5)};
    WHEN("Compared") {
      THEN("They are not equal") { REQUIRE_FALSE(a == b); }
      THEN("They are unequal") { REQUIRE(a != b); }
    }
    REQUIRE(a.and_(b) == b);
    REQUIRE(a.or_(b) == a);
    REQUIRE(b.and_(a) == b);
    REQUIRE(b.or_(a) == a);
  }
  GIVEN("Ok and Err with different values") {
    const auto a = Result<int, int>{Ok(5)};
    const auto b = Result<int, int>{Err(6)};
    WHEN("Compared") {
      THEN("They are not equal") { REQUIRE_FALSE(a == b); }
      THEN("They are unequal") { REQUIRE(a != b); }
    }
    REQUIRE(a.and_(b) == b);
    REQUIRE(a.or_(b) == a);
    REQUIRE(b.and_(a) == b);
    REQUIRE(b.or_(a) == a);
  }
}

// TODO: Test TRY_ASSIGNMENT.
