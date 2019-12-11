#pragma once

#include "rlib/core/qps/rc.hh"
#include "rlib/core/qps/recv_helper.hh"

#include "../mem_block.hh"

namespace r2 {

namespace ring_msg {

// TODO
template <usize R> class RecvBundler {
  // structure for post_recvs
  Arc<RecvEntries<R>> recv_entries;
  usize idle_recv_entries = 0;

  // ring buffer for storing the message


  Result<int> consume_one(Arc<RC> &qp) {

    idle_recv_entries += 1;
    if (idle_recv_entries >= poll_recv_step) {
      auto ret = qp->post_recvs(*recv_entries, idle_recv_entries);
      idle_recv_entries = 0;
      return ret;
    }
    return ::rdmaio::Ok(0);
  }
};
}
} // namespace r2
