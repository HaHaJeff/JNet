#ifndef EPOLLPOLLER_H
#define EPOLLPOLLER_H

#include <vector>
#include "poller.h"

struct epoll_event;

class EpollPoller: public Poller {
  public:
    EpollPoller(EventLoop* loop);
    ~EpollPoller();

    // 统一管理底层的poll select epoll等复用函数
    TimeStamp Poll(int timeoutMs, ChannelList* activeChannels) override;
    void UpdateChannel(Channel* channel) override;
    void RemoveChannel(Channel* channel) override;
    void AddChannel(Channel* channel) override;

  private:
    static const int kInitEventListSize = 16;
    void FillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    static std::string OperationToString(int op);
    typedef std::vector<struct epoll_event> EventList;
    void Update(int operation, Channel* channel);

  private:
    int epollfd_;
    EventList events_;

};

#endif
