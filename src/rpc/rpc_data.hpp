#pragma once

#include "../msg/protocol.hpp"

namespace r2 {

namespace rpc {

enum MsgType {
  REQ = 0,  // RPC request
  REPLY     // RPC reply
};

/**
 * RPC request structure
 */
class Req {
 public:
  struct Meta {
    uint8_t cor_id;
    Addr    dest;
  };

  struct Arg {
    const char *send_buf  = nullptr;
    int         len       = 0;
    char       *reply_buf = nullptr;
    int         reply_cnt = 0;
  };

  // internal data structures used in RPC
  struct Header {
    uint32_t type : 2;
    uint32_t rpc_id :  5;
    uint32_t payload : 18;
    uint32_t cor_id     : 7;
  }  __attribute__ ((aligned (sizeof(uint64_t))));

 public:
  static int sizeof_header(void) {
    return sizeof(Header);
  }
};  // end class RPC data

/**
 * record the pending reply structures
 */
struct Reply {
  char *reply_buf   = nullptr;
  int   reply_count = 0;
};

} // end namespace rpc

} // end namespace r2
