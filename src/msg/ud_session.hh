#pragma once

#include "rlib/core/qps/ud.hh"

#include "./msg_session.hh"

namespace r2 {

using namespace rdmaio::qp;

class UDSession : public Session {

  UD *ud = nullptr; // unsafe for performance reason

  ibv_ah *ah = nullptr;
  u64 qkey;
  u64 qpn;

  ibv_send_wr wr;
  ibv_sge sge;

  const int send_depth = 0;

  UDSession(const u32 &id, Arc<UD> &ud, const QPAttr &remote_attr)
      : ah(ud->create_ah(remote_attr)), qkey(remote_attr.qkey),
        qpn(remote_attr.qpn), ud(ud.get()),
        send_depth(ud->my_config.max_send_sz() / 2) {
    // init setup of wr
    wr.opcode = IBV_WR_SEND_WITH_IMM;
    wr.num_sge = 1;
    wr.next = nullptr;
    wr.sg_list = &sge;
    wr.imm_data = id;

    wr.wr.ud.ah = ah;
    wr.wr.ud.remote_qpn = qpn;
    wr.wr.ud.remote_qkey = qkey;
  }

  void setup_sge(const MemBlock &msg) {
    sge.addr = (uintptr_t)(msg.mem_ptr);
    sge.length = msg.sz;
  }

public:
  static ::r2::Option<Arc<UDSession>> create(const u32 &id, Arc<UD> &ud, const QPAttr &remote_attr) {
    auto res = Arc<UDSession>(new UDSession(id, ud, remote_attr));
    if (res->ah)
      return res;
    return {};
  }

  u32 my_id() const {
    return wr.imm_data;
  }

  Result<std::string>
  send_blocking(const MemBlock &msg,
                const double timeout = no_timeout) override {
    auto ret_s = send_pending(msg);
    if (unlikely(ret_s != IOCode::Ok))
      return ret_s;

    auto ret = ud->wait_one_comp(timeout);
    if (unlikely(ret != IOCode::Ok)) {
      return ::rdmaio::transfer(ret, UD::wc_status(ret.desc));
    }
    return ::rdmaio::Ok(std::string(""));
  }

  Result<std::string> send(const MemBlock &msg, const double timeout,
                           R2_ASYNC) override {
    return ::rdmaio::Err(std::string("not implemented"));
  }

  Result<std::string> send_pending(const MemBlock &msg) override {
    setup_sge(msg);
    wr.send_flags =
        IBV_SEND_SIGNALED |
      ((msg.sz <= ::rdmaio::qp::kMaxInlinSz) ? IBV_SEND_INLINE : 0);

    struct ibv_send_wr *bad_sr = nullptr;
    auto rc = ibv_post_send(ud->qp, &wr, &bad_sr);
    if (unlikely(rc != 0)) {
      return ::rdmaio::Err(std::string(strerror(errno)));
    }
    return ::rdmaio::Ok(std::string(""));
  }

  int unsigned_reqs = 0;

  /*!
    This call should not mix with all the other calls
   */
  Result<std::string> send_unsignaled(const MemBlock &msg) {
    setup_sge(msg);
    wr.send_flags =
        (unsigned_reqs == 0 ? IBV_SEND_SIGNALED : 0) |
        ((msg.sz <= ::rdmaio::qp::kMaxInlinSz) ? IBV_SEND_INLINE : 0);

    if (unsigned_reqs > send_depth) {
      auto ret = ud->wait_one_comp(1000000);
      if (unlikely(ret != IOCode::Ok)) {
        return ::rdmaio::transfer(ret, UD::wc_status(ret.desc));
      }
      unsigned_reqs = 0;
    } else
      unsigned_reqs += 1;

    struct ibv_send_wr *bad_sr = nullptr;
    auto rc = ibv_post_send(ud->qp, &wr, &bad_sr);
    if (unlikely(rc != 0)) {
      return ::rdmaio::Err(std::string(strerror(errno)));
    }
    return ::rdmaio::Ok(std::string(""));
  }

  Result<std::string> send_pending(const MemBlock &msg, R2_ASYNC) override {
    return ::rdmaio::Err(std::string("not implemented"));
  }
};

} // namespace r2
