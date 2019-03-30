#include "protobuf_codec.h"

#include "unit_test.h"
#include <iostream>

void func(const TcpConnPtr& conn, TimeStamp now) {
    std::cout << now.ToString() << std::endl;
}

TEST(TestBase, ProtobufCodec) {
    TcpConnPtr conn(new TcpConn);
    TimeStamp now = TimeStamp::Now();
    ProtobufCodec codec(nullptr, std::bind(&func, conn, now));
}

int main()
{
    RunAllTests();
}
