#include <sys/epoll.h>
#include <assert.h>
#include <poll.h>
#include <unistd.h>

#include "log.h"
#include "epollpoller.h"
#include "channel.h"

EpollPoller::EpollPoller(EventLoop *loop)
    : Poller(loop),
      epollfd_(epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize)
{
    FATALIF(epollfd_ < 0, "EPollPoller::EpollPoller");
}

EpollPoller::~EpollPoller()
{
    close(epollfd_);
}

// 解耦Poll与EventLoop
// 将有读写事件的channel作为链表返回给EventLoop
TimeStamp EpollPoller::Poll(int timeoutMs, ChannelList *activeChannels)
{
    TRACE("fd total count %d", channels_.size());

    int numEvents = epoll_wait(epollfd_, &*(events_.begin()), static_cast<int>(events_.size()), timeoutMs);
    TimeStamp now(TimeStamp::Now());

    if (numEvents > 0)
    {
        TRACE("%d events happend", numEvents);
        FillActiveChannels(numEvents, activeChannels);

        //如果发现一次epoll_wait返回的数值与当前events_可以容纳的事件数目相等，
        //说明此时网络事件比较活跃，扩容
        if (static_cast<size_t>(numEvents) == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if (numEvents == 0)
    {
        TRACE("nothing happend");
    }
    else
    {
        FATALIF(errno != EINTR, "Epoller::Poll()");
    }
    return now;
}

void EpollPoller::FillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    assert(static_cast<size_t>(numEvents) <= events_.size());

    for (int i = 0; i < numEvents; ++i)
    {
        Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
        channel->SetREvents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EpollPoller::UpdateChannel(Channel *channel)
{
    Poller::AssertInLoopThread();

    TRACE("updating channel %lld fd %d events %d epoll %d", (long long)channel->GetId(), channel->GetFd(), channel->GetEvents(), epollfd_);
    Update(EPOLL_CTL_MOD, channel);
}

void EpollPoller::AddChannel(Channel *channel)
{
    Poller::AssertInLoopThread();

    TRACE("adding channel %lld fd %d events %d epoll %d", (long long)channel->GetId(), channel->GetFd(), channel->GetEvents(), epollfd_);
    Update(EPOLL_CTL_ADD, channel);
    channels_[channel->GetFd()] = channel;
}

void EpollPoller::RemoveChannel(Channel *channel)
{
    Poller::AssertInLoopThread();

    assert(channels_.find(channel->GetFd()) != channels_.end());
    assert(channels_[channel->GetFd()] == channel);

    TRACE("deleting channel %lld fd %d epoll %d", (long long)channel->GetId(), channel->GetFd(), epollfd_);
//FIXME: if fd's reference is zero, close it, epoll will automatic remove it from interesting tree
//reduce a system call
//    Update(EPOLL_CTL_DEL, channel);
    channels_.erase(channel->GetFd());
}

void EpollPoller::Update(int operation, Channel* channel) {
    int fd = channel->GetFd();
    struct epoll_event event;
    memset(&event, 0, sizeof(struct epoll_event));
    event.events = channel->GetEvents();
    event.data.ptr = channel;

    int ret = epoll_ctl(epollfd_, operation, fd, &event);
//    FATALIF(ret < 0, "epoll_ctl %s failed %d %s", EpollPoller::OperationToString(operation).c_str(), errno, strerror(errno));
}

std::string EpollPoller::OperationToString(int op) {
    switch(op) {
        case EPOLL_CTL_ADD:
            return "ADD";
        case EPOLL_CTL_DEL:
            return "DEL";
        case EPOLL_CTL_MOD:
            return "MOD";
        default:
            assert(false);
            return "Unknown operation";
    }
}
