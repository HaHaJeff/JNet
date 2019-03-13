#include "tcpconn.h"
#include "log.h"
#include "channel.h"
#include <poll.h>
#include <functional>


TcpConn::TcpConn(EventLoop* loop, const Ip4Addr& local, const Ip4Addr& peer, int timeout) :
    loop_(loop),
    localAddr_(local),
    peerAddr_(peer),
    connectTimeout_(timeout) {
}

void TcpConn::Attach(EventLoop* loop, int fd, const Ip4Addr& local, const Ip4Addr& peer) {
    loop_ = loop;
    state_ = State::kHandShakeing;
    localAddr_ = local;
    peerAddr_ = peer;

    Channel* ch = new Channel(loop, fd, Channel::kWriteEvent | Channel::kReadEvent);
    socket_.reset(new Socket(fd));
    channel_.reset(ch);
    channel_->AddToLoop();

    TRACE("tcp constructed %s - %s fd: %d events: %d", localAddr_.ToString().c_str(), peerAddr_.ToString().c_str(), fd, channel_->GetEvents());

    TcpConnPtr conn = shared_from_this();
    conn->channel_->SetReadCallback([=]{conn->HandleRead(conn); });
    conn->channel_->SetWriteCallback([=]{conn->HandleWrite(conn); });
}

void TcpConn::Connect(EventLoop* loop, const Ip4Addr& local, const Ip4Addr& peer, int timeout) {
    localAddr_ = local;
    peerAddr_ = peer;
    connectTimeout_ = timeout;
    connectedTimeout_ = Util::TimeMilli();

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    Net::SetNonBlock(fd);

    int r = 0;
    r = bind(fd, sockaddr_cast(&localAddr_.GetAddr()), sizeof(struct sockaddr));

    if (r < 0) {
        ERROR("bind to %s failed error %d %s", localAddr_.ToString().c_str(), errno, strerror(errno));
    }
    if (r == 0) {
        r = connect(fd, sockaddr_cast(&peer.GetAddr()), sizeof(struct sockaddr));
        //  The socket is nonblocking and the connection cannot be completed immediately.
        if (r != 0 && errno != EINPROGRESS) {
            ERROR("connect to %s error %d %s", peerAddr_.ToString().c_str(), errno, strerror(errno));
        }

    }

    struct sockaddr_in localin = localAddr_.GetAddr();
    socklen_t len = sizeof(local);

    if (r == 0) {
        r = getsockname(fd, sockaddr_cast(&localin), &len);
        if (r < 0) {
            ERROR("getsockname failed %d %s", errno, strerror(errno)); }
    }

    state_ = State::kHandShakeing;
    // if connected has successed, fd is writeable
    Attach(loop, fd, localAddr_, peerAddr_);
    if (timeout) {
        TcpConnPtr con = shared_from_this();
        timeoutId_ = loop_->RunAfter(timeout, [con] {
            if (con->GetState() == kHandShakeing) { con->channel_->Close(); }
        });
    }
}

void TcpConn::Close() {
    if (channel_ != nullptr) {
        TcpConnPtr con = shared_from_this();
        loop_->RunInLoop([con]{ if (con->channel_) con->channel_->Close(); });
    }
}

void TcpConn::CleanUp(const TcpConnPtr& con) {
    if (readcb_ && input_.GetSize()) {
        readcb_(con);
    }
    if (state_ == State::kHandShakeing) {
        state_ = State::kFailed;
    } else {
        state_ = State::kClosed;
    }

    TRACE("tcp closeing %s - %s fd %d %d",
        localAddr_.ToString().c_str(),
        peerAddr_.ToString().c_str(),
        channel_ ? channel_->GetFd(): -1, errno);
    loop_->Cancel(timeoutId_);

    if (statecb_) {
        statecb_(con);
    }

    readcb_ = writecb_ = statecb_ = nullptr;
    auto ch = channel_.release();
    delete ch;
}

void TcpConn::HandleRead(const TcpConnPtr& con) {
   if (state_ == State::kHandShakeing && HandleHandShake(con)) {
       return;
   }
   while (state_ == State::kConnected) {
       input_.MakeRoom();
       int rd = 0;
       if (channel_->GetFd() >= 0) {
           rd = read(channel_->GetFd(), input_.End(), input_.GetSpace());
           TRACE("channel %lld fd %d read %d bytes", (long long)channel_->GetId(), channel_->GetFd(), rd);
       }
       // Teh call interrupted by signal
       if (rd == -1 && errno == EINTR) {
           continue;
       } else if(rd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
           TRACE("channel %lld fd %d call readcb_, input size %d, con %p", (long long)channel_->GetId(), channel_->GetFd(), input_.GetSize(), con.get());
           if (readcb_ && input_.GetSize() && con != nullptr) {
               readcb_(con);
           }
           break;
       } else if (channel_->GetFd() == -1 || rd == -1 || rd == 0) {
           TRACE("channel %lld fd %d call cleanUP", (long long)channel_->GetId(), channel_->GetFd());
           CleanUp(con);
           break;
       } else {
           input_.AddSize(rd);
       }
   }
}

int TcpConn::HandleHandShake(const TcpConnPtr& con) {
    struct pollfd pfd;
    pfd.fd = channel_->GetFd();
    pfd.events = POLLOUT | POLLERR;
    int r = poll(&pfd, 1, 0);

    // if fd is connected, fd is writable
    if (r == 1 && pfd.revents == POLLOUT) {
        channel_->EnableRead();
        state_ = State::kConnected;
        if (state_ == State::kConnected) {
            connectedTimeout_ = Util::TimeMilli();
            TRACE("tcp connected %s - %s fd %d", localAddr_.ToString().c_str(), peerAddr_.ToString().c_str(), channel_->GetFd());
            if (statecb_) { statecb_;}
        }
    } else {
        TRACE("poll fd %d return %d revents %d", channel_->GetFd(), r, pfd.revents);
        CleanUp(con);
        return -1;
    }
    return 0;
}

void TcpConn::HandleWrite(const TcpConnPtr& con) {
    if (state_ == State::kHandShakeing) {
        HandleHandShake(con);
    } else if (state_ == State::kConnected) {
        ssize_t sended = ISend(output_.Begin(), output_.GetSize());
        output_.Consume(sended);
        if (output_.IsEmpty() && channel_->IsWriting()) {
           channel_->DisableWrite();
        }
    } else {
        ERROR("handle write unexpected");
    }
}

ssize_t TcpConn::ISend(const char* buf, size_t len) {
    size_t sended = 0;
    while (len > sended) {
        ssize_t wd = write(channel_->GetFd(), buf + sended, len - sended);
        TRACE("channel %lld fd %d write %lld bytes", (long long)channel_->GetId(), channel_->GetFd(), wd);
        if (wd > 0) {
            sended += wd;
            continue;
        } else if(wd == -1 && errno == EINTR) { // 慢速系统调用，信号打断
            continue;
        } else if (wd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { // 非阻塞fd
            if (!channel_-> IsWriting()) {
                channel_->EnableWrite();
            }
            break;
        } else {
            ERROR("write error: channel %lld fd %d wd %ld %ld %s", channel_->GetId(), channel_->GetFd(), wd, errno, strerror(errno));
            break;
        }
    }
    return sended;
}

void TcpConn::Send(Buffer& message) {
    if (channel_) {
        // 如果channel开启了写，则将message直接添加到output中
        if (channel_->IsWriting()) {
            output_.Absorb(message);
        } else if (message.GetSize()) {
            ssize_t sended = ISend(message.Begin(), message.GetSize());
            message.Consume(sended);
            if (message.GetSize()) {
                output_.Absorb(message);
                if (!channel_->IsWriting()) channel_->EnableWrite();
            }
        }
    } else {
        WARN("connection %s - %s closed, but still writing %lu bytes", localAddr_.ToString().c_str(), peerAddr_.ToString(), message.GetSize());
    }
}

void TcpConn::Send(const char* message, size_t len) {
    if (channel_) {
        if(output_.IsEmpty()) {
            ssize_t sended = ISend(message, len);
            message += sended;
            len -= sended;
        }
        if (len) {
            output_.Append(message, len);
        }
    } else {
        WARN("connection %s - %s closed, but still writing %lu bytes", localAddr_.ToString().c_str(), peerAddr_.ToString(), len);
    }
}

void TcpConn::OnMsg(CodecBase* codec, const MsgCallBack& cb) {
    assert(!readcb_);
    codec_.reset(codec);
    OnRead([cb](const TcpConnPtr& con) {
        int r = 1;
        while (r) {
            std::string msg;
            r = con->codec_->TryDecode(con->GetInput().GetData(), msg);

            if (r < 0) {
                con->channel_->Close();
            } else {
                cb(con, msg);
                con->GetInput().Consume(r);
            }
        }
    });
}

void TcpConn::SendMsg(std::string msg) {
    Buffer& b = GetOutput();
    codec_->Encode(msg, b);
    SendOutput();
}
