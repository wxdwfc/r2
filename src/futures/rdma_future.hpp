#pragma once

#include "rlib/rc.hpp"

#include "future.hpp"

namespace r2 {

/**
 * RdmaFuture does not track timeout.
 * This is because the ibv_qp track the timeout
 */
class RdmaFuture : public Future<rdmaio::IOStatus> {
  using namespace rdmaio;
 public:
  RdmaFuture(int cor_id,RCQP *qp) :
      Future(cor_id),
      qp(qp) {
  }

  rdmaio::IOStatus poll(const std::vector<int> &routine_count) override {
    ibv_wc wc;
    int    cor_id;
    if((cor_id = qp->poll_one_comp(wc)) > 0) {
      ASSERT(routine_count.size() > cor_id);

      //  we decrease the routine counter here
      routine_count[cor_id] = 0;
      if(routine_count[cor_id] == 0)
        return SUCC;
      return EJECT;
    } else if (cor_id == 0)
      return NOT_READY;
    else {
      // TODO: fix error cases
      return ERR;
    }
  }

 private:
  RCQP *qp;
}; // end class

} // end namespace r2
