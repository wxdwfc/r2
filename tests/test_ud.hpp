#include "../deps/rlib/rdma_ctrl.hpp"

#include <string.h>

using namespace rdmaio;

namespace test {

const int tcp_port = 9999;
const int buf_size = 1024;
const int ud_id    = 1;
const int mr_id    = 73;

TEST(RdmaTest, UDConnect) {
  char *test_buffer = new char[buf_size];
  // write something to the test buffer
  Marshal::serialize_to_buf<uint64_t>(0,test_buffer);

  auto all_devices = RNicInfo::query_dev_names();
  ASSERT_FALSE(all_devices.empty());

  RNic nic(all_devices[0]);
  ASSERT_TRUE(nic.ready());

  {
    RdmaCtrl ctrl(tcp_port);
    ASSERT_EQ(ctrl.mr_factory.register_mr(mr_id,test_buffer,buf_size,nic),SUCC);

    RemoteMemory::Attr local_mr_attr;
    auto ret = RMemoryFactory::fetch_remote_mr(mr_id,
                                               std::make_tuple("localhost",tcp_port),
                                               local_mr_attr);
    ASSERT_EQ(ret,SUCC);

    // try fetch ud attr
    auto qp = new UDQP(nic,local_mr_attr,QPConfig().set_max_send(1).set_max_recv(2));
    //delete qp;
    ASSERT_TRUE(ctrl.qp_factory.register_ud_qp(ud_id,qp));

    QPAttr original_attr = qp->get_attr();
    QPAttr fetched_attr;

    ASSERT_EQ(QPFactory::fetch_qp_addr(QPFactory::UD,ud_id,std::make_tuple("localhost",tcp_port),
                                       fetched_attr),SUCC);
    ASSERT_EQ(memcmp(&original_attr,&fetched_attr,sizeof(QPAttr)),0);
  }

  delete test_buffer;
}

} // end namespace test
