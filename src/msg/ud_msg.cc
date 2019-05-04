#include "rlib/rdma_ctrl.hpp"

#include "ud_msg.hpp"
#include "ud_iter.hpp"

using namespace rdmaio;

namespace r2 {

UdAdapter::UdAdapter(const Addr &my_addr,UDQP *qp) :
    my_addr(my_addr),
    qp_(qp),
    sender_(my_addr,qp->local_mem_.key),
    receiver_(qp_) {
}

static ibv_ah *create_ah(UDQP *qp,const QPAttr &attr);

IOStatus UdAdapter::connect(const Addr &addr,const rdmaio::MacID &id,int uid) {

  // check if we already connected
  if(connect_infos_.find(addr.to_u32()) != connect_infos_.end())
    return SUCC;

  QPAttr fetched_attr;
  auto ret = QPFactory::fetch_qp_addr(QPFactory::UD,uid,id,fetched_attr);
  if(ret == SUCC) {
    auto ah = create_ah(qp_,fetched_attr);
    if(ah == nullptr) return ERR;
    UdConnectInfo connect_info = {
      .address_handler = ah,
      .remote_qpn      = fetched_attr.qpn,
      .remote_qkey     = fetched_attr.qkey
    };
    connect_infos_.insert(std::make_pair(addr.to_u32(),connect_info));
    return SUCC;
  } else
    return ret;
}

IOStatus UdAdapter::send_async(const Addr &addr,const char *msg,int size) {

  auto &wr  = sender_.cur_wr();
  auto &sge = sender_.cur_sge();

  auto it = connect_infos_.find(addr.to_u32());
  if(unlikely(it == connect_infos_.end()))
    return NOT_CONNECT;

  const auto &link_info = it->second;
  wr.wr.ud.ah = link_info.address_handler;
  wr.wr.ud.remote_qpn  = link_info.remote_qpn;
  wr.wr.ud.remote_qkey = link_info.remote_qkey;

  wr.send_flags = (qp_->empty() ? IBV_SEND_SIGNALED : 0)
                  | ((size < ::rdmaio::MAX_INLINE_SIZE) ? IBV_SEND_INLINE : 0);

  if(qp_->need_poll()) {
    ibv_wc wc;auto ret = ::rdmaio::QPUtily::wait_completion(qp_->cq_,wc);
    ASSERT(ret == SUCC) << "poll UD completion reply error: " << ret;
    qp_->clear();
  } else {
    qp_->forward(1);
  }
  sge.addr   = (uintptr_t)msg;
  sge.length = size;

  // FIXME: we assume that the qp_ is appropriate created,
  // so that it's send queue is larger than MAX_UD_SEND_DOORBELL
  if((++(sender_.current_window_idx_))
     > std::min(MAX_UD_SEND_DOORBELL,qp_->max_send_size))
    flush_pending();
  return SUCC;
}

IOStatus UdAdapter::flush_pending() {
  return sender_.flush_pending(qp_);
}

int UdAdapter::poll_all(const MsgProtocol::msg_callback_t &f) {
  uint poll_result = ibv_poll_cq(qp_->recv_cq_,MAX_UD_RECV_SIZE,receiver_.wcs_);
  for(uint i = 0;i < poll_result;++i) {
    if(likely(receiver_.wcs_[i].status == IBV_WC_SUCCESS)) {
      Addr addr; addr.from_u32(receiver_.wcs_[i].imm_data);
      f((const char *)(receiver_.wcs_[i].wr_id + GRH_SIZE),MAX_UD_PACKET_SIZE,addr);
    } else {
      ASSERT(false)  << "error wc status " << receiver_.wcs_[i].status;
    }
  }
  flush_pending();
  receiver_.post_recvs(qp_,poll_result);
  return poll_result;
}

IncomingIter UdAdapter::get_iter() {
  return UDIncomingIter(this);
}

static ibv_ah *create_ah(UDQP *qp,const QPAttr &attr) {
  struct ibv_ah_attr ah_attr;
  ah_attr.is_global = 1;
  ah_attr.dlid = attr.lid;
  ah_attr.sl = 0;
  ah_attr.src_path_bits = 0;
  ah_attr.port_num = attr.port_id;

  ah_attr.grh.dgid.global.subnet_prefix = attr.addr.subnet_prefix;
  ah_attr.grh.dgid.global.interface_id = attr.addr.interface_id;
  ah_attr.grh.flow_label = 0;
  ah_attr.grh.hop_limit = 255;
  ah_attr.grh.sgid_index = qp->rnic_p->addr.local_id;
  return ibv_create_ah(qp->rnic_p->pd, &ah_attr);
}

} // end namespace r2
