#pragma once

namespace r2
{

#define R2_ASYNC handler_t &yield, RScheduler &_r

#define R2_PLACEHOLDER yield, _r

#define R2_YIELD _r.yield_to_next(yield);

#define R2_COR_ID() _r.cur_id()

#define R2_SPAWN(f) _r.spawnr((r));

#define R2_RET routine_ret(yield, _r);

#define R2_STOP() _r.stop_schedule();

} // namespace r2