#pragma once

// R2 coroutine is a wrapper over boost asymmetric coroutine
#include <boost/coroutine/all.hpp>

#include <functional>
#include <memory>

#include "rlib/core/result.hh"

#include "./common.hh"
#include "./linked_list.hh"

namespace r2 {

using namespace rdmaio;

using yield_f = boost::coroutines::symmetric_coroutine<void>::yield_type;
using b_coroutine_f = boost::coroutines::symmetric_coroutine<void>::call_type;

/*!
  Exposed to user, a routine function is just void -> void
 */
using routine_func_t = std::function<void(yield_f &)>;

class Routine {
  using id_t = u8;

  std::shared_ptr<routine_func_t> raw_f;
  b_coroutine_f core;

public:
  Result<> status;
  const id_t id;

  Routine(const id_t &id, std::shared_ptr<routine_func_t> f)
    : id(id), raw_f(f), core(*f), status(::rdmaio::Ok()) {}

  inline void execute(yield_f &yield) {
    yield(core);
  }

  void start() { core(); }
};
} // namespace r2
