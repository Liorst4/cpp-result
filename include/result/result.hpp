#include <variant>

// TODO: Lint
// TODO: Format
// TODO: Tests

// TODO: Automatically resolve T
template <typename T> struct Ok {
	T val;
};

// TODO: Automatically resolve T
template <typename T> struct Err {
	T val;
};


template <typename T, typename E> class Result {
public:
  Result(Ok<T>&& value): m_v(value) {};
  Result(Err<E>&& error) : m_v(error) {};
  Result(const Result &) = default;
  Result(Result &&) = default;
  ~Result() = default;

  auto operator=(Result &&) -> Result & = default;
  auto operator=(const Result &) -> Result & = default;

  auto is_ok() const -> bool { return std::holds_alternative<Ok<T>>(this->m_v); };
  auto is_err() const -> bool { return !this->is_ok(); };

  // TODO: auto ok() const -> std::optional<T>;
  // TODO: auto err() const -> std::optional<T>;

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

  // TODO: unwrap
  // TODO: unwrap_err
  // TODO: unwrap_or_default

private:
  std::variant<Ok<T>, Err<E>> m_v;
};
