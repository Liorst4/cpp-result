#ifndef CPP_RESULT_HPP
#define CPP_RESULT_HPP

#include <exception>
#include <functional>
#include <iostream>
#include <optional>
#include <string_view>
#include <variant>

#ifdef THROW_ON_PANIC
using ResultException = std::logic_error;
#endif // THROW_ON_PANIC

struct ok_tag {};
struct err_tag {};

#define Ok(x)                                                                  \
  ok_tag{}, { x }
#define Err(x)                                                                 \
  err_tag{}, { x }

template <typename T, typename E> class Result {
public:
  // Wrapping T and E with ok_t and err_t because `std::variant` doesn't support
  // multiple instances of the same type.
  struct ok_t {
    T val;
  };
  struct err_t {
    E val;
  };

  Result(ok_tag /*t*/, ok_t &&value) : m_v(std::move(value)){};
  Result(err_tag /*e*/, err_t &&error) : m_v(std::move(error)){};
  Result(const Result &) = default;
  Result(Result &&) noexcept = default;
  ~Result() = default;

  auto operator=(Result &&) noexcept -> Result & = default;
  auto operator=(const Result &) -> Result & = default;

  [[nodiscard]] auto equals(const Result<T, E> &other) const -> bool {
    const auto index = this->m_v.index();
    if (index != other.m_v.index()) {
      return false;
    }
    if (0 == index) {
      return std::get<0>(this->m_v).val == std::get<0>(other.m_v).val;
    }
    return std::get<1>(this->m_v).val == std::get<1>(other.m_v).val;
  };

  [[nodiscard]] auto is_ok() const -> bool {
    return std::holds_alternative<ok_t>(this->m_v);
  };
  [[nodiscard]] auto is_err() const -> bool { return !this->is_ok(); };

  [[nodiscard]] auto ok() const -> std::optional<T> {
    if (!this->is_ok()) {
      return std::nullopt;
    }
    return std::make_optional(std::get<ok_t>(this->m_v).val);
  };
  [[nodiscard]] auto err() const -> std::optional<E> {
    if (!this->is_err()) {
      return std::nullopt;
    }
    return std::make_optional(std::get<err_t>(this->m_v).val);
  };

  [[nodiscard]] auto as_ref() const -> Result<const T &, const E &> {
    if (this->is_ok()) {
      const auto &wrapper_ref = std::get<ok_t>(this->m_v);
      const auto &wrapper_value_ref = wrapper_ref.val;
      return {Ok(wrapper_value_ref)};
    } else {
      const auto &wrapper_ref = std::get<err_t>(this->m_v);
      const auto &wrapper_value_ref = wrapper_ref.val;
      return {Err(wrapper_value_ref)};
    }
  }

  [[nodiscard]] auto as_mut() -> Result<T &, E &> {
    if (this->is_ok()) {
      auto &wrapper_ref = std::get<ok_t>(this->m_v);
      auto &wrapper_value_ref = wrapper_ref.val;
      return {Ok(wrapper_value_ref)};
    } else {
      auto &wrapper_ref = std::get<err_t>(this->m_v);
      auto &wrapper_value_ref = wrapper_ref.val;
      return {Err(wrapper_value_ref)};
    }
  }

  template <typename U, typename F = std::function<U(T)>>
  [[nodiscard]] auto map(const F &op) const -> Result<U, E> {
    if (!this->is_ok()) {
      return {Err(this->unwrap_err())};
    }
    return {Ok(std::invoke(op, this->unwrap()))};
  }

  template <typename U, typename F = std::function<U(T)>>
  [[nodiscard]] auto map_or(const U &default_value, const F &op) const -> U {
    if (!this->is_ok()) {
      return default_value;
    }
    return std::invoke(op, this->unwrap());
  }

  template <typename U, typename D = std::function<U(E)>,
            typename F = std::function<U(T)>>
  [[nodiscard]] auto map_or_else(const D &on_err, const F &on_ok) const -> U {
    if (!this->is_ok()) {
      return std::invoke(on_err, this->unwrap_err());
    }
    return std::invoke(on_ok, this->unwrap());
  }

  template <typename F, typename O = std::function<F(E)>>
  [[nodiscard]] auto map_err(const O &op) const -> Result<T, F> {
    if (!this->is_ok()) {
      return {Err(std::invoke(op, this->unwrap_err()))};
    }
    return {Ok(this->unwrap())};
  }

  // Added underscore at the end because `and` is reserved.
  template <typename U>
  [[nodiscard]] auto and_(const Result<U, E> &other) const -> Result<U, E> {
    if (this->is_ok()) {
      return other;
    }
    return {Err(this->unwrap_err())};
  }

  template <typename U, typename F = std::function<Result<U, E>(T)>>
  [[nodiscard]] auto and_then(const F &op) const -> Result<U, E> {
    if (this->is_ok()) {
      return std::invoke(op, this->unwrap());
    }
    return {Err(this->unwrap_err())};
  }

  // Added underscore at the end because `or` is reserved.
  template <typename F>
  [[nodiscard]] auto or_(const Result<T, F> &res) const -> Result<T, F> {
    if (this->is_err()) {
      return res;
    }
    return {Ok(this->unwrap())};
  }

  template <typename F, typename O = std::function<Result<T, F>(E)>>
  [[nodiscard]] auto or_else(const O &op) const -> Result<T, F> {
    if (this->is_err()) {
      return std::invoke(op, this->unwrap_err());
    }
    return {Ok(this->unwrap())};
  }

  [[nodiscard]] auto expect(const std::string_view &msg) const -> T {
    if (!this->is_ok()) {
      panic(msg);
    }
    return this->unwrap();
  }
  [[nodiscard]] auto expect_err(const std::string_view &msg) const -> E {
    if (!this->is_err()) {
      panic(msg);
    }
    return this->unwrap_err();
  }

  [[nodiscard]] auto unwrap() const -> T {
    if (!this->is_ok()) {
      panic();
    }
    return std::get<ok_t>(this->m_v).val;
  };

  [[nodiscard]] auto unwrap_or(const T &default_value) const -> T {
    if (!this->is_ok()) {
      return default_value;
    }
    return this->unwrap();
  }

  [[nodiscard]] auto unwrap_or_default() const -> T {
    if (!this->is_ok()) {
      return T{};
    }
    return this->unwrap();
  }

  template <typename F = std::function<T(E)>>
  [[nodiscard]] auto unwrap_or_else(const F &op) const -> T {
    if (!this->is_ok()) {
      return std::invoke(op, this->unwrap_err());
    }
    return this->unwrap();
  }

  [[nodiscard]] auto unwrap_err() const -> E {
    if (!this->is_err()) {
      panic();
    }
    return std::get<err_t>(this->m_v).val;
  };

private:
  std::variant<ok_t, err_t> m_v;

  static void panic(const std::string_view &msg = "") {
#ifdef THROW_ON_PANIC
    throw ResultException{msg.data()};
#else  // THROW_ON_PANIC
    std::cerr << msg << std::endl;
    std::terminate();
#endif // THROW_ON_PANIC
  }
};

template <typename T, typename E>
constexpr auto operator==(const Result<T, E> &a, const Result<T, E> &b) {
  return a.equals(b);
}
template <typename T, typename E>
constexpr auto operator!=(const Result<T, E> &a, const Result<T, E> &b) {
  return !a.equals(b);
}

#endif /* CPP_RESULT_HPP */
