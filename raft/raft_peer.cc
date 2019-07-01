#include "raft_peer.h"

namespace jraft {

void RaftPeer::PreVote(const RequestVoteRequest& request) {
    std::cout << "Not Implented" << std::endl;
}

void RaftPeer::RequestVote(const RequestVoteRequest& request) {
    std::cout << "Not Implented" << std::endl;
}

void RaftPeer::AppendEntries(const AppendEntriesRequest& request) {
    std::cout << "Not Implented" << std::endl;
}

}
