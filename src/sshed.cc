#include "./libroutine.hh"

namespace r2 {

const usize kMaxRoutineSupported = 64;
static_assert(
    kMaxRoutineSupported + 1 <= std::numeric_limits<id_t>::max(),
    "invalid id_t type, consider make it from u8 to a larger unsigned type");

/*!
  The constructor will spawn a main coroutine for polling all the futures
 */
SScheduler::SScheduler() : pending_futures(kMaxRoutineSupported, 0) {

  this->spawn([](R2_ASYNC) {
    while (R2_EXECUTOR.running) {
      // TODO, the body is not implemented
      R2_YIELD;
    }
    R2_RET;
  });
}

Option<id_t> SScheduler::spawn(const sroutine_func_t &f) {
  auto cid = routines.size();
  if (cid >= kMaxRoutineSupported)
    return {};
  auto wrapper = std::shared_ptr<routine_func_t>(new routine_func_t,
                                                 [](auto p) { delete p; });
  *wrapper = std::bind(f, std::placeholders::_1, std::ref(*this));
  routines.emplace_back(cid, wrapper);

  { // unsafe code
    auto cur = routine_chain.add(new Node<Routine *>(&(routines[cid])));
    if (cur_routine_ptr == nullptr)
      cur_routine_ptr = cur;
  }
  return cid;
}

void SScheduler::exit(yield_f &f) {
  auto temp = cur_routine_ptr;
  cur_routine_ptr = routine_chain.leave_one(cur_routine_ptr);

  // there are still remaining coroutines
  if (cur_routine_ptr != temp) {
    cur_routine_ptr->val->execute(f);
  }
}

} // namespace r2
