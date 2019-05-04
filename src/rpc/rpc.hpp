#pragma once

#include "../common.hpp"
#include "../scheduler.hpp"
#include "rpc_data.hpp"

namespace r2 {

namespace rpc {

/** The RPC callback takes 6 parameters:
 *  1. Rpc
 *  2. RPC context (sid,tid,cid)
 *  3. a pointer to the msg
 *  4. an extra(typically not used argument)
 */
class RPC {
  typedef std::function<void(RPC &,const Req::Meta &ctx,const char *,void *)> rpc_func_t;
 public:
  explicit RPC(std::shared_ptr<MsgProtocol> msg_handler);

  void register_callback(int rpc_id, rpc_func_t callback);

  /**
   * Call and reply methods.
   * Meta-data reserved for each message.
   * It includes a header, and some implementation specific padding
   */
  rdmaio::IOStatus call(const Req::Meta &context, int rpc_id, const Req::Arg &arg);

  rdmaio::IOStatus reply(const Req::Meta &context, char *reply,int size);

  rdmaio::IOStatus call_async(const Req::Meta &context, int rpc_id, const Req::Arg &arg);

  rdmaio::IOStatus reply_async(const Req::Meta &context, char *reply,int size);

  inline rdmaio::IOStatus flush_pending() {
    return msg_handler_->flush_pending();
  }

  /**
   * Return a future, so that the scheduler can poll it for receiving messages
   */
  void spawn_recv(RScheduler &s);

 private:
  std::shared_ptr<MsgProtocol>        msg_handler_;
  std::vector<rpc_func_t>             rpc_callbacks_;
  const int                           padding_ = 0;

  // replies
  std::vector<Reply>                  replies_;
  DISABLE_COPY_AND_ASSIGN(RPC);
}; // end class RPC

} // end namespace rpc

} // end namespace r2
