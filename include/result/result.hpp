#ifndef CPP_RESULT_HPP
#define CPP_RESULT_HPP

#include <exception>
#include <optional>
#include <variant>

struct ok_tag {};
struct err_tag {};

#define Ok(x)                                                                  \
  ok_tag{}, { x }
#define Err(x)                                                                 \
  err_tag{}, { x }

template <typename T, typename E> class Result {
public:
  struct ok_t {
    T val;
  };
  struct err_t {
    E val;
  };

  Result(ok_tag t, ok_t &&value) : m_v(value){};
  Result(err_tag e, err_t &&error) : m_v(error){};
  Result(const Result &) = default;
  Result(Result &&) = default;
  ~Result() = default;

  auto operator=(Result &&) -> Result & = default;
  auto operator=(const Result &) -> Result & = default;

  auto is_ok() const -> bool {
    return std::holds_alternative<ok_t>(this->m_v);
  };
  auto is_err() const -> bool { return !this->is_ok(); };

  auto ok() const -> std::optional<T> {
    if (!this->is_ok()) {
      return std::nullopt;
    }
    return std::make_optional(std::get<ok_t>(this->m_v).val);
  };
  auto err() const -> std::optional<E> {
    if (!this->is_err()) {
      return std::nullopt;
    }
    return std::make_optional(std::get<err_t>(this->m_v).val);
  };

  // TODO: auto as_ref() const -> Result<const T&, const E&>;
  // TODO: auto as_mut() -> Result<T&, E&>;

  // TODO: map
  // TODO: map_or
  // TODO: map_or_else
  // TODO: map_err

  // TODO: iter ?
  // TODO: iter_mut ?

  // TODO: and
  // TODO: and_then

  // TODO: or
  // TODO: or_else

  // TODO: unwrap_or
  // TODO: unwrap_or_else

  // TODO: expect
  // TODO: expect_err

  auto unwrap() const -> const T & { return std::get<ok_t>(this->m_v).val; };
  // TODO: auto unwrap() -> T &&;

  auto unwrap_err() const -> const E & {
    return std::get<err_t>(this->m_v).val;
  };
  // TODO: auto unwrap_err() -> E&&
  // TODO: unwrap_or_default

private:
  std::variant<ok_t, err_t> m_v;
};

#endif /* CPP_RESULT_HPP */
