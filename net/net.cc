#include <fcntl.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <utility>
#include "net.h"
#include "net/log.h"
#include "util.h"

namespace jnet {

const struct sockaddr* sockaddr_cast(const struct sockaddr_in *addr) {
    return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

struct sockaddr* sockaddr_cast(struct sockaddr_in* addr) {
    return reinterpret_cast<struct sockaddr*>(addr);
}

int Net::SetNonBlock(int fd, bool value) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    return errno;
  }
  if (value) {
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  }

  return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
}

int Net::SetReuseAddr(int fd, bool value) {
    int flag = value;
    int len = sizeof(flag);
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, len);
}

int Net::SetReusePort(int fd, bool value) {
    int flag = value;
    int len = sizeof(flag);
    return setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag, len);
}

int Net::SetNoDelay(int fd, bool value) {
    int flag = value;
    int len = sizeof(flag);
    return setsockopt(fd, SOL_SOCKET, TCP_NODELAY, &flag, len);
}

int Net::SetKeepAlive(int fd, bool value) {
    int flag = value;
    int len = sizeof(flag);
    return setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &flag, len);
}

struct in_addr Net::GetHostByName(const std::string& host) {
    struct in_addr addr;
    char buf[1024];
    struct hostent hent;
    struct hostent* he = nullptr;
    int herrno = 0;

    memset(&hent, 0, sizeof hent);
    int r = gethostbyname_r(host.c_str(), &hent, buf, sizeof buf, &he, &herrno);
    if (r == 0 && he && he->h_addrtype == AF_INET) {
        addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
    } else {
        addr.s_addr = INADDR_NONE;
    }
    return addr;
}

void Net::Bind(int sockfd, const struct sockaddr* addr) {
    int ret = bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr)));
    if (ret < 0) {
        FATAL("Net::Bind");
    }
}

void Net::Listen(int sockfd) {
    int ret = listen(sockfd, SOMAXCONN);
    if (ret < 0) {
        FATAL("Net::Listen");
    }
}

int Net::Accept(int sockfd, struct sockaddr* addr) {
    socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
    int connfd = accept(sockfd, addr, &addrlen);
    if (connfd < 0) {
        int savedErrno = errno;
        ERROR("Net::Accept");
        switch(savedErrno) {
            case EAGAIN: // nonblock and no connection
            case ECONNABORTED: // a connection has been aborted
            case EINTR: // the system call was interrupted by a signal that was caught before a valid connection arrived
            case EPROTO: //protocol error
            case EPERM: // firewall rules forbid connection
            default:
              FATAL("accept unexpected error");
              break;
        }
    }
    return connfd;
}

int Net::Connect(int sockfd, const struct sockaddr* addr) {
    return connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr)));
}

ssize_t Net::Read(int sockfd, void* buf, size_t count) {
    return read(sockfd, buf, count);
}

ssize_t Net::Write(int sockfd, const void* buf, size_t count) {
    return write(sockfd, buf, count);
}

Ip4Addr::Ip4Addr(const std::string& host, short port) {
    memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    if (host.size()) {
        addr_.sin_addr = Net::GetHostByName(host);
    } else {
        addr_.sin_addr.s_addr = INADDR_ANY;
    }

    if (addr_.sin_addr.s_addr == INADDR_NONE) {
        ERROR("cannot resove %s to ip", host.c_str());
    }
}

void Net::Close(int sockfd) {
    close(sockfd);
}

void Net::ShutdownWrite(int sockfd) {
    shutdown(sockfd, SHUT_WR);
}

// 网络字节序，大端模式
std::string Ip4Addr::ToString() const {
    uint32_t uip = addr_.sin_addr.s_addr;
    return Util::Format("%d.%d.%d.%d:%d", (uip>>0) & 0xff, (uip>>8) & 0xff, (uip>>16) & 0xff, (uip>>24) & 0xff, ntohs(addr_.sin_port));
}

std::string Ip4Addr::GetIp() const {
    uint32_t uip = addr_.sin_addr.s_addr;
    return Util::Format("%d.%d.%d.%d", (uip>>0) & 0xff, (uip>>8) & 0xff, (uip>>16) & 0xff, (uip>>24) & 0xff);
}

short Ip4Addr::GetPort() const {
    return ntohs(addr_.sin_port);
}

unsigned int Ip4Addr::GetIpInt() const {
    return ntohl(addr_.sin_addr.s_addr);
}

bool Ip4Addr::IsIpValid() const {
    return addr_.sin_addr.s_addr != INADDR_NONE;
}
}
