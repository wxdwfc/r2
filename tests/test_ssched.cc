#include <gtest/gtest.h>

#include "../src/libroutine.hh"

using namespace r2;

namespace test {

TEST(SSched, Basic) {

  usize counter = 0;
  SScheduler ssched;
  for (uint i = 0; i < 12; ++i)
    ssched.spawn([&counter](R2_ASYNC) {
      counter += 1;
      R2_RET;
    });
}

} // namespace test
