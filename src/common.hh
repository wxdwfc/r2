#pragma once

#include <cinttypes>

#include "./utils/option.hh"
#include "./logging.hh"

namespace r2 {

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using i64 = int64_t;
using u8 = uint8_t;
using i8 = int8_t;
using usize = unsigned int;

constexpr usize kCacheLineSize = 128;

} // end namespace r2

#pragma once

namespace r2 {

#define DISABLE_COPY_AND_ASSIGN(classname)                                     \
private:                                                                       \
  classname(const classname &) = delete;                                       \
  classname &operator=(const classname &) = delete

#define unlikely(x) __builtin_expect(!!(x), 0)
#define likely(x) __builtin_expect(!!(x), 1)

#define NOT_INLE __attribute__((noinline))
#define ALWAYS_INLINE __attribute__((always_inline))

static inline void compile_fence(void) { asm volatile("" ::: "memory"); }

} // end namespace r2
