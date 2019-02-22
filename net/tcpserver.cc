#include "tcpserver.h"
#include "log.h"
#include <fcntl.h>
#include <unistd.h>

TcpServer::TcpServer(EventLoop* loop):
loop_(loop),
listen_channel_(nullptr),
createcb_([]{return TcpConnPtr(new TcpConn);}) {}

int TcpServer::Bind(const std::string& host, short port, bool reusePort) {
    addr_ = Ip4Addr(host, port);
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
    listen_channel_->SetReadCallback([this]{ HandleAccept();});
    return 0;
}

TcpServerPtr TcpServer::StartServer(EventLoop* loop, const std::string& host, short port, bool reusePort) {
    TcpServerPtr p(new TcpServer(loop));
    int r = p->Bind(host, port, reusePort);
    return r == 0 ? p : nullptr;
}


void TcpServer::HandleAccept() {
    struct sockaddr_in raddr;
    socklen_t rsz = sizeof(raddr);
    int lfd = listen_channel_->GetFd();
    int cfd = 0;
    while (lfd >= 0 && (cfd = accept(lfd, (struct sockaddr*)&raddr, &rsz)) >= 0) {
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

        auto addcon = [=] {
            TcpConnPtr con = createcb_();
            con->Attach(loop_, cfd, local, peer);
            if (statecb_) {
                con->OnState(statecb_);
            }
            if (readcb_) {
                con->OnRead(readcb_);
            }
            if (msgcb_) {
                con->OnMsg(codec_->Clone(), msgcb_);
            }
        };
        addcon();
    }
    if (lfd >= 0 && errno != EAGAIN && errno != EINTR) {
        WARN("accept return %d %d %s", cfd, errno, strerror(errno));
    }
}