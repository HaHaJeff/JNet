#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <memory>
#include <assert.h>
#include "socket.h"
#include "net.h"
#include "buffer.h"
#include "util.h"
#include "eventloop.h"
#include "tcpconn.h"
#include "codec.h"
#include "channel.h"

class TcpServer;
typedef std::shared_ptr<TcpServer> TcpServerPtr;
class TcpServer : private Noncopyable {
public:
    TcpServer(EventLoop* loop);
    int Bind(const std::string& host, short port, bool reusePort=false);
    static TcpServerPtr StartServer(EventLoop* loop, const std::string& host, short port, bool reusePort=false);
    ~TcpServer() {delete listen_channel_;}
    Ip4Addr GetAddr() { return addr_;}
    EventLoop* GetLoop() { return loop_; }
    void OnConnCreate(const std::function<TcpConnPtr()>& cb) { createcb_ = cb;}
    void OnConnState(const TcpCallBack& cb) { statecb_ = cb;}
    void OnConnRead(const TcpCallBack& cb) { readcb_ = cb; assert(!msgcb_);}
    void OnConnMsg(CodecBase* codec, const MsgCallBack& cb) { codec_.reset(codec); msgcb_ = cb; assert(!readcb_);}
private:
    EventLoop* loop_;
    Ip4Addr addr_;
    Channel* listen_channel_;
    TcpCallBack statecb_, readcb_;
    MsgCallBack msgcb_;
    std::function<TcpConnPtr()> createcb_;
    std::unique_ptr<CodecBase> codec_;
    void HandleAccept();
};

#endif
