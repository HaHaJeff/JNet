#ifndef RAFT_H
#define RAFT_H

#include <vector>
#include <stdint.h>
#include "config.h"
#include "random.h"
#include "raft/log.h"
#include "raft/state.pb.h"

namespace jraft {

class RaftPeer;
class AppendEntriesRequest;
class AppendEntriesReply;
class RequestVoteRequest;
class RequestVoteReply;

class Raft {
public:

    Raft(const Config&);

    //
    // Call this function on every node
    // if the raft instance can append this entries, reply true
    //
    void AppendEntries(const AppendEntriesRequest&, AppendEntriesReply&);
    
    //
    // Call this function on leader
    // if the this request has got voted in a majority qurom, incr commitedindex and update some infomation
    //
    void OnAppendEntries(int, const AppendEntriesRequest&, const AppendEntriesReply&);

    //
    // Call this function on every node
    //
    void RequestVote(const RequestVoteRequest&, RequestVoteReply&);

    //
    // Call this function on leader
    //
    void OnRequestVote(int, const RequestVoteRequest&, const RequestVoteReply&);

    //
    // Call this function on every node
    // Updated on stable storage before responding tp RPCS
    // currentTerm: latest term server has seen(initialized to 0 on first boot, increase monotonically)
    // votedFor: candidated that received vote in current term (or null if none)
    // log[]: log entries, each entry contains command for state macgine, and term when entry was received by leader(first index is 1)
    //
    void Persist();
    void ReadPersist();

private:
    void ToFollower();
    void ToCandidater();
    void ToLeader();
    void Tick();
    void TickOnHeartBeat();
    void TickOnElection();

    int64_t heartbeatTimeout_;
    int64_t electionTimeout_;
    int64_t randomizedElectionTimeout_;
    Random random_;

private:
    // 
    // State identifier
    //
    enum Role {
        kLeader,
        kCandidate,
        kFollower,
    };

    //
    // For persist
    //
    Storage storage_;

    //
    // Updated on stable storage before responding to RPCs
    //
    // use protobuf repeated field
    PersistState persistState_;
    // int64_t term_;
    // int64_t votedFor_;
    // std::vector<LogEntry> logs_;

    // 
    // Volatile state on all servers
    //
    int64_t commitIndex_;
    int64_t lastApplied_;

    //
    // Volatile state on leders
    //
    std::vector<int64_t> nextIndex_;
    std::vector<int64_t> matchIndex_;

    // 
    // Extra infomation exclude raft paper
    //
    int64_t id_;
    int64_t voted_;

    //Persister persister_;
};

struct RaftState {
    //
    // Updated on stable storage before responding to RPCs
    //
    int64_t term_;
    int64_t votedFor_;
    std::vector<LogEntry> logs_;

    // 
    // Volatile state on all servers
    //
    int64_t commitIndex_;
    int64_t lastApplied_;

    //
    // Volatile state on leders
    //
    std::vector<int64_t> nextIndex_;
    std::vector<int64_t> matchIndex_;

    // 
    // Extra infomation exclude raft paper
    //
    int64_t id_;
    int64_t voted_;

    //
    // peer rpc server
    //
    std::vector<RaftPeer*>& peers_;
};

}


#endif
