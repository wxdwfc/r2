#pragma once

#include "rlib/rc.hpp"
#include "../scheduler.hpp"

namespace r2
{
namespace rdma
{
using namespace rdmaio;

/*!
 SROP states for single RDMA one-sided OP.
 example usage:  // read 3 bytes at remote machine with address 0xc using one-sided RDMA.
      ::r2::rdma::SROp op(qp);
      op.set_payload(ptr,3).set_remote_addr(0xc).set_op(IBV_WR_RDMA_READ);
      auto ret = op.execute(IBV_SEND_SIGNALED,R2_ASYNC_WAIT);
      ASSERT(std::get<0>(ret) == SUCC);
 */
class SROp
{
public:
  explicit SROp(RCQP *qp) : qp(qp)
  {
  }

  inline SROp &set_payload(char *ptr, int size)
  {
    local_ptr = ptr;
    this->size = size;
    return *this;
  }

  inline SROp &set_op(const ibv_wr_opcode &op)
  {
    this->op = op;
    return *this;
  }

  inline SROp &set_remote_addr(const u64 &ra)
  {
    remote_addr = ra;
    return *this;
  }

  using Result_t = std::tuple<IOStatus, struct ibv_wc>;

  inline Result_t execute(R2_ASYNC)
  {
    return execute(IBV_SEND_SIGNALED, R2_ASYNC_WAIT);
  }

  inline Result_t execute(int flags, R2_ASYNC)
  {
    ibv_wc wc;
    auto res = qp->send({.op = op,
                         .flags = flags,
                         .len = size,
                         .wr_id = R2_COR_ID()},
                        {.local_buf = local_ptr,
                         .remote_addr = remote_addr,
                         .imm_data = 0});
    if (unlikely(res != SUCC))
      return std::make_pair(res, wc);
    // spawn a future
    if (flags & IBV_SEND_SIGNALED)
    {
      auto poll_future = [this, &res, &wc](std::vector<int> &routine_count,
                                           int &cor_id) {
        if ((cor_id = qp->poll_one_comp(wc)) == 0)
          return rdmaio::NOT_READY;
        if (likely(wc.status == IBV_WC_SUCCESS))
        {
          ASSERT(routine_count.size() > cor_id)
              << "get a wrong corid: " << cor_id;
          ASSERT(routine_count[cor_id] == 1);
          //  we decrease the routine counter here
          return rdmaio::SUCC;
        }
        else
        {
          LOG(4) << "poll till completion error: " << wc.status << " "
                 << ibv_wc_status_str(wc.status);
          // TODO： we need to filter out timeout events
          res = ERR;
          return SUCC;
        }
      };
      // if the request is signaled, we emplace a poll future
      R2_EXECUTOR.emplace(R2_COR_ID(), 1, poll_future);
      R2_PAUSE_AND_YIELD;
    }

    return std::make_pair(res, wc);
  }

private:
  RCQP *qp = nullptr;
  ibv_wr_opcode op;

  u64 remote_addr;

  char *local_ptr = nullptr;
  int size = 0;
};
} // namespace rdma

} // namespace r2
