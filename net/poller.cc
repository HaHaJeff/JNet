#include "poller.h"
#include "channel.h"

namespace jnet {

Poller::Poller(EventLoop *loop)
    : ownerLoop_(loop)
{
}

Poller::~Poller() { }

bool Poller::HasChannel(Channel* channel) const {
    AssertInLoopThread();
    ChannelMap::const_iterator it = channels_.find(channel->GetFd());
    return it != channels_.end() && it->second == channel;
}
}
