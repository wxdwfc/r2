#pragma once

#include "rlib/core/bootstrap/proto.hh"

namespace r2 {

namespace ring_msg {

using namespace rdmaio;
using namespace rdmaio::qp;
using namespace rdmaio::proto;
using namespace rdmaio::rmem;

struct __attribute__((packed)) RingReply {
  RCReply rc_reply;
  u64 base_off;
  RegAttr mr;
};

} // namespace ring_msg

} // namespace r2
