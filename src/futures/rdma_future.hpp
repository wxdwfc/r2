#pragma once

#include "rlib/rc.hpp"

#include "../scheduler.hpp"

namespace r2 {

/**
 * RdmaFuture does not track timeout.
 * This is because the ibv_qp track the timeout
 */
class RdmaFuture : public Future<rdmaio::IOStatus> {
 public:
  RdmaFuture(int cor_id,rdmaio::RCQP *qp) :
      Future(cor_id),
      qp(qp) {
  }

  rdmaio::IOStatus poll(std::vector<int> &routine_count) override {
    ibv_wc wc;
    int    cor_id;
    if((cor_id = qp->poll_one_comp(wc)) > 0) {
      ASSERT(routine_count.size() > cor_id);

      //  we decrease the routine counter here
      routine_count[cor_id] -= 1;
      if(routine_count[cor_id] == 0)
        return rdmaio::SUCC;
      return rdmaio::EJECT;
    } else if (cor_id == 0)
      return rdmaio::NOT_READY;
    else {
      // TODO: fix error cases
      return rdmaio::ERR;
    }
  }

  static void spawn_future(RScheduler &s,rdmaio::RCQP *qp,int num = 1) {
    if(likely(num > 0))
      s.emplace(s.cur_id(),num,[qp](std::vector<int> &routine_count,int &cor_id) {
                                 ibv_wc wc;

                                 if((cor_id = qp->poll_one_comp(wc)) > 0) {
                                   ASSERT(routine_count.size() > cor_id);
                                   ASSERT(routine_count[cor_id] > 0);
                                   //  we decrease the routine counter here
                                   routine_count[cor_id] -= 1;
                                 } else if (cor_id == 0)
                                   return rdmaio::NOT_READY;
                                 else {
                                   // TODO: fix error cases
                                   return rdmaio::ERR;
                                 }

                                 if(routine_count[cor_id] == 0)
                                   return rdmaio::SUCC;
                                 return rdmaio::NOT_READY;
                               });
  }

  static rdmaio::IOStatus send_wrapper(RScheduler &s,rdmaio::RCQP *qp,int cor_id,
                                       const rdmaio::RCQP::ReqMeta &meta,
                                       const rdmaio::RCQP::ReqContent &req) {

    auto res = qp->send(meta,req);

    if(likely(res == rdmaio::SUCC))
      spawn_future(s,qp,1);
    return res;
  }

 private:
  rdmaio::RCQP *qp;
}; // end class

} // end namespace r2
