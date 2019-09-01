#include "raft.h"
#include "callback.h"
#include "raft_peer.h"

namespace jraft{

Raft::Raft(const Config& config) : storage_(config.path_), random_(0, 0, 0) {
}

void Raft::AppendEntries(const AppendEntriesRequest& request, AppendEntriesResponse& reply) {
    
}

void Raft::OnAppendEntries(const AppendEntriesRequest& request, const AppendEntriesResponse& reply) {

}

void Raft::RequestVote(const RequestVoteRequest& request, RequestVoteResponse& reply) {
    if (role_ != kCandidate) {
        jnet::INFO("node was not candidater, but has requested vote");
        return;
    }
    
    reply.set_term(persistState_.currentTerm());
    if (request.term < persistState_.currentTerm() || persistState_.votedFor == -1 || NewestLog(request))
    {
        reply.set_voteGranted(true);
    } 
    else
    {
        reply.set_voteGranted(false);
    }
}

void Raft::OnRequestVote(const RequestVoteRequest& request, const RequestVoteResponse& reply) {
    
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

/*
void Raft::Persist() {
    std::string input;
    persistState_.SerializeToString(&input);
    storage_.SavePersist(input);
}

void Raft::ReadPersist() {
    std::string info = storage_.ReadPersist();
    persistState_.ParseFromString(info);
}
*/

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

}
