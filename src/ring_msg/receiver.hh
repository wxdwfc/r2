#pragma once

#include <unordered_map>

#include <string>

#include "./session.hh"

namespace r2 {

namespace ring_msg {

using namespace rdmaio;
using namespace rdmaio::qp;

/*!
  A ring receiver is responsible for receiving message from *n* QPs using a
  shared recv_cq
 */
template <usize R, usize kRingSz, usize kMaxMsg> class Receiver {
  const std::string name;
  // main structure for handling requests
  ibv_cq *recv_cq = nullptr;
  std::vector<ibv_wc> wcs;

  std::unordered_map<id_t, Arc<::r2::ring_msg::Session<R, kRingSz, kMaxMsg>>>
      incoming_sessions;

public:
  Receiver(const std::string &n, ibv_cq *cq)
      : name(n), recv_cq(cq), wcs(cq->cqe) {
    // the overall recv_cq depth should be larger than per QP depth (R)
    ASSERT(cq->cqe >= R);
  }

  inline ibv_wc *get_wcs_ptr() { return &wcs[0]; }

  bool reg_channel(Arc<::r2::ring_msg::Session<R, kRingSz, kMaxMsg>> msg) {
    if (incoming_sessions.find(msg->id) != incoming_sessions.end())
      return false;
    incoming_sessions.insert(std::make_pair(msg->id, msg));
    return true;
  }
};

} // namespace ring_msg

} // namespace r2
