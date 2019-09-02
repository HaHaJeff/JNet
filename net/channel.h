#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include <memory>
#include <stdint.h>
#include "net.h"
#include "util.h"
#include "log.h"

namespace jnet {

class EventLoop;

class Channel : public Noncopyable {
public:
    typedef std::function<void()> EventCallback;

    Channel(EventLoop* loop, int fd);
    Channel(EventLoop* loop, int fd, int events);
    ~Channel();

    void SetReadCallback(const EventCallback& cb) { readCallback_ = cb;}
    void SetWriteCallback(const EventCallback& cb) { writeCallback_ = cb;}
    void SetCloseCallback(const EventCallback& cb) { closeCallback_ = cb;}
    void SetErrorCallback(const EventCallback& cb) { errorCallback_ = cb;}
    void SetReadCallback(EventCallback&& cb) { readCallback_ = std::move(cb);}
    void SetWriteCallback(EventCallback&& cb) { writeCallback_ = std::move(cb);}
    void SetCloseCallback(EventCallback&& cb) { closeCallback_ = std::move(cb);}
    void SetErrorCallback(EventCallback&& cb) { errorCallback_ = std::move(cb);}

    void EnableRead() { events_ |= kReadEvent; Update(); }
    void EnableWrite() { events_ |= kWriteEvent; Update(); }
    void DisableRead() { events_ &= ~kReadEvent; Update(); }
    void DisableWrite() { events_ &= ~kWriteEvent; Update(); }
    bool IsWriting() const { return events_ & kWriteEvent; }
    bool IsReading() const { return events_ & kReadEvent; }

    short GetEvents() const { return events_; }
    int GetFd() const { return fd_; }
    int64_t GetId() const { return id_; }
    short GetREvents() const { return revents_; }
    int GetIndex() { return index_; }
    //loop_ will call this function
    void SetREvents(short revents) { revents_ = revents; }
    void SetIndex(int idx) { index_ = idx; }

    EventLoop* GetOwnerLoop() const { return loop_; }

    void HandleEvent();
    void RemoveFromLoop();
    void AddToLoop();

    // DEBUG
    std::string REventsToString() const;
    std::string EventsToString() const;

    void Close();

private:
    std::string EventsToString(int fd, short ev) const;
    void Update();

private:
    EventLoop* loop_;
    short events_;
    short revents_;
    int fd_;
    int64_t id_;
    int index_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;

public:
    static const int kReadEvent;
    static const int kWriteEvent;
};
}

#endif
