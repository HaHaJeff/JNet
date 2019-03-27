#ifndef TCPCONN_H
#define TCPCONN_H
#include <memory>
#include <assert.h>
#include "socket.h"
#include "net.h"
#include "buffer.h"
#include "util.h"
#include "eventloop.h"
#include "codec.h"

struct AutoContext: Noncopyable {
  void* ctx;
  std::function<void()> ctxDel;
  AutoContext():ctx(nullptr) {}

  template<class T>
  T& context() {
    if (nullptr == ctx) {
        ctx = new T();
        ctxDel = [this] {delete (T*)ctx;};
    }
    return *(T*)ctx;
  }

  ~AutoContext() { if (ctx != nullptr) ctxDel(); }
};

class TcpConn;
typedef std::shared_ptr<TcpConn> TcpConnPtr;
typedef std::function<void(const TcpConnPtr&)> TcpCallBack;
typedef std::function<void(const TcpConnPtr&, std::string& msg)> MsgCallBack;

class TcpConn : public std::enable_shared_from_this<TcpConn>, private Noncopyable{
public:
    enum State { kInvalid = 1, kHandShakeing, kConnected, kClosed, kFailed };
    TcpConn() {};
    TcpConn(EventLoop* loop, const Ip4Addr& local, const Ip4Addr& peer, int timeout=0);
    ~TcpConn() {}

    template <class T=TcpConn>
    static TcpConnPtr CreateConnection(EventLoop* loop, const Ip4Addr& local, const Ip4Addr& peer, int timeout = 0) {
        TcpConnPtr con = std::make_shared<T>();
        con->Connect(loop, local, peer, timeout);
        return con;
    }

    template <class T=TcpConn>
    static TcpConnPtr CreateConnection(EventLoop* loop, int fd, const Ip4Addr& local, const Ip4Addr& peer) {
        TcpConnPtr con = std::make_shared<T>();
        con->Attach(loop, fd, local, peer);
        return con;
    }

    EventLoop* GetLoop() const { return loop_; }
    const Ip4Addr GetLocalAddress() const { return localAddr_; }
    const Ip4Addr GetPeerAddress() const { return peerAddr_; }
    bool IsConnected() const {return state_ == kConnected; }
    bool GetTcpInfo(struct tcp_info* info) const { assert(IsConnected()); socket_->GetTcpInfo(info); return true;}
    bool IsClient() const {return peerAddr_.GetPort() > 0;}
    std::string GetTcpinfoString() const { char buf[1024]; socket_->GetTcpInfoString(buf, 1024); return buf;}
    State GetState() const { return state_; }

    // TODO: not thread safe
    void Send(const char* message, size_t len);
    void Send(const char* s) { Send(s, strlen(s));}
    void Send(const std::string& message) { Send(message.c_str()); }
    void Send(Buffer& message);
    void SendOutput() { Send(output_); }
    ssize_t ISend(const char* buf, size_t len);
    void SendInLoop(const char* message, size_t len);

    //数据到达时回调
    void OnRead(const TcpCallBack& cb) { readcb_ = cb; }
    //数据可写时回调
    void OnWrite(const TcpCallBack& cb) { writecb_ = cb; }
    //状态改变时回调
    void OnState(const TcpCallBack& cb) { statecb_ = cb; }
    void OnMsg(CodecBase* codec, const MsgCallBack& cb);
    void SendMsg(std::string msg);

    void Connect(EventLoop* loop, const Ip4Addr& local, const Ip4Addr& peer, int timeout=0);
    void Close();

    void CleanUp(const TcpConnPtr& con);
    void Shutdown();
    void ShutdownInLoop();

    Buffer& GetInput() { return input_; }
    Buffer& GetOutput() { return output_; }
    AutoContext& GetContext() { return ctx_;}

    void Attach(EventLoop* loop, int fd, const Ip4Addr& local, const Ip4Addr& peer);
private:
    void HandleRead(const TcpConnPtr& con);
    void HandleWrite(const TcpConnPtr& con);
    int HandleHandShake(const TcpConnPtr& con);

private:
    EventLoop* loop_;
    Ip4Addr localAddr_;
    Ip4Addr peerAddr_;
    State state_;
    Buffer input_;
    Buffer output_;
    TcpCallBack readcb_;
    TcpCallBack writecb_;
    TcpCallBack statecb_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    std::unique_ptr<CodecBase> codec_;
    AutoContext ctx_;
    int connectedTimeout_;
    int connectTimeout_;
    TimerId timeoutId_;
    int fd_;

};

#endif
