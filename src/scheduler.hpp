#pragma once

#include "rexecutor.hpp"
#include "futures/future.hpp"

#include "rlib/common.hpp"

#include <vector>
#include <deque>

namespace r2 {

class RScheduler : public RExecutor<rdmaio::IOStatus> {
 public:
  typedef std::function<rdmaio::IOStatus (std::vector<int> &,int &)> poll_func_t;
  typedef std::deque<poll_func_t> Futures;
  typedef std::function<void (handler_t &yield,RScheduler &r)> routine_t;

  RScheduler();

  int spawnr(const routine_t &f);

  void emplace(Future<rdmaio::IOStatus> &f) {
    poll_futures_.push_back([&f](std::vector<int> &routine_count,int &cor_id) {
                              rdmaio::IOStatus res = f.poll(routine_count);
                              cor_id   = f.cor_id;
                              return res;
                            });
  }

  void emplace(int corid,int num,poll_func_t f) {
    pending_futures_[corid] += num;
    poll_futures_.push_back(f);
  }

  /**
   * Stop & resume scheduling coroutines
   */
  void stop_schedule();

 private:
  std::vector<int>          pending_futures_;
  Futures                   poll_futures_;

  void poll_all();

  bool                      running_ = true;

  DISABLE_COPY_AND_ASSIGN(RScheduler);
}; // end class

} // end namespace rdmaio
