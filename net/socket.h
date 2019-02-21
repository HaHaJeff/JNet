#ifndef SOCKET_H
#define SOCKET_H
#include "util.h"
#include "net.h"

class Socket : public Noncopyable{
public:
    explicit Socket(int sockfd) : sockfd_(sockfd) {}
    ~Socket();
    int GetFd() const { return sockfd_; }
    bool GetTcpInfo(struct tcp_info*) const;
    bool GetTcpInfoString(char* buf, int len) const;
    void BindAddress(const Ip4Addr& localAddr);
    void Listen();

    int Accept(Ip4Addr& peeraddr);
    void ShutdownWrite();
    void SetTcpNodelay(bool on);
    void SetReuseAddr(bool on);
    void SetReusePort(bool on); 
    void SetKeepAlive(bool on);
private:
    const int sockfd_;
};

#endif