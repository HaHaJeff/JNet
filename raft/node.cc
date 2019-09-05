#include "node.h"
#include <functional>
using namespace jrpc;
using namespace jnet;
namespace jraft
{
Node::Node(const Config &config, jnet::EventLoop *loop) : loop_(loop),
                                                          server_(loop, config.serverAddress)
{
    // loop_->RunEvery(something);

    server_.RegisterService(&service_);
    // init raft peers, set their's callbacks
    service_.SetPreVote(std::bind(&jraft::Node::PreVote, this, std::placeholders::_1, std::placeholders::_2));
    service_.SetRequstVote(std::bind(&jraft::Node::RequestVote, this, std::placeholders::_1, std::placeholders::_2));
    service_.SetAppendEntries(std::bind(&jraft::Node::AppendEntries, this, std::placeholders::_1, std::placeholders::_2));
}

void Node::Propose(const std::string &str)
{
    raft_->StartRequestVote();
}

void Node::RequestVote(const RequestVoteRequest& request, RequestVoteResponse& reply)
{

}

void Node::OnRequestVote(const RequestVoteRequest& request, const RequestVoteResponse& reply)
{

}

void Node::PreVote(const RequestVoteRequest& request, RequestVoteResponse& reply)
{

}

void Node::OnPreVote(const RequestVoteRequest& request, const RequestVoteResponse& reply)
{

}

void Node::AppendEntries(const AppendEntriesRequest& request, AppendEntriesResponse& reply)
{

}

void Node::OnAppendEntries(const AppendEntriesRequest& request, const AppendEntriesResponse& reply)
{

}

} // namespace jraft