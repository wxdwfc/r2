#pragma once

#include "../msg/protocol.hpp"

namespace r2 {

namespace rpc {

enum MsgType {
  REQ = 0,  // RPC request
  REPLY     // RPC reply
};

class REQ {
 public:
  struct Meta {
    uint8_t cor_id;
    Addr    dest;
  };

  struct Arg {
    const char *buf       = nullptr;
    int         len       = 0;
    int         reply_cnt = 0;
    char       *reply_buf = nullptr;
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

} // end namespace rpc

} // end namespace r2
