#include "scheduler.hpp"

namespace r2 {

static constexpr int scheduler_cid = 0;

RScheduler::RScheduler(const routine_t &f) :
    RExecutor(),
    pending_futures_(1,0) {
  spawnr(f);
}

RScheduler::RScheduler() :
    RScheduler([](handler_t &yield,RScheduler &coro) {

                 while(coro.running_) {
                   // poll the completion events
                   coro.poll_all();

                   if(coro.next_id() != coro.cur_id()) {
                     coro.yield_to_next(yield);
                   } else {
                     // pass
                   }
                 }
                 routine_ret(yield,coro);
               }) {
}

int RScheduler::spawnr(const RScheduler::routine_t &func) {
  auto ret = RExecutor::spawn<RScheduler>(func);
  pending_futures_.push_back(0);
  return ret;
}

void RScheduler::stop_schedule() {
  // To ask, why poll_futures_.size() == 1?
  // if(running_ && poll_futures_.size() == 1) {
  if(running_) {
    //ASSERT(cur_routine_ != &(routines_[scheduler_cid]));
    routines_[scheduler_cid].leave(chain_);
    running_ = false;
  }
}

using namespace rdmaio;

void RScheduler::poll_all() {

  for(auto it = poll_futures_.begin(); it != poll_futures_.end();) {
    int cor_id = -1;
    switch((*it)(pending_futures_,cor_id)) {
      case SUCC:
        add(cor_id);                  // add the coroutine back to the scheduler
        //status_[cor_id] = SUCC;
      case EJECT:
        it = poll_futures_.erase(it); // eject this future from the list
        break;
      case ERROR:
        // TODO, not implement error case
        break;
      case NOT_READY:
        // pass, do nothing
        it++;
        break;
      default:
        ASSERT(false) << "poll an invalid future return value";
    }
  } // end iterate all pending futures
} // end poll_all

} // end namespace r2
