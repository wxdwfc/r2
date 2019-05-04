#include <gtest/gtest.h>

#include "rlib/rdma_ctrl.hpp"
#include "../src/msg/ud_msg.hpp"
#include "rlib/ralloc/ralloc.h"

namespace test {

using namespace r2;
using namespace rdmaio;

const int tcp_port2 = 8888;
const int buf_size2 = 64 * 1024 * 1024;
const int mr_id    = 73;

TEST(UdMsgTest, simple) {

  char *test_buffer = new char[buf_size2];
  // write something to the test buffer
  Marshal::serialize_to_buf<uint64_t>(0,test_buffer);

  RInit(test_buffer, buf_size2);
  RThreadLocalInit();

  auto all_devices = RNicInfo::query_dev_names();
  ASSERT_FALSE(all_devices.empty());

  RNic nic(all_devices[0]);
  ASSERT_TRUE(nic.ready());
  {
    // first we create the UDPQ like the previous test_ud connect test.
    RdmaCtrl ctrl(tcp_port2);
    ASSERT_EQ(ctrl.mr_factory.register_mr(mr_id,test_buffer,buf_size2,nic),SUCC);

    RemoteMemory::Attr local_mr_attr;
    auto ret = RMemoryFactory::fetch_remote_mr(mr_id,
                                               std::make_tuple("localhost",tcp_port2),
                                               local_mr_attr);
    ASSERT_EQ(ret,SUCC);

    auto qp = new UDQP(nic,local_mr_attr,QPConfig().set_max_send(64).set_max_recv(2048));
    ASSERT_TRUE(qp->valid());
    // create the UDMsg
    int ud_id = 73;

    UdAdapter adapter({.mac_id = 0,.thread_id = 73},qp);
    ASSERT_TRUE(ctrl.qp_factory.register_ud_qp(ud_id,qp));
    ASSERT_EQ(adapter.connect({.mac_id = 0,.thread_id = 73}, // connect to myself
                              ::rdmaio::make_id("localhost",tcp_port2),ud_id),
              SUCC);

    int total_msg_count = 512;
    char *send_buf = (char *)Rmalloc(64);
    for(uint i = 0;i < total_msg_count;++i) {
      sprintf(send_buf, "sender message: %d", i);
      ASSERT_EQ(adapter.send({.mac_id = 0,.thread_id = 73},send_buf,strlen(send_buf) + 1),SUCC);
    }

    sleep(1);

    // now we check the message contents
    int received_count = 0;
    while(received_count < total_msg_count) {
      adapter.poll_all([&received_count](const char *msg,int size,const Addr &addr) {
                         ASSERT_EQ(addr.thread_id,73);
                         received_count += 1;
                         //LOG(2) << "received msg: " << msg;
                       });
    }

    // pass the connect phase
  }
} // end test function

TEST(UdMsgTest, iter) {
  // TODO: implemented
}

} // end namespace test
