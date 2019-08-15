#include "../../src/common.hpp"
#include "../../src/rdma/connect_handlers.hpp"

#include <gflags/gflags.h>

DEFINE_string(server_host, "localhost", "Server host used.");
DEFINE_int64(server_port, 8888, "Server port used.");
DEFINE_int64(qp_id, 73, "server's qp id. should be equal at server.");
DEFINE_int64(mr_id, 73, "server's mr id. should be equal at server.");

using namespace r2;
using namespace rdmaio;

/*!
  The server code of basic example.
  It just setup QPs for client to connect, and then sleep forever.
*/
int
main(int argc, char** argv)
{
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  // a local buffer to store for local communication
  char* local_buffer = new char[1024];

  // open the first RDMA nic found
  // Note, we assume that there are RDMA NIC
  RNic nic(RNicInfo::query_dev_names()[0]);

  // RDMACtrl is the control hook of rlib.
  // All others bootstrap with it
  RdmaCtrl ctrl(FLAGS_server_port);

  ASSERT(ctrl.mr_factory.register_mr(FLAGS_mr_id, local_buffer, 1024, nic) ==
         SUCC);

  rdma::ConnectHandlers::register_cc_handler(ctrl, nic);

  // Then the server just sleep
  while (true) {
    sleep(1);
  }
}