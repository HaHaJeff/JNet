#include "node.h"

namespace jraft {
Node::Node(const Config& config, jnet::EventLoop* loop) : raft_(config),
                                                          loop_(loop),
                                                          server_(loop, config.serverAddress){
    // loop_->RunEvery(something);

    server_.RegisterService(&service_);
}

void Node::Propose(const std::string& str) {
    raft_.StartRequestVote();
}

}