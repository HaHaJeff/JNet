#ifndef NET_H
#define NET_H
#include <netdb.h>
#include <string>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

template<typename To, typename From>
inline To implicit_cast(From const&f) {
    return f;
}

const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);

class Net {
  public:
    static int SetNonBlock(int fd, bool value=true);
    static int SetReuseAddr(int fd, bool value=true);
    static int SetReusePort(int fd, bool value=true);
    static int SetNoDelay(int fd, bool value=true);
    static int SetKeepAlive(int fd, bool value=true);
    static struct in_addr GetHostByName(const std::string& host);
    static void Bind(int sockfd, const struct sockaddr* addr);
    static void Listen(int sockfd);
    static int Accept(int sockfd, struct sockaddr* addr);
    static int Connect(int sockfd, const struct sockaddr* addr);
    static ssize_t Read(int sockfd, void *buf, size_t count);
    static ssize_t Write(int sockfd, const void* buf, size_t count);
    static void Close(int sockfd);
    static void ShutdownWrite(int sockfd);
};

class Ip4Addr {
public:
    Ip4Addr(const std::string& host, short port);
    Ip4Addr(short port = 0) : Ip4Addr("", port) { }
    Ip4Addr(const struct sockaddr_in& addr) : addr_(addr) {}
    std::string ToString() const;
    std::string GetIp() const;
    short GetPort() const;
    unsigned int GetIpInt() const;
    unsigned int GetNetEndianIp() const;
    short GetNetEndianPort() const;
    bool IsIpValid() const;
    struct sockaddr_in& GetAddr() { return addr_;}
    const struct sockaddr_in& GetAddr() const { return addr_; }
    void SetSockaddr(const struct sockaddr_in& addr) { addr_ = addr; }

private:
    struct sockaddr_in addr_;
};

#endif
