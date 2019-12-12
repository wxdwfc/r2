#pragma once

#include "./session.hh"
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

  // 128: the registered entry at each QP
  //Arc<RingManager<128>> rm = RingManager<128>::create(ctrl);
  RingManager<128> rm(ctrl);

  auto recv_cq = ...;
  Arc<AbsRecvAllocator> alloc = ...;

  Arc<Receiver<128>> ring_receiver = rm.create("my_ring_receiver", recv_cq,alloc).value();

  // after server receive the first message, using the recv_cq
  `

  At client:
  `
  const usize ring_sz = 1024;
  const usize max_ring_sz = 64;
  RingSession<128> rs(qp,1024,64);

  RingCM cm(some_addr); // use a specificed ring cm
  ASSERT(rs.connect(cm) == IOCode::Ok);

  // we need a bootstrap message, or whatever
  `
 */

//template <usize max_recv_sz> class RingRecvIter {};


} // namespace ring_msg
} // namespace r2
