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
    u64 nic_id;
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
  static std::tuple<IOStatus, QPAttr> create_connect(const CCReq& req,
                                                     RdmaCtrl& ctrl,
                                                     std::vector<RNic*>& nics)
  {
    auto ret = std::make_pair(WRONG_ARG, QPAttr());
    // create and reconnect
    RemoteMemory::Attr dummy; // since we are creating QP for remote, no MR attr
    // is required for it
    if (req.nic_id >= nics.size()) {
      return ret;
    }
    auto& nic = *nics[static_cast<usize>(req.nic_id)];
    auto qp = new RCQP(nic, dummy, dummy, req.config);
    ASSERT(qp != nullptr);

    // try register the QP. since a QP cannot be re-connected, so
    // we return an error if there is an already registered QP.
    if (!ctrl.qp_factory.register_rc_qp(req.qp_id, qp)) {
      goto ERR_RET;
    }
    // we connect for the qp
    {
      auto code = qp->connect(req.attr, req.config);

      if (code != SUCC) {
        std::get<0>(ret) = code;
        goto ERR_RET;
      }
    }
    return std::make_pair(SUCC, qp->get_attr());
  ERR_RET:
    delete qp;
    return ret;
  }

  static bool register_cc_handler(RdmaCtrl& ctrl, std::vector<RNic*>& nics)
  {
    ctrl.register_handler(CreateConnect,
                          std::bind(ConnectHandlers::create_connect_wrapper,
                                    std::ref(ctrl),
                                    std::ref(nics),
                                    std::placeholders::_1));
  }

private:
  static Buf_t create_connect_wrapper(RdmaCtrl& ctrl,
                                      std::vector<RNic*>& nics,
                                      const Buf_t& req)
  {
    if (req.size() < sizeof(CCReq))
      return Marshal::null_reply();
    auto decoded_req = Marshal::deserialize<CCReq>(req);

    auto res = create_connect(decoded_req, ctrl, nics);

    CCReply reply;

    if (std::get<0>(res) == SUCC) {
      reply.res = SUCC;
      reply.attr = std::get<1>(res);
    } else {
      reply.res = std::get<0>(res);
    }

    return Marshal::serialize_to_buf(reply);
  }
};
} // namespace rdma

} // namespace r2