#include "raft.h"
#include "callback.h"
#include "raft_peer.h"
#include "storage.h"

namespace jraft{

Raft::Raft(const Config& config) : storage_(new Storage(config.path_)),
                                   random_(0, 0, 0) {
}

void Raft::AppendEntries(const AppendEntriesRequest& request, AppendEntriesResponse& reply) {
    
}

void Raft::OnAppendEntries(const AppendEntriesRequest& request, const AppendEntriesResponse& reply) {

}

void Raft::RequestVote(const RequestVoteRequest& request, RequestVoteResponse& reply) {
    if (role_ != kCandidate) {
        INFO("node was not candidater, but has requested vote");
        return;
    }
    
    reply.set_term(persistState_.currentterm());
    if (request.term() < persistState_.currentterm() || persistState_.votedfor() == -1 || NewestLog(request))
    {
        INFO("granted term %lld for peerid = %lld", request.term(), request.peerid());
        reply.set_votegranted(true);
    } 
    else
    {
        INFO("refused term %lld for peerid = %lld", request.term(), request.peerid());
        reply.set_votegranted(false);
    }
}

void Raft::OnRequestVote(const RequestVoteRequest& request, const RequestVoteResponse& reply) {
    // other's term is greater than currentterm,
    // so change role to follower
    if (reply.term() > persistState_.currentterm())
    {
        ToFollower();
    }


}

void Raft::PreVote(const RequestVoteRequest& request, RequestVoteResponse& reply) {

}

void Raft::OnPreVote(const RequestVoteRequest& request, const RequestVoteResponse& reply) {

}

void Raft::StartRequestVote() {
    RequestVoteRequest request;
}

void Raft::Propose(const std::string& cmd) {

}

void Raft::ToFollower() {

}

void Raft::ToCandidater() {
    
}

void Raft::ToLeader() {

}

void Raft::Tick() {

}

void Raft::TickOnHeartBeat() {

}

void Raft::TickOnElection() {

}

bool Raft::NewestLog(const RequestVoteRequest& request)
{
    return true;
}

}
