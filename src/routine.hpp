#pragma once

#include "logging.hpp"

#include <vector>
#include <functional>

#include <boost/coroutine/all.hpp>

namespace r2 {

typedef boost::coroutines::symmetric_coroutine<void>::yield_type      handler_t;
typedef boost::coroutines::symmetric_coroutine<void>::call_type       coroutine_func_t;
typedef std::function<void (handler_t &yield)>                        internal_routine_t;

class RoutineLink {
 public:
  RoutineLink()  = default;
  ~RoutineLink() = default;

  class Routine {
   public:
    Routine(int id,std::shared_ptr<internal_routine_t> &f) :
        id_(id), func_(*f),
        unwrapperd_fuc_(f) {
    }

    inline Routine *leave(RoutineLink &c) {
      active_ = false;
      auto next  = next_routine_;
      ASSERT(prev_routine_ != nullptr);
      prev_routine_->next_routine_  = next;
      next_routine_->prev_routine_  = prev_routine_;
      if(c.tailer_ == this)
        c.tailer_ = prev_routine_;
      return next_routine_;
    }

    /**
     * execute this the routine callable function
     */
    inline void execute(handler_t &yield) {
      active_ = true;
      yield(func_);
    }

    coroutine_func_t    func_;
    Routine            *prev_routine_ = nullptr;
    Routine            *next_routine_ = nullptr;
    bool                active_ = false;
    const int           id_;
    std::shared_ptr<internal_routine_t> unwrapperd_fuc_;
  }; // end class Routine

  inline Routine *append(Routine *r) {
    auto prev = tailer_;
    if(unlikely(header_ == nullptr)) {
      // this routine is the header
      tailer_ = (header_ = r);
      prev = tailer_;
    }
    ASSERT(tailer_ != nullptr) ;
    tailer_->next_routine_ = r;

    // this routine is the new tailer
    tailer_ = r;

    // my position
    r->prev_routine_ = prev;
    r->next_routine_ = header_;
    return r;
  }
 private:
  Routine *header_ = nullptr;
  Routine *tailer_ = nullptr;

  DISABLE_COPY_AND_ASSIGN(RoutineLink);
};

} // namesapce r2
