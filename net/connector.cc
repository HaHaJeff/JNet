#include "connector.h"
#include "channel.h"
#include "eventloop.h"
#include "net/log.h"

#include <assert.h>

namespace jnet {

Connector::Connector(EventLoop* loop, const Ip4Addr& serverAddr)
    : loop_(loop), 
      serverAddr_(serverAddr),
      isConnected_(false),
      state_(kDisconnected),
      retryDelayMs_(kInitRetryDelayMs)
{ }

Connector::~Connector() {
}

void Connector::Start() {
    isConnected_ = true;
    loop_->RunInLoop(std::bind(&Connector::StartInLoop, this));
}

void Connector::StartInLoop() {
    loop_->AssertInLoopThread();
    assert(state_ == kDisconnected);
    if (isConnected_) {
        Connect();
    } 
}

void Connector::Stop() {
    isConnected_ = false;
    loop_->RunInLoop(std::bind(&Connector::StopInLoop, this));
}

void Connector::StopInLoop() {
    loop_->AssertInLoopThread();

    if (state_ == kConnecting) {

    }
} 

void Connector::Connect() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    Net::SetNonBlock(sockfd);
    Net::Connect(sockfd, sockaddr_cast(&serverAddr_.GetAddr()));
}
}
