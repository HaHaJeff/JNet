#include <poll.h>
#include <atomic>
#include <assert.h>
#include <unistd.h>

#include "channel.h"
#include "eventloop.h"

// POLLPRI: urgent data to read
// POLLNVAL: invalid request, fd not open
// POLLERR: error condition
// POLLRDHUP:  stream socket peer connection close, or shutdown writing of half connection
// POLLHUP: fd is valid, however, a device disconnect
const int Channel::kWriteEvent = POLLOUT;
const int Channel::kReadEvent = POLLIN | POLLPRI;

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      id_(-1)
{
    static std::atomic<int64_t> id(0);
    id_ = ++id;
    FATALIF(Net::SetNonBlock(fd_) < 0, "channel set non block failed");
}

Channel::Channel(EventLoop* loop, int fd, int events)
    : loop_(loop),
      fd_(fd),
      events_(events),
      revents_(0),
      id_(-1)
{
    static std::atomic<int64_t> id(0);
    id_ = ++id;
    FATALIF(Net::SetNonBlock(fd_) < 0, "channel set non block failed");
}


Channel::~Channel() {
    if (fd_ != -1) Close();
}


// response event change from loop_
// EventLoop will change the channel's revents_
void Channel::HandleEvent() {
   TRACE(REventsToString().c_str());
   // handle POLLHUP event
   if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
       WARN("channel id =%d, fd = %d, Channel::HandleEvent() POLLHUP", id_, fd_);
       if (closeCallback_) closeCallback_();
   }
   if (revents_ & POLLNVAL) {
       WARN("channel id = %d, fd = %d, Channel::HandleEvent() POLLNVAL", id_, fd_);
   }
   if (revents_ & (POLLERR | POLLNVAL)) {
       if (errorCallback_) errorCallback_();
   }
   if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
       if (readCallback_) readCallback_();
   }
   if (revents_ & POLLOUT) {
       if (writeCallback_) writeCallback_();
   }
}

void Channel::Update() {
    loop_->UpdateChannel(this);
}

// before call this function, should call Disable
void Channel::RemoveFromLoop() {
    loop_->RemoveChannel(this);
}

void Channel::AddToLoop() {
    loop_->AddChannel(this);
}

std::string Channel::REventsToString() const {
    return EventsToString(fd_, revents_);
}

std::string Channel::EventsToString()  const {
    return EventsToString(fd_, events_);
}

std::string Channel::EventsToString(int fd, short ev) const {
    std::string result = Util::Format("channel id = %d, fd = %d", id_, fd);
    if (ev & POLLIN) result += "IN ";
    if (ev & POLLPRI) result += "PRI ";  // urgent data
    if (ev & POLLOUT) result += "OUT ";
    if (ev & POLLHUP) result += "HUP";
    if (ev & POLLRDHUP) result += "RDHUP";
    if (ev & POLLERR) result += "ERR";
    if (ev & POLLNVAL) result += "NVAL";
    return result;
}

void Channel::Close() {
    if (fd_ >= 0) {
        TRACE("close channel %ld fd %d", (long)id_, fd_);
        close(fd_);
        RemoveFromLoop();
        fd_ = -1;
    }
}
