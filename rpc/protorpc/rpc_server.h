#ifndef RPCSERVER_H
#define RPCSERVER_H

#include "tcpserver.h"
#include "net.h"
namespace google{
namespace protobuf{
class Service;
}
}
#include "tcpserver.h"
#include "net.h"
#include "eventloop.h"
#include <map>
#include <string>
//
// extend tcpserver
//
class RpcServer {
public:
    RpcServer(EventLoop* loop, const Ip4Addr& addr);

    void SetThreadNum(int numThreads) { server_.SetThreadNum(numThreads); }

    void RegisterService(::google::protobuf::Service*);

    void Start(bool reusePort = false);

private:
    void OnConnection(const TcpConnPtr& conn);

private:
    TcpServer server_;
    //
    // key: method full name, value: service pointer
    std::map<std::string, ::google::protobuf::Service*> services_;
};

#endif
