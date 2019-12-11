#pragma once

#include <array>

#include "./manager.hh"

namespace r2 {

namespace ring_msg {

/*!
  Example usage of ring message.
  At server:
  `
  RCtrl ctrl(...);

  // the two manager are used to create rdma send/recv verbs
  // we additionally uses one manager for ring message
  Arc<RingManager> rm = RingManager::create(ctrl, manager);

  auto recv_cq = ...;
  Arc<AbsRecvAllocator> alloc = ...;
  manager.reg_recv_cqs.create_then_reg(std::to_string(thread_id), recv_cq,
                                       alloc);


  std::unordered_map<u16, RingSession> incoming_sessions;
  // after server receive the first message, using the recv_cq
#if 0
  for (RecvIter<RC, 4096> iter(recv_cq, wcs); iter.has_msgs();
    iter.next()) {
    auto imm_msg = iter.cur_msg().value();
    auto session_id = decode_s_id(std::get<0>(imm_msg));

    if (incoming_sessions.find(session_id) == incoming_sessions.end()){

    }

  }
#endif
  `

  At client:
  `
  const usize ring_sz = 1024;
  const usize max_ring_sz = 64;
  RingSession<128> rs(qp,1024,64);
  ASSERT(rs.connect(cm) == IOCode::Ok);

  // we need a bootstrap message, or whatever
  `
 */
template <usize max_recv_sz>
class RingReceiver {

  // main structure for handling requests
  ibv_cq *recv_cq = nullptr;
  std::array<ibv_wc,max_recv_sz> wcs;

  std::unordered_map<u16, Arc<RingSession>> incoming_sessions;

  // alias structure for coordinating with client requests
  RCtrl *ctrl = nullptr;
};

template <usize max_recv_sz> class RingRecvIter {};
}
} // namespace r2
