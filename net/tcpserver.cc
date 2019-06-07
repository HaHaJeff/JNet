#include "tcpserver.h"
#include "log.h"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

namespace jnet {

TcpServer::TcpServer(EventLoop* loop, const Ip4Addr& addr):
    loop_(loop),
    addr_(addr),
    listen_channel_(nullptr),
    threadPool_(new EventLoopPool(loop))
{}

TcpServer::TcpServer(EventLoop* loop):
    loop_(loop),
    listen_channel_(nullptr),
    threadPool_(new EventLoopPool(loop))
{}

TcpServer::~TcpServer() {
    loop_->AssertInLoopThread();
    TRACE("TcpServer::~TcpServer");

    for (auto it : conns_) {
        TcpConnPtr con = it;
        it.reset();
        con->GetLoop()->RunInLoop(std::bind(&TcpConn::Close, con));
    }
    delete listen_channel_;
}

int TcpServer::Bind(const std::string& host, short port, bool reusePort) {
    addr_ = Ip4Addr(host, port);
    Bind(reusePort);
}

void TcpServer::Start(bool reusePort) {
    Bind(reusePort);
    threadPool_->Start(); 
    loop_->Loop();
}

int TcpServer::Bind(bool reusePort) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int r = Net::SetReuseAddr(fd);
    FATALIF(r, "set socket reuse addr option failed");
    r = Net::SetReusePort(fd, reusePort);
    FATALIF(r, "set socket reuse port option failed");
    r = ::bind(fd, (struct sockaddr *)&addr_.GetAddr(), sizeof(struct sockaddr));
    if(r != 0) {
        Net::Close(fd);
        ERROR("bind to %s failed %d %s", addr_.ToString().c_str(), errno, strerror(errno));
        return errno;
    }
    r = ::listen(fd, 20);
    FATALIF(r, "listen failed %d %s", errno, strerror(errno));
    INFO("fd %d listening at %s", fd, addr_.ToString().c_str());
    listen_channel_ = new Channel(loop_, fd, Channel::kReadEvent);
    listen_channel_->SetReadCallback([=]{ HandleAccept();});
    listen_channel_->AddToLoop();
    return 0;
}

TcpServerPtr TcpServer::StartServer(EventLoop* loop, const std::string& host, short port, int threads, bool reusePort) {
    TcpServerPtr p = std::make_shared<TcpServer>(loop);
    int r = p->Bind(host, port, reusePort);
    p->StartThread(threads);
    return r == 0 ? p : nullptr;
}

void TcpServer::StartThread(int threads) {
    SetThreadNum(threads);
    threadPool_->Start();
}

void TcpServer::HandleAccept() {
    struct sockaddr_in raddr;
    socklen_t rsz = sizeof(raddr);
    int lfd = listen_channel_->GetFd();
    int cfd = -1;
    while (lfd >= 0 && (cfd = accept(lfd, (struct sockaddr*)&raddr, &rsz)) >= 0) {
        INFO("fd %d accept at %d", lfd, cfd);
        sockaddr_in peer, local;
        socklen_t alen = sizeof(peer);
        int r = getpeername(cfd, (sockaddr*)&peer, &alen);
        if (r < 0) {
            ERROR("get peer name failed %d %s", errno, strerror(errno));
            continue;
        }
        r = getsockname(cfd, (sockaddr*)&local, &alen);
        if (r < 0) {
            ERROR("getsockname failed %d %s", errno, strerror(errno));
            continue;
        }
        r = fcntl(cfd, F_SETFD, FD_CLOEXEC);
        FATALIF(r, "set fd FD_CLOEXEC failed");

        EventLoop* ioLoop = threadPool_->GetNextLoop();
        TcpConnPtr con = std::make_shared<TcpConn>();
        if (createcb_ != nullptr)
            createcb_(con);
        // FIXME: always hold shared_ptr in tcpserver
        conns_.insert(con);
        ioLoop->RunInLoop([=](){con->Attach(ioLoop, cfd, local, peer);});
        if (statecb_) {
            con->OnState(statecb_);
        }
        if (readcb_) {
            con->OnRead(readcb_);
        }
    }
    if (lfd >= 0 && errno != EAGAIN && errno != EINTR) {
        WARN("accept return %d %d %s", cfd, errno, strerror(errno));
    }
}

void TcpServer::SetThreadNum(int num) {
    threadPool_->SetThreadNum(num);
}

void TcpServer::RemoveConn(const TcpConnPtr& conn) {
    loop_->RunInLoop(std::bind(&TcpServer::RemoveConnInLoop, this, conn));
}

void TcpServer::RemoveConnInLoop(const TcpConnPtr& conn) {
    loop_->AssertInLoopThread();
    TRACE("TcpServer::RemoveConnInLoop");
    size_t n = conns_.erase(conn);
    EventLoop* ioLoop = conn->GetLoop();
    ioLoop->QueueInLoop(std::bind(&TcpConn::Close, conn));
}
}
