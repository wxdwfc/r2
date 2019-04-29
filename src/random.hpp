#pragma once

#include <assert.h>

namespace r2 {

namespace util {

// taken from java:
// http://developer.classpath.org/doc/java/util/Random-source.html
class FastRandom {
 public:
  FastRandom(unsigned long seed)
      : seed(0) {
    set_seed0(seed);
  }

  FastRandom() : seed(0) {
    set_seed0(seed);
  }

  inline unsigned long next() {
    return ((unsigned long) next(32) << 32) + next(32);
  }

  inline uint32_t next_u32() {
    return next(32);
  }

  inline uint16_t next_u16() {
    return next(16);
  }

  /** [0.0, 1.0) */
  inline double next_uniform() {
    return (((unsigned long) next(26) << 27) + next(27)) / (double) (1L << 53);
  }

  inline char next_char()
  {
    return next(8) % 256;
  }

  inline std::string next_string(size_t len) {
    std::string s(len, 0);
    for (size_t i = 0; i < len; i++)
      s[i] = next_char();
    return s;
  }

  inline unsigned long get_seed() {
    return seed;
  }

  inline void set_seed(unsigned long seed) {
    this->seed = seed;
  }

  inline void set_seed0(unsigned long seed) {
    this->seed = (seed ^ 0x5DEECE66DL) & ((1L << 48) - 1);
  }

  inline uint64_t rand_number(int min,int max) {
    return check_between_inclusive( (uint64_t)(next_uniform() * (max - min + 1) + min),min,max);
  }

  inline uint64_t check_between_inclusive(uint64_t v,uint64_t min,uint64_t max) {
    assert(v >= min);
    assert(v <= max);
    return v;
  }

 private:
  inline unsigned long
  next(unsigned int bits)
  {
    seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
    return (unsigned long) (seed >> (48 - bits));
  }

  unsigned long seed;
};

}

} // end namespace r2
