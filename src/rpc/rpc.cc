#include "../common.hpp"
#include "rpc.hpp"

using namespace rdmaio;

namespace r2 {

namespace rpc {

static const int kMaxRPCFuncSupport = 16;

RPC::RPC(std::shared_ptr<MsgProtocol> msg_handler):
    msg_handler_(msg_handler) {
  rpc_callbacks_.resize(kMaxRPCFuncSupport);
  replies_.resize(RExecutor<int>::kMaxCoroutineSupported);
}

void RPC::register_callback(int rpc_id, rpc_func_t callback) {
  ASSERT(rpc_id >= 0 && rpc_id < kMaxRPCFuncSupport);
  rpc_callbacks_[rpc_id] = callback;
}

static inline Req::Header make_rpc_header(int type,int id,int size,int cid) {
  return {
    .type    = type,
    .rpc_id  = id,
    .payload = size,
    .cor_id  = cid
  };
}

IOStatus RPC::call(const Req::Meta &context, int rpc_id, const Req::Arg &arg) {
  auto ret = call_async(context, rpc_id, arg);
  if(likely(ret == SUCC))
    ret = flush_pending();
  return ret;
}

IOStatus RPC::call_async(const Req::Meta &context, int rpc_id, const Req::Arg &arg) {
  Req::Header *header = (Req::Header *)(arg.buf - sizeof(Req::Header));
  *header = make_rpc_header(REQ, rpc_id, arg.len,context.cor_id);
  return msg_handler_->send_async(context.dest, (char *)header,sizeof(Req::Header) + arg.len);
}

IOStatus RPC::reply(const Req::Meta &context, char *reply,int size) {
  auto ret = reply_async(context,reply,size);
  if(likely(ret == SUCC))
    ret = flush_pending();
  return ret;
}

IOStatus RPC::reply_async(const Req::Meta &context, char *reply,int size) {
  Req::Header *header = (Req::Header *)(reply - sizeof(Req::Header));
  *header = make_rpc_header(REPLY, 0 /* a place holder*/,size,context.cor_id);
  return msg_handler_->send_async(context.dest, (char *)header,sizeof(Req::Header) + size);
}

void RPC::spawn_recv(RScheduler &s) {
  auto  msg_ptr = this->msg_handler_;
  auto &replies = replies_;
  s.emplace(0,0,[&s,msg_ptr,&replies](std::vector<int> &routine_count,int &cor_id){
                  msg_ptr->poll_all([&s,&routine_count,&replies]
                                    (const char *msg,int size,const Addr &addr) {
                                      // first we parse the header
                                      Req::Header *header = (Req::Header *)msg;
                                      const char *args = msg + sizeof(Req::Header);
                                      switch (header->type) {
                                        case REQ:
                                          break;
                                        case REPLY:
                                          break;
                                        default:
                                          ASSERT(false);
                                      }
                                    });
                  return NOT_READY; // this function should never return
                }
    );
}

} // end namespace rpc

} // end namespace r2
