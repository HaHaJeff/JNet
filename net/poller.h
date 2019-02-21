#ifndef POLLER_H
#define POLLER_H

#include <vector>
#include <map>
#include "timestamp.h"
#include "util.h"
#include "eventloop.h"

class Channel;

// like libevent
// Base class
// channel的关闭有上层负责，不由Poller管理
class Poller : public Noncopyable {
public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop *loop);
    virtual ~Poller();


    // 统一管理底层的poll select epoll等复用函数
    virtual TimeStamp Poll(int timeoutMs, ChannelList* activeChannels) = 0;

    virtual void UpdateChannel(Channel* channel) = 0;
    virtual void RemoveChannel(Channel* channel) = 0;
    virtual void AddChannel(Channel* channel) = 0;

    bool HasChannel(Channel* channel) const;

    static Poller* NewDefaultPoller(EventLoop* loop);

    void AssertInLoopThread() const {
        ownerLoop_->AssertInLoopThread();
    }

 protected:
    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap channels_;

 private:
    EventLoop* ownerLoop_;
};

#endif