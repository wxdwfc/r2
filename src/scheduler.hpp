#pragma once

#include "rexecutor.hpp"
#include "futures/future.hpp"

#include "rlib/common.hpp"

#include <vector>
#include <deque>

namespace r2 {

class RScheduler : public RExecutor {
 public:
  typedef std::function<rdmaio::IOStatus (std::vector<int> &,int &)> poll_func_t;
  typedef std::deque<poll_func_t> Futures;
  typedef std::function<void (handler_t &yield,RScheduler &r)> routine_t;

  RScheduler();

  int spawnr(const routine_t &f);

  void map(Future<rdmaio::IOStatus> &f) {
    poll_futures_.push_back([&f](const std::vector<int> &routine_count,int &cor_id) {
                              rdmaio::IOStatus res = f.poll(routine_count);
                              cor_id   = f.cor_id;
                              return res;
                            });
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
}; // end class

} // end namespace rdmaio
