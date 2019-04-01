#include "protobuf_codec.h"
#include "rpc.pb.h"
#include "unit_test.h"
#include <iostream>

void func(const TcpConnPtr& conn, const MessagePtr&, TimeStamp now) {
    std::cout << now.ToString() << std::endl;
}

TEST(TestBase, ProtobufCodec) {
    TcpConnPtr conn(new TcpConn);
    TimeStamp now = TimeStamp::Now();
    std::shared_ptr<RpcMessage> msg(new RpcMessage);
    msg->set_type(REQUEST);
    msg->set_id(0);
    msg->set_service("test_service");
    msg->set_method("test_method");
    msg->set_request("test_request\0");
    ProtobufCodec codec(msg.get(), std::bind(&func, conn, msg, now));
    codec.Send(conn, *msg);
}

int main()
{
    RunAllTests();
}
