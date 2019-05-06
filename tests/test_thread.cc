#include "../src/thread.hpp"

#include <gtest/gtest.h>

using namespace r2;

namespace test {

class TestThread : public Thread<double> {
 public:
  TestThread() {

  }

  void *run_body() override {
    res = 73;
  }
};

TEST(ThreadTest, Simple) {
  TestThread t;
  t.start();
  auto res = t.join();
  ASSERT_EQ(res,73);
}

} // end namespace test
