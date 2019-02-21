#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "util.h"
#include "net.h"

#include <functional>
#include <memory>

class Channel;
class EventLoop;

class Connector : private Noncopyable, public std::enable_shared_from_this<Connector> {
public:
    typedef std::function<void (int sockfd)> NewConnectionCallback;

    Connector(EventLoop* loop, const Ip4Addr& serverAddr);
    ~Connector();

    void SetNewConnectionCallback(const NewConnectionCallback& cb) {
        newConnectionCallback_ = cb;    
    }

    void Start();
    void Stop();
    void Restart();

    const Ip4Addr& GetServerAddress() const { return serverAddr_; }

private:
    enum State { kDisconnected, kConnecting, kConected };
    static const int kMaxRetryDelayMs = 30*1000;
    static const int kInitRetryDelayMs = 500;

    void SetState(State s) { state_ = s; }
    void StartInLoop();
    void StopInLoop();
    void Connect();

private:
    EventLoop* loop_;
    Ip4Addr serverAddr_;
    bool isConnected_;
    State state_;
    std::unique_ptr<Channel> channel_;
    NewConnectionCallback newConnectionCallback_;
    int retryDelayMs_;
};

#endif