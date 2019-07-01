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
