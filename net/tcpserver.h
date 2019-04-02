#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <memory>
#include <set>
#include "socket.h"
#include "net.h"
#include "buffer.h"
#include "util.h"
#include "eventloop.h"
#include "tcpconn.h"
#include "codec.h"
#include "channel.h"
#include "eventloop_pool.h"

class TcpServer;
typedef std::shared_ptr<TcpServer> TcpServerPtr;
class TcpServer : Noncopyable {
public:
    TcpServer(EventLoop* loop);
    TcpServer(EventLoop* loop, const Ip4Addr& addr);
    ~TcpServer();
    int Bind(bool reusePort = false);
    int Bind(const std::string& host, short port, bool reusePort=false);
    static TcpServerPtr StartServer(EventLoop* loop, const std::string& host, short port, int threads = 0, bool reusePort=false);
    Ip4Addr GetAddr() { return addr_;}
    EventLoop* GetLoop() { return loop_; }
    void SetThreadNum(int num);
    void OnConnCreate(const std::function<void(const TcpConnPtr&)>& cb) { createcb_ = cb;}
    void OnConnState(const TcpCallBack& cb) { statecb_ = cb;}
    void OnConnRead(const TcpCallBack& cb) { readcb_ = cb;}
    void Start(bool reusePort = false);
private:
    typedef std::set<TcpConnPtr> ConnSet;
    EventLoop* loop_;
    Ip4Addr addr_;
    Channel* listen_channel_;
    TcpCallBack statecb_, readcb_;
    std::function<void (const TcpConnPtr&)> createcb_;
    std::unique_ptr<CodecBase> codec_;
    std::unique_ptr<EventLoopPool> threadPool_;
    ConnSet conns_;
private:
    void HandleAccept();
    void RemoveConn(const TcpConnPtr& conn);
    void RemoveConnInLoop(const TcpConnPtr& conn);
    void StartThread(int threads = 0);
};

#endif
