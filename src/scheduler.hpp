#pragma once

#include "futures/future.hpp"
#include "rexecutor.hpp"

#include "rlib/common.hpp"

#include <deque>
#include <vector>

namespace r2
{

class RScheduler : public RExecutor
{
public:
  using poll_result_t = std::pair<rdmaio::IOStatus, int>;
  using poll_func_t = std::function<poll_result_t(std::vector<int> &)>;
  using Futures = std::deque<poll_func_t>;
  using routine_t = std::function<void(handler_t &yield, RScheduler &r)>;
  // TODO, XD:
  // Shall we leave the futures per coroutine, instead poll all futures per
  // schedule?
private:
  Futures poll_futures_;

  bool running_ = true;

public:
  RScheduler();

  explicit RScheduler(const routine_t &f);

  int spawnr(const routine_t &f);

  void emplace(Future<rdmaio::IOStatus> &f)
  {
    ASSERT(false) << " legacy API, should not be used any more.";
  }

  void emplace(int corid, int num, poll_func_t f)
  {
    pending_futures_[corid] += num;
    poll_futures_.push_back(f);
  }


  IOStatus pause(handler_t &yield)
  {
    if (pending_futures_[cur_id()] > 0)
      pause_and_yield(yield);
  }

  /**
   * Stop & resume scheduling coroutines
   */
  void stop_schedule();

  /*!
   * Poll all the registered futures.
   */
  void poll_all();

  bool is_running() const { return running_; }

  std::vector<int> pending_futures_;

  DISABLE_COPY_AND_ASSIGN(RScheduler);
}; // end class

} // namespace r2

#include "scheduler_marocs.hpp"
