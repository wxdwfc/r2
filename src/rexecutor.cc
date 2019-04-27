#include "rexecutor.hpp"

#include <memory>

namespace r2 {

/**
 * In-default, ensure at least one coroutine is in the chain
 */
RExecutor::RExecutor() {
  /**
   * reserve some space for coroutines
   * we expect tens of coroutines in total
   */
  routines_.reserve(kMaxCoroutineSupported);
}

void RExecutor::run(int idx) {
  assert(routines_.size() > idx);
  cur_routine_ = &(routines_[idx]);
  routines_[idx].func_();
}

void RExecutor::exit(handler_t &yield) {
  auto temp = cur_routine_;
  cur_routine_ = cur_routine_->leave(chain_);
  if(cur_routine_ != temp) {
    cur_routine_->execute(yield);
  }
}

void RExecutor::print_all() const {
  auto cur = cur_routine_;
  /**
   * +1 means, that to check whether the chain has linked back, like A->B->A
   * this is important, since the chain are scheduele in a round-robin way
   */
  for(uint i = 0;i < routines_.size() + 1;++i) {
    LOG(3) << "" << cur->id_ << " -> ";
    cur = cur->next_routine_;
  }
  LOG(3) << "\n";
}

}  // namespace r2
