#pragma once

#include "rlib/rdma_ctrl.hpp"
#include "../src/timer.hpp"

namespace r2
{

namespace rdma
{
using namespace rdmaio;

/*!
    ConnectManager helps establish connection from remote machines.
    example:
    ::r2::rdma::SyncCM cm(::rdmaio::make_id("localhost",8888));
    auto mr = cm.get_mr(12);
    auto res = cm.connect_for_qp(qp,id);
    ASSERT(res == SUCC);
*/
class SyncCM
{
public:
    const MacID remote_id;

    using Result_mr_t = std::tuple<IOStatus, RemoteMemory::Attr>;

    explicit SyncCM(const MacID &id) : remote_id(id) {}

    Result_mr_t get_mr(u64 mr_id, double timeout = Timer::no_timeout)
    {
        RemoteMemory::Attr mr;
        IOStatus res = SUCC;
        Timer t;
        do
        {
            auto res = RMemoryFactory::fetch_remote_mr(
                mr_id, remote_id, mr);
            if (t.passed_msec() > timeout)
            {
                res = TIMEOUT;
                break;
            }
            // TODO: add some nop to prevent keep connecting
        } while (!(res == ERR || res == SUCC));

        return std::make_pair(res, mr);
    }

    IOStatus connect_for_rc(RCQP *qp, u64 remote_qp_id,
                            const QPConfig &config,
                            double timeout = Timer::no_timeout)
    {
        IOStatus res = SUCC;
        QPAttr attr;
        Timer t;
        do
        {
            res = QPFactory::fetch_qp_addr(
                QPFactory::RC, remote_qp_id, remote_id, attr);
            if (t.passed_msec() > timeout)
            {
                res = TIMEOUT;
                break;
            }
        } while (!(res == SUCC || res == NOT_READY));

        if (res == SUCC)
        {
            res = qp->connect(attr, config);
        }

        return res;
    }
};
} // namespace rdma
} // namespace r2