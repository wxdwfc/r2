#pragma once

#include "ud_msg.hpp"
#include "ud_data.hpp"

namespace r2 {

class UDIncomingIter : public IncomingIter {
 public:
  UDIncomingIter(UdAdapter *adapter) :
      adapter(adapter),
      poll_result(ibv_poll_cq(adapter->qp_->recv_cq_,MAX_UD_RECV_SIZE,adapter->receiver_.wcs_)) {
  }

  IncomingMsg next() override {
    Addr addr; addr.from_u32(adapter->receiver_.wcs_[idx_].imm_data);
    return {
      .msg  = (char *)(adapter->receiver_.wcs_[idx_].wr_id + GRH_SIZE),
      .size = MAX_UD_RECV_SIZE,
      .from = addr
    };
  }

  bool        has_next() {
    return idx_ < poll_result;
  }

 private:
  UdAdapter *adapter = nullptr;
  uint idx_          = 0;
  uint poll_result   = 0;
};

} // end namespace r2
