#include "poller.h"
#include "epollpoller.h"
#include "pollpoller.h"

Poller* Poller::NewDefaultPoller(EventLoop* loop) {
    return new EpollPoller(loop);
    //return new PollPoller(loop);
}
