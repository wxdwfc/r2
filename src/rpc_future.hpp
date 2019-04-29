#pragma once

#include "future.hpp"
#include "timer.hpp"

#include "rlib/common.hpp"

namespace r2 {

class RpcFuture : public Future<rdmaio::IOStatus> {
 public:
  RpcFuture(int cor_id,double timeout) :
      Future(cor_id),
      timeout(timeout) {
  }

  int poll() override {
    if(timer_.passed_msec() >= timeout) {
      res = rdmaio::TIMEOUT;
      return cor_id;
    }
    else
      return -1;
  }

 private:
  Timer timer_;
  double timeout;
}; // end class

} // end namespace r2
