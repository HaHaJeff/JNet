#include "node.h"
#include "raft_peer.h"
#include <functional>
using namespace jrpc;
using namespace jnet;
namespace jraft
{
Node::Node(const Config &config, jnet::EventLoop *loop) : loop_(loop),
                                                          server_(loop, config.serverAddress_)
{
    // init raft peers, set their's callbacks
    for (int i = 0; i < config.peerAddresses_.size(); i++)
    {
        peers_.push_back(new RaftPeer(loop, config.serverAddress_, config.peerAddresses_[i]));
        // set rpc client reply method
        peers_[i]->SetOnPreVote(std::bind(&jraft::Node::OnPreVote, this, std::placeholders::_1, std::placeholders::_2));
        peers_[i]->SetOnRequestVote(std::bind(&jraft::Node::OnRequestVote, this, std::placeholders::_1, std::placeholders::_2));
        peers_[i]->SetOnAppendEntries(std::bind(&jraft::Node::OnAppendEntries, this, std::placeholders::_1, std::placeholders::_2));
    }
    // set rpc server hanlde method
    service_.SetPreVote(std::bind(&jraft::Node::PreVote, this, std::placeholders::_1, std::placeholders::_2));
    service_.SetRequstVote(std::bind(&jraft::Node::RequestVote, this, std::placeholders::_1, std::placeholders::_2));
    service_.SetAppendEntries(std::bind(&jraft::Node::AppendEntries, this, std::placeholders::_1, std::placeholders::_2));

    server_.RegisterService(&service_);
    raft_ = new Raft(config, peers_);
}

Node::~Node()
{
    INFO("exit raft");
    delete raft_;
    delete loop_;
}

void Node::Propose(const std::string &str)
{
    raft_->Propose(str);
}

void Node::RequestVote(const RequestVoteRequest& request, RequestVoteResponse* reply)
{
    loop_->RunInLoop([=](){raft_->RequestVote(request, *reply);});
}

void Node::OnRequestVote(const RequestVoteRequest& request, const RequestVoteResponse& reply)
{
    raft_->OnRequestVote(request, reply);
}

void Node::PreVote(const RequestVoteRequest& request, RequestVoteResponse* reply)
{

}

void Node::OnPreVote(const RequestVoteRequest& request, const RequestVoteResponse& reply)
{

}

void Node::AppendEntries(const AppendEntriesRequest& request, AppendEntriesResponse* reply)
{

}

void Node::OnAppendEntries(const AppendEntriesRequest& request, const AppendEntriesResponse& reply)
{

}

void Node::Start()
{
    server_.Start();
    for (auto& peer : peers_) peer->Connect();
}


} // namespace jraft