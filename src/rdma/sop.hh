#pragma once

#include "rlib/core/qps/doorbell_helper.hh"
#include "rlib/core/qps/rc.hh"

#include "../libroutine.hh"

namespace r2 {

namespace rdma {

using namespace rdmaio;
using namespace rdmaio::qp;

/*!
 SROP states for single RDMA one-sided OP.
 This is a simple wrapper over the RC API provided by the RLib,
 which itself is a wrapper of libibverbs.

 Example usage:  // read 3 bytes at remote machine with address 0xc using
 one-sided RDMA.
      Arc<RC> qp; // some pre-initialized QP
      ::r2::rdma::SROp op;
      op.set_payload(ptr,3).set_remote_addr(0xc).set_op(IBV_WR_RDMA_READ);
      auto ret = op.execute(qp,IBV_SEND_SIGNALED,R2_ASYNC_WAIT); // async
 version

      or
      auto ret = op.execute_sync(qp); // sync version
 */
struct SROp {
  ibv_wr_opcode op;
  u64 remote_addr;
  void *local_ptr = nullptr;
  u32 size = 0;
  int flags = 0;

  u64 imm_data = 0;

  double timeout_usec = 1000000;

public:
  SROp() = default;

  inline SROp &set_payload(void *ptr, const u32 &size) {
    this->local_ptr = ptr;
    this->size = size;
    return *this;
  }

  inline SROp &set_op(const ibv_wr_opcode &op) {
    this->op = op;
    return *this;
  }

  inline SROp &set_read() {
    this->set_op(IBV_WR_RDMA_READ);
    return *this;
  }

  inline SROp &set_write() {
    this->set_op(IBV_WR_RDMA_WRITE);
    return *this;
  }

  inline SROp &set_remote_addr(const u64 &ra) {
    this->remote_addr = ra;
    return *this;
  }

  inline SROp &set_imm(const int &imm) {
    this->imm_data = imm;
    return *this;
  }

  inline SROp &set_timeout_usec(const double &tu) {
    this->timeout_usec = tu;
    return *this;
  }

  inline auto execute_my(const Arc<RC> &qp, const int &flags,int wr_id = 0)
      -> Result<std::string> {

    // to avoid performance overhead of Arc, we first extract QP's raw pointer
    // out
    RC *qp_ptr = ({ // unsafe code
      RC *temp = qp.get();
      temp;
    });

    auto res = qp_ptr->send_normal(
        {.op = this->op, .flags = flags, .len = this->size, .wr_id = wr_id},
        {.local_addr = static_cast<rmem::RMem::raw_ptr_t>(this->local_ptr),
         .remote_addr = this->remote_addr,
         .imm_data = this->imm_data});
    if (unlikely(res != ::rdmaio::IOCode::Ok))
      return res;
    return ::rdmaio::Ok(std::string(""));
  }

  inline auto execute_sync(const Arc<RC> &qp, int flags,
                           const double &timeout_usec = 1000000)
      -> Result<ibv_wc> {
    ibv_wc wc;

    auto ret_s = execute_my(qp, flags);
    if (unlikely(ret_s != IOCode::Ok))
      return ::rdmaio::transfer(ret_s, wc);

    if (flags & IBV_SEND_SIGNALED) {
      auto ret = qp->wait_rc_comp(timeout_usec);
      return ::rdmaio::transfer(ret, std::get<1>(ret.desc));
    }
    return ::rdmaio::Ok(wc);
  }

  inline auto execute(const Arc<RC> &qp, int flags, R2_ASYNC)
      -> Result<ibv_wc> {
    ibv_wc wc;
    auto ret_s = execute_my(qp, flags,R2_COR_ID());
    if (unlikely(ret_s != IOCode::Ok))
      return ::rdmaio::transfer(ret_s, wc);

    if (flags & IBV_SEND_SIGNALED) {
      return wait_one(qp, R2_ASYNC_WAIT);
    }
    return ::rdmaio::Ok(wc);
  }

private:
  inline auto wait_one(const Arc<RC> &qp, R2_ASYNC) -> Result<ibv_wc> {

    // to avoid performance overhead of Arc, we first extract QP's raw pointer
    // out
    RC *qp_ptr = ({ // unsafe code
      RC *temp = qp.get();
      temp;
    });

    ibv_wc wc;
    auto id = R2_COR_ID();

    poll_func_t poll_future = [qp_ptr, &wc,
                               id]() -> Result<std::pair<id_t, usize>> {
      auto wr_wc = qp_ptr->poll_rc_comp();
      if (wr_wc) {
        id_t polled_cid = static_cast<id_t>(std::get<0>(wr_wc.value()));
        wc = std::get<1>(wr_wc.value());

        if (wc.status == IBV_WC_SUCCESS && qp_ptr->ongoing_signaled() == 0)
          return ::rdmaio::Ok(std::make_pair(polled_cid, 1u));
        else if (wc.status == IBV_WC_SUCCESS)
          // Ok, but the future must still in the queue
          return NearOk(std::make_pair(polled_cid, 1u));
        else
          return ::rdmaio::Err(std::make_pair(id, 1u));
      }
      // we still need to poll this future
      return NotReady(std::make_pair(0u, 0u));
    };

    // end spawning future
    if (qp_ptr->ongoing_signaled() == 1) {
      // 1 means that I am the only one to wait for this QP
      auto ret = R2_PAUSE_WAIT(poll_future, 1);
      return ::rdmaio::transfer(ret, wc);
    }
    auto ret = R2_WAIT_NUM(1);
    return ::rdmaio::transfer(ret, wc);
  }
};
} // namespace rdma

} // namespace r2
