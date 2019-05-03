#pragma once

#include <functional>

#include "rlib/common.hpp"
#include "../common.hpp"

namespace r2 {

struct Addr {
  uint64_t mac_id : 16;
  uint64_t thread_id  : 16;

  inline uint64_t to_u32() const {
    return *((uint32_t *)this);
  }

  inline void from_u32(uint32_t res) {
    *((uint32_t *)(this)) = res;
  }
};
typedef uint32_t Addr_id_t;

/**
 * The message can have multiple implementations.
 * So we use a virtual class to identify which functions must be implemented
 * for a msg protocol.
 */
class MsgProtocol {
 public:
  /**
   * connect to the remote end point
   * opt: an optional field which can be used by different message implementations
   */
  virtual rdmaio::IOStatus connect(const Addr &addr,const rdmaio::MacID &id,int opt = 0) = 0;

  virtual rdmaio::IOStatus send_async(const Addr &addr,const char *msg,int size) = 0;

  virtual rdmaio::IOStatus flush_pending() = 0;

  virtual int padding() const {
    return 0;
  }

  // send a message to a destination
  rdmaio::IOStatus send(const Addr &addr, const char *msg,int size) {
    auto ret = send_async(addr, msg, size);
    if(likely(ret == rdmaio::SUCC))
      ret = flush_pending();
    return ret;
  }

  // poll all in-coming msgs
  typedef std::function<void(const char *,int size,const Addr &addr)> msg_callback_t;
  virtual int poll_all(const msg_callback_t &f) = 0;
};

} // end namespace r2
