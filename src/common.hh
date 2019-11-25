#pragma once

#include <cinttypes>

#include "option.hh"

namespace r2 {

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef int64_t i64;
typedef uint8_t u8;
typedef int8_t i8;
typedef unsigned int usize;

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

static inline unsigned long read_tsc(void) {
  unsigned a, d;
  __asm __volatile("rdtsc" : "=a"(a), "=d"(d));
  return ((unsigned long)a) | (((unsigned long)d) << 32);
}

class RDTSC {
public:
  RDTSC() : start(read_tsc()) {}
  unsigned long passed() const { return read_tsc() - start; }

private:
  unsigned long start;
};

static inline void compile_fence(void) { asm volatile("" ::: "memory"); }

} // end namespace r2
