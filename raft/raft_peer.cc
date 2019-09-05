#include "raft_peer.h"

namespace jraft {

void RaftPeer::PreVote(const RequestVoteRequest& request, RequestVoteResponse* reply) {
    std::cout << "Not Implented" << std::endl;
}

void RaftPeer::RequestVote(const RequestVoteRequest& request, RequestVoteResponse* reply) {
    ::google::protobuf::Closure* closure = ::google::protobuf::NewCallback(this, &RaftPeer::OnRequestVote, &request, reply);
    stub_.RequestVote(nullptr, &request, reply, closure);
}

void RaftPeer::AppendEntries(const AppendEntriesRequest& request, AppendEntriesResponse* reply) {
    std::cout << "Not Implented" << std::endl;
}
}
