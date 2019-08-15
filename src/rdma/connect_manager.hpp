#pragma once

#include "../src/common.hpp"
#include "../src/timer.hpp"
#include "rlib/rdma_ctrl.hpp"

namespace r2 {

namespace rdma {
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
  // connect mac id
  const MacID remote_id;

  /*!
      on failure, we sleep pause_usec usecs.
   */
  const double pause_usec = 1000;

  double timeout = Timer::no_timeout; // 1000us

  using Result_mr_t = std::tuple<IOStatus, RemoteMemory::Attr>;

  explicit SyncCM(const MacID& id)
    : remote_id(id)
  {}

  SyncCM(const MacID& id, const double& p)
    : pause_usec(p)
    , remote_id(id)
  {}

  SyncCM& set_timeout(double t)
  {
    timeout = t;
    return *this;
  }

  auto get_mr(u64 mr_id, double timeout = Timer::no_timeout) -> Result_mr_t
  {
    this->set_timeout(timeout);
    return execute<RemoteMemory::Attr, u64>(
      mr_id, [](const u64& mr, const MacID& id) {
        RemoteMemory::Attr attr;
        return std::make_pair(RMemoryFactory::fetch_remote_mr(mr, id, attr),
                              attr);
      });
  }

  IOStatus connect_for_rc(RCQP* qp,
                          u64 remote_qp_id,
                          const QPConfig& config,
                          double timeout = Timer::no_timeout)
  {
    this->set_timeout(timeout);
    auto ret =
      execute<QPAttr, u64>(remote_qp_id, [](const u64& qp_id, const MacID& id) {
        QPAttr attr;
        auto res = QPFactory::fetch_qp_addr(QPFactory::RC, qp_id, id, attr);
        return std::make_pair(res, attr);
      });
    if (std::get<0>(ret) == SUCC) {
      return qp->connect(std::get<1>(ret), config);
    }
    return std::get<0>(ret);
  }

  template<typename RET_T, typename INPUT_T>
  std::tuple<IOStatus, RET_T> execute(
    const INPUT_T& input,
    std::function<std::tuple<IOStatus, RET_T>(const INPUT_T&, const MacID& m)>
      f)
  {
    Timer t;
    usize paused = 0;
    do {
      auto ret = f(input, remote_id);
      auto& res = std::get<0>(ret);
      // add some nop
      if (!(res == ERR || res == SUCC || res == WRONG_ARG)) {
        usleep(pause_usec);
        paused += 1;
      } else {
        return ret;
      }
      if (t.passed_msec() > (timeout + paused * pause_usec)) {
        res = TIMEOUT; // ok because "res" reference to "ret"
        return ret;
      }
    } while (true);
    // the function should never return here
  }
};

} // namespace rdma

} // namespace r2
