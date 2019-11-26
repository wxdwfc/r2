#pragma once

#include <deque>
#include <vector>

#include "./routine.hh"

namespace r2 {

/*!
  A poll future should return: Result<(id, num)>
  if result == Ok:
  - then pending_futures[id] -= num
  else if result == Err:
  - then yield (id) back
 */
using poll_func_t = std::function<Result<std::pair<id_t, usize>>()>;
class SScheduler;
using sroutine_func_t = std::function<void(yield_f &yield, SScheduler &r)>;

/*!
  SScheduler is a **single-threaded** executor who execute R2 routines.
  An example usage is:
  `
    SScheduler s;
    s.spawnr([](R2_ASYNC) {
      R2_RET;
    });
    s.run();
  `
  This spawns a corutine which does nothing.

  In each coroutine, applications can execute multiple R2 calls.
  For example:
  `
  R2_RET; // coroutine should return with R2_RET key world. Otherwise, the
  behavior is undefined. R2_YIELD; // yield this coroutine to another;
  `
  For more keyword, please refer to libroutine.hh
*/

class SScheduler {

  /*!
    Each elemenent in routine_chain stores **pointers to** routines vector
    element
   */
  LinkedList<Routine *> routine_chain;
  std::vector<Routine> routines;
  Node<Routine *> *cur_routine_ptr = nullptr;

  std::vector<usize> pending_futures;

  /*!
    A set of futures fo scheduling
   */
  std::deque<poll_func_t> futures;

  volatile bool running = false;

public:
  SScheduler();

  /*!
    Spawn a coroutine to the SScheduer.
   */
  Option<id_t> spawn(const sroutine_func_t &f);

  /*!
    Empace a future for waiting coroutine (id)
    This means that the coroutine (id) should wait at least (num) requests.
    The request is monitored by the poll_func_t.
   */
  void emplace_for_routine(const id_t &id, usize num, poll_func_t);

  void run() {
    this->running = true;
    routines.at(0).start();
  }

  /**********************************************************************************************/

  /*! Note, the following functions must be called **inside** a specific coroutine, not outside */
  /*!
    Pause the current coroutine, and yield to others
   */
  Result<> pause_and_yield(yield_f &f) {
    cur_routine_ptr = routine_chain.leave_one(cur_routine_ptr);
    cur_routine_ptr->val->execute(f);
    return cur_routine_ptr->val->status;
  }

  Result<> pause(yield_f &f) {
    if (pending_futures[cur_id()] > 0)
      return pause_and_yield(f);
    return Ok();
  }

  void yield_to_next(yield_f &f) {
    auto temp = cur_routine_ptr;
    cur_routine_ptr = cur_routine_ptr->next_p;

    if(likely(temp != cur_routine_ptr))
      cur_routine_ptr->val->execute(f);
  }

  id_t cur_id() const {
    return cur_routine_ptr->val->id;
  }

  void exit(yield_f &f);

  void stop() { this->running = false; }
};
} // namespace r2