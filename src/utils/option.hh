#pragma once

#if defined(__GNUC__) && __GNUC__ < 7
#include <experimental/optional>
#define optional experimental::optional
#else
#include <optional>
#endif

namespace r2 {
using Option = std::optional<T>;
} // namespace r2
