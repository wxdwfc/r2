#pragma once

#include "routine.hpp"

namespace r2 {

class RExecutor {
 public:
  static constexpr int kMaxCoroutineSupported = 65;

  RExecutor();
  ~RExecutor() = default;

  /**
   * spawn a new coroutine to the chain
   * T must be a subclass of RExecutor
   */
  template <class T>
  int spawn(const std::function<void (handler_t &yield,T &)> &func) {

    int cid = routines_.size();
    ASSERT(cid < kMaxCoroutineSupported) << "max " << kMaxCoroutineSupported << " supported";

    auto wrapper = std::shared_ptr<internal_routine_t>(
        new internal_routine_t,
        [](auto p) {
          delete p;
        });

    *wrapper = std::bind(func,std::placeholders::_1, std::ref(*(static_cast<T *>(this))));

    routines_.emplace_back(cid,wrapper);

    auto cur = chain_.append(&(routines_[cid]));
    if(cur_routine_ == nullptr)
      cur_routine_ = cur;

    return cid;
  }

  /**
   * start executing the routines, given the registered index
   */
  void run(int idx = 0);

  /**
   * add a pre-spawned coroutine back to the list
   */
  inline void add(int cor_id) {
    chain_.append(&(routines_[cor_id]));
  }

  /**
   * for debug only, print current chain, start from cur_routine_
   */
  void print_all() const;

  /***************************************/
  /** Applied to invidivual routine
   *  These are called within the spawned functions
   */
  /**
   * exit the current coroutine
   * like return in the function call
   */
  void exit(handler_t &yield);

  /**
   * return the current in-execute coroutine id
   */
  inline int cur_id() const {
    return cur_routine_->id_;
  }

  inline int next_id() const {
    return cur_routine_->next_routine_->id_;
  }

  /**
   * paused the current routine, and then yield
   */
  inline void pause_and_yield(handler_t &yield) {
    /**
     * FIXME: what if there is no routines ?
     * Then there will be a problem, since the routines should exit!
     */
    cur_routine_ = cur_routine_->leave(chain_);
    cur_routine_->execute(yield);
  }

  /**
   * yield to next routine
   */
  inline void yield_to_next(handler_t &yield) {

    auto temp    = cur_routine_;
    cur_routine_ = cur_routine_->next_routine_;

    if(likely(temp != cur_routine_))
      cur_routine_->execute(yield);
  }

  /**
   * allow graceful return, otherwise, the coroutine can still execute,
   * e.g. in a while(true) {
   *              coro.exit(yield);  // not really exit!
   *           }
   * should call: routine_ret(yield,coro); // not execute anymore
   */
#define routine_ret(yield,coro) {      \
  coro.exit(yield); return;            \
}

 protected:
  /**
   * FIXME: may have problem when the capacity is increased
   * + max coroutine supported ?
   */
  std::vector<RoutineLink::Routine>   routines_;
  RoutineLink                         chain_;
  RoutineLink::Routine               *cur_routine_ = nullptr;

  DISABLE_COPY_AND_ASSIGN(RExecutor);
};

};
