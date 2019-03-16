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
#include <thread>
int func(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    Net::SetNonBlock(fd);
    Socket s(fd);
    //Ip4Addr local1("127.0.0.1", port);
    //Ip4Addr local2("127.0.0.1", port+1);
    Ip4Addr ip4("127.0.0.1", 9999);

    char buf[4096];
    for (int i = 0; i < 4095; i++) {
        buf[i] = 'a';
    }
    buf[4095] = '\0';

    EventLoop* loop = new EventLoop();
    std::vector<TcpConnPtr> vec;
    for (int i = 0; i < 10; i++) {
        Ip4Addr local1("127.0.0.1", port+i);
        auto ptr = TcpConn::CreateConnection<TcpConn>(loop, local1, ip4);
        vec.push_back(ptr);
        int cnt = 1;
        int k = 10000;
        TimeStamp start = TimeStamp::Now();  
        vec[i]->OnRead([&](const TcpConnPtr& con){
            if (cnt >= k) {
                TimeStamp end = TimeStamp::Now();
                double clicks = TimeDifference(start, end)-1; 
                double qps = cnt/clicks;
                double out = cnt*4096/clicks;
                std::cout <<"elapse: " << clicks <<  " qps: " << qps << " through output: " << out/(1024*1024) << std::endl;
                return;
            }
            Buffer& b = con->GetInput();
            con->Send(b);
            cnt++;
        });
        loop->RunAfter(1, [&vec,&buf,i](){vec[i]->Send(buf, sizeof(buf));});
    }
    loop->Loop(); 
}

int main()
{
    SETLOGLEVEL("FATAL");
    func(8888);
}
