#include <iostream>
#include <poll.h>
#include "socket.h"
#include "net.h"
#include "buffer.h"
#include "eventloop.h"
#include "log.h"
#include "timer.h"
#include "timestamp.h"
#include "channel.h"
#include "processinfo.h"
#include "timerqueue.h"
#include "tcpconn.h"
#include "rpc_server.h"
#include <thread>
#include "echo.pb.h"

using namespace jrpc;
using namespace jnet;

class EchoServiceImpl : public ::echo::EchoService {
public:
    virtual void Echo(::google::protobuf::RpcController* controller,
                      const ::echo::EchoRequest* request,
                      ::echo::EchoResponse* response,
                      ::google::protobuf::Closure* done) {
        response->set_msg(request->msg());
        done->Run();
    }
};

int func() { 
  Ip4Addr addr("127.0.0.1", 9999);

  EchoServiceImpl impl;
  EventLoop* loop = new EventLoop();
  RpcServer server(loop, addr);
  server.RegisterService(&impl);
  server.Start();
}

int main()
{
//    SETLOGLEVEL("FATAL");
    func();
}
