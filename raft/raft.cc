#include "raft.h"
#include "callback.h"
#include "raft_peer.h"

namespace jraft{

Raft::Raft(const Config& config) : storage_(config.path_), random_(0, 0, 0) {
}

void Raft::AppendEntries(const AppendEntriesRequest& request, AppendEntriesReply& reply) {
    
}

void Raft::OnAppendEntries(int peerId, const AppendEntriesRequest& request, const AppendEntriesReply& reply) {

}

void Raft::RequestVote(const RequestVoteRequest& request, RequestVoteReply& reply) {

}

void Raft::OnRequestVote(int peerId, const RequestVoteRequest& request, const RequestVoteReply& reply) {

}

void Raft::Persist() {
    std::string input;
    persistState_.SerializeToString(&input);
    storage_.SavePersist(input);
}

void Raft::ReadPersist() {
    std::string info = storage_.ReadPersist();
    persistState_.ParseFromString(info);
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

}
