#include "../src/ring_msg/ring.hh"

#include <gtest/gtest.h>

using namespace r2::ring_msg;

namespace test {

TEST(Ring, Basic) {
  Ring ring({
    .mem_ptr = nullptr;
    .sz = 1024
  });

  ASSERT_TRUE(ring.cur_msg(64));
  ASSERT_FALSE(ring.cur_msg(1024));
}

} // namespace test
