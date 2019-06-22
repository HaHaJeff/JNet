#include "raft_peer.h"

namespace jraft {

void RaftPeer::PreVote(const RequestVoteRequest& request, RequestVoteResponse& reply) {
    std::cout << "Not Implented" << std::endl;
}

void RaftPeer::RequestVote(const RequestVoteRequest& request, RequestVoteResponse& reply) {
    std::cout << "Not Implented" << std::endl;
}

void RaftPeer::AppendEntries(const AppendEntriesRequest& request, AppendEntriesResponse& reply) {
    std::cout << "Not Implented" << std::endl;
}
}
