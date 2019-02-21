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
#include "net_test.h"
#include "timerqueue.h"
#include "tcpconn.h"


int func() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  Net::SetNonBlock(fd);
  Socket s(fd);
  Ip4Addr local("127.0.0.1", 8888);
  Ip4Addr ip4("127.0.0.1", 9999);

  EventLoop* loop = new EventLoop();
  auto ptr = TcpConn::CreateConnection<TcpConn>(loop, local, ip4);

  ptr->OnMsg(new LineCode(), [&](const TcpConnPtr& con, const std::string& msg) {
        std::cout << "echo: " << msg << std::endl;
      });

  ptr->SendMsg("123");

  loop->Loop();
  /*
  if (connfd == -1) {
      printf("-1\n");
  }

  struct pollfd pfd;
  pfd.fd = fd;
  pfd.events = POLLOUT | POLLERR;

  //connect完成之后文件描述父可写
  int r = poll(&pfd, 1, 0);

  if (r == 1 && pfd.revents == POLLOUT) {
    printf("connect\n");
  }
  */
}

int main()
{
    func();
}
