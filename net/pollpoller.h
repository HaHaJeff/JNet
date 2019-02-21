#ifndef POLLPOLLER_H
#define POLLPOLLER_H

#include <vector>
#include <map>
#include "poller.h"

class PollPoller: public Poller {
  public:
    PollPoller(EventLoop* loop);
    virtual ~PollPoller();

    TimeStamp Poll(int timeoutMs, ChannelList* activeChannels) override;
    void UpdateChannel(Channel* channel) override;
    void RemoveChannel(Channel* channel) override;
    void AddChannel(Channel* channel) override;

   private:
     void FillActiveChannels(int numEvents, ChannelList* activeChannels) const;
     typedef std::vector<struct pollfd> PollFdList;

   private:
     PollFdList pollfds_;
};

#endif
