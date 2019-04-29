#include <gtest/gtest.h>

#include <thread>

#include "../src/rpc_future.hpp"

using namespace r2;
using namespace std;

namespace test {

TEST(RpcTest, Future) {

  int cor_id = 73;
  RpcFuture future(cor_id,1000);

  this_thread::sleep_for(chrono::microseconds(500));
  ASSERT_EQ(future.poll(), -1);

  this_thread::sleep_for(chrono::microseconds(700));
  ASSERT_EQ(future.poll(),73);
  ASSERT_EQ(future.res,::rdmaio::TIMEOUT);
}

}; // end namespace test
