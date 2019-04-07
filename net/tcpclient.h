#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "tcpconn.h"

class TcpClient : Noncopyable {
public:
    TcpClient(EventLoop* loop, const Ip4Addr& local, const Ip4Addr& peer) {}
    ~TcpClient() {}

    void Connect() {}
    void Disconnnect() {}  

    void SetReadCallback(const TcpCallBack& cb) { readCb_ = cb; }
    void SetWriteCallback(const TcpCallBack& cb) { writeCb_ = cb; }
    void SetStateCallback(const TcpCallBack& cb) { stateCb_ = cb; }

private:
    void NewConnection(int sockfd) {}
    void RemoveConnection(const TcpConnPtr& conn) {}

private:
    TcpConn conn_;
    TcpCallBack readCb_;
    TcpCallBack writeCb_;
    TcpCallBack stateCb_;
};

#endif
