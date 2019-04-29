#include <gtest/gtest.h>

#include <thread>

#include "../src/futures/rpc_future.hpp"

using namespace r2;
using namespace rdmaio;
using namespace std;

namespace test {

TEST(RpcTest, Future) {

  int cor_id = 73;
  std::vector<int> temp(128,1);

  RpcFuture future(cor_id,1000);

  this_thread::sleep_for(chrono::microseconds(500));
  ASSERT_EQ(future.poll(temp), NOT_READY); // this time, the timeout has not reached

  temp[cor_id] = 0;
  ASSERT_EQ(future.poll(temp), SUCC); // this time, the timeout has not reached

  temp[cor_id] = 73;
  this_thread::sleep_for(chrono::microseconds(800));
  ASSERT_EQ(future.poll(temp), TIMEOUT); // this time, the timeout has not reached
}

}; // end namespace test
