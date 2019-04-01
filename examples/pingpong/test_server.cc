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
#include "tcpserver.h"
#include <thread>

int func() { 
  Ip4Addr addr("127.0.0.1", 9999);

  EventLoop* loop = new EventLoop();
  TcpServerPtr ptr = TcpServer::StartServer(loop, "127.0.0.1", 9999, 2, true);
  ptr->OnConnRead([](const TcpConnPtr& con)
  {
      TcpConnPtr ptr= con;
      Buffer& b = ptr->GetInput();
      std::cout << b.GetData() << std::endl;
      ptr->Send(b);
  });
  loop->Loop();
}

int main()
{
    SETLOGLEVEL("FATAL");
    func();
//    std::thread t1(func);
   // std::thread t2(func);
  //  func();
//   t1.join();
  //  t2.join();
}
