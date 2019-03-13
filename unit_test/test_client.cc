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
#include "unit_test.h"
#include "timerqueue.h"
#include "tcpconn.h"

int func() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  Net::SetNonBlock(fd);
  Socket s(fd);
  Ip4Addr local("127.0.0.1", 8888);
  Ip4Addr ip4("127.0.0.1", 9999);

  char buf[4096];
  for (int i = 0; i < 4095; i++) {
      buf[i] = 'a';
  }
  buf[4095] = '\0';

  EventLoop* loop = new EventLoop();
  auto ptr = TcpConn::CreateConnection<TcpConn>(loop, local, ip4);

  ptr->OnRead([](const TcpConnPtr& con){
        Buffer& b = con->GetInput();
        std::cout << "echo: " << b.GetData() << std::endl;
        con->Send(b);
      });
  ptr->Send(buf, sizeof(buf));

  loop->Loop();
}

int main()
{
    func();
}
