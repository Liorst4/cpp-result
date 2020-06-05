# C++ Result
Rust's `std::result::Result` re-implemented in c++ using `std::variant`.

## Example
```c++
#include <array>
#include <cerrno>
#include <iostream>
#include <result/result.hpp>
#include <unistd.h>

auto modern_read(int fd, void *buf, size_t count) -> Result<size_t, int> {
  const auto result = read(fd, buf, count);
  if (result < 0) {
    return {Err(errno)};
  }
  return {Ok(static_cast<size_t>(result))};
}

int main() {
  std::array<char, 100> buf{};
  const auto read_result = modern_read(STDIN_FILENO, buf.data(), sizeof(buf));
  if (read_result.is_ok()) {
    std::cout << read_result.unwrap() << std::endl;
  } else {
    std::cerr << read_result.unwrap_err() << std::endl;
  }
  return 0;
}
```
