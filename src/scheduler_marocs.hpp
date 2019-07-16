#pragma once

/*!
    Several macros to easily manage coroutines like the ones in many modern
   language.
   Exposed function:
    - Define a function to be called in a coroutine (R2 coroutine function):
      T fn(...args, R2_ASYNC);
    - Call an R2 coroutine function in an R2 coroutine function:
        fn(...args,R2_ASYNC_WAIT);
    - Yield this coroutine:
        ... // some piece of code
        YIELD;
        ... // code after yield
    - Get the current coroutine id:
        auto id = R2_COR_ID();
    - Coroutine ret.
        R2_RET;
 */
namespace r2 {

#define R2_EXECUTOR _r

#define R2_ASYNC handler_t &yield, RScheduler &_r

#define R2_PLACEHOLDER yield, _r

#define R2_ASYNC_WAIT R2_PLACEHOLDER

#define R2_YIELD _r.yield_to_next(yield);

#define R2_PAUSE_AND_YIELD _r.pause(yield);

#define R2_COR_ID() _r.cur_id()

#define R2_SPAWN(f) _r.spawnr((r));

#define R2_RET routine_ret(yield, _r);

#define R2_STOP() _r.stop_schedule();

} // namespace r2