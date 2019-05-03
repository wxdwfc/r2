#include "../common.hpp"
#include "rpc.hpp"

namespace r2 {

namespace rpc {

static const int kMaxRPCFuncSupport = 16;

RPC::RPC(std::shared_ptr<MsgProtocol> msg_handler):
    msg_handler_(msg_handler) {
  rpc_callbacks_.resize(kMaxRPCFuncSupport);
}

void RPC::register_callback(int rpc_id, rpc_func_t callback) {
  ASSERT(rpc_id >= 0 && rpc_id < kMaxRPCFuncSupport);
  rpc_callbacks_[rpc_id] = callback;
}

static inline REQ::Header make_rpc_header(int type,int id,int size,int cid) {
  return {
    .type    = type,
    .rpc_id  = id,
    .payload = size,
    .cor_id  = cid
  };
}

rdmaio::IOStatus RPC::call(const REQ::Meta &context, int rpc_id, const REQ::Arg &arg) {
  auto ret = call_async(context, rpc_i, arg);
  if(likely(ret == SUCC))
    ret = flush_pending();
  return ret;
}

rdmaio::IOStatus RPC::call_async(const REQ::Meta &context, int rpc_id, const REQ::Arg &arg) {
  REQ::Header *header = (REQ::Header *)(arg.buf - sizeof(REQ::Header));
  *header = make_rpc_header(REQ, rpc_id, arg.len,context.cor_id);
  return msg_handler_->send_async(context.dest, (char *)header,sizeof(REQ::Header) + arg.len);
}

rdmaio::IOStatus reply(const REQ::Meta &context, char *reply,int size) {
  auto ret = reply_async(context,reply,size);
  if(likely(ret == SUCC))
    ret = flush_pending();
  return ret;
}

rdmaio::IOStatus RPC::reply_async(const REQ::Meta &context, char *reply,int size) {
  REQ::Header *header = (REQ::Header *)(arg.buf - sizeof(REQ::Header));
  *header = make_rpc_header(REPLY, 0 /* a place holder*/,size,context.cor_id);
  return msg_handler_->send_async(context.dest, (char *)header,sizeof(REQ::Header) + size);
}

} // end namespace rpc

} // end namespace r2
