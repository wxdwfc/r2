#pragma once

#include "rexecutor.hpp"
#include "futures/future.hpp"

#include "rlib/common.hpp"

#include <vector>
#include <deque>

namespace r2
{

class RScheduler : public RExecutor<rdmaio::IOStatus>
{
public:
  typedef std::function<rdmaio::IOStatus(std::vector<int> &, int &)> poll_func_t;
  typedef std::deque<poll_func_t> Futures;
  typedef std::function<void(handler_t &yield, RScheduler &r)> routine_t;

  RScheduler();

  explicit RScheduler(const routine_t &f);

  int spawnr(const routine_t &f);

  void emplace(Future<rdmaio::IOStatus> &f)
  {
    poll_futures_.push_back([&f](std::vector<int> &routine_count, int &cor_id) {
      rdmaio::IOStatus res = f.poll(routine_count);
      cor_id = f.cor_id;
      return res;
    });
  }

  void emplace(int corid, int num, poll_func_t f)
  {
    pending_futures_[corid] += num;
    poll_futures_.push_back(f);
  }

  /*!
    Pause with a timeout
    \param timeout: timeout in meconds
  */
  inline rdmaio::IOStatus pause_to(handler_t &yield, double timeout)
  {
    Timer t;
    while (true)
    {
      yield_to_next(yield);
      if (pending_futures_[cur_id()] == 0)
        return rdmaio::SUCC;
      if (t.passed_msec() > timeout)
      {
        return rdmaio::TIMEOUT;
      }
    }
  }

  void pause(handler_t &yield)
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

  bool is_running() const
  {
    return running_;
  }

  std::vector<int> pending_futures_;

private:
  // TODO, XD:
  // Shall we leave the futures per coroutine, instead poll all futures per schedule?
  Futures poll_futures_;

  bool running_ = true;

  DISABLE_COPY_AND_ASSIGN(RScheduler);
}; // end class

#include "scheduler_marocs.hpp"

} // namespace r2
