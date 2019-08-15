#pragma once

#include "../common.hpp"
#include "rlib/rdma_ctrl.hpp"

#include <functional>

namespace r2 {

namespace rdma {

using namespace rdmaio;

enum
{
  CreateConnect = (::rdmaio::RESERVED_REQ_ID::FREE + 1),
};
/*!
    RDMAHandlers register callback to RdmaCtrl, so that:
    it handles QP creation requests;
    it handles QP deletion requests, etc.
 */
class ConnectHandlers
{
public:
  struct CCReq
  {
    u64 qp_id;
    QPAttr attr;
    QPConfig config;
  };

  struct CCReply
  {
    IOStatus res;
    QPAttr attr;
  };
  /*!
    This handler receive a CCReq, create a corresponding QP,
    register it with RdmaCtrl, and finally return the created QP's attr.
    If creation failes, then a {} is returned.
   */
  static Option<QPAttr> create_connect(const CCReq& req,
                                       RdmaCtrl& ctrl,
                                       RNic& nic)
  {
    // create and reconnect
    RemoteMemory::Attr dummy; // since we are creating QP for remote, no MR attr
                              // is required for it
    auto qp = new RCQP(nic, dummy, dummy, req.config);
    ASSERT(qp != nullptr);

    // try register the QP. since a QP cannot be re-connected, so
    // we return an error if there is an already registered QP.
    if (!ctrl.qp_factory.register_rc_qp(req.qp_id, qp)) {
      goto ERR_RET;
    }
    // we connect for the qp
    if (qp->connect(req.attr, req.config) != SUCC) {
      goto ERR_RET;
    }
    return Option<QPAttr>(qp->get_attr());
  ERR_RET:
    delete qp;
    return {};
  }

  static bool register_cc_handler(RdmaCtrl& ctrl, RNic& nic)
  {
    ctrl.register_handler(CreateConnect,
                          std::bind(ConnectHandlers::create_connect_wrapper,
                                    std::ref(ctrl),
                                    std::ref(nic),
                                    std::placeholders::_1));
  }

private:
  static Buf_t create_connect_wrapper(RdmaCtrl& ctrl,
                                      RNic& nic,
                                      const Buf_t& req)
  {
    return "";
  }
};
} // namespace rdma

} // namespace r2