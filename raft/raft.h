#ifndef RAFT_H
#define RAFT_H

#include <vector>
#include <stdint.h>
#include "config.h"
#include "random.h"
#include "raft_log.h"
#include "state.pb.h"

namespace jraft {

class RaftPeer;
class AppendEntriesRequest;
class AppendEntriesResponse;
class RequestVoteRequest;
class RequestVoteResponse;
class Storage;

class Raft {
public:

    Raft(const Config&, const std::vector<RaftPeer*>& peers);

    //
    // Call this function on every node
    // if the raft instance can append this entries, reply true
    //
    void AppendEntries(const AppendEntriesRequest&, AppendEntriesResponse&);
    
    //
    // Call this function on leader
    // if the this request has got voted in a majority qurom, incr commitedindex and update some infomation
    //
    void OnAppendEntries(AppendEntriesRequest&, AppendEntriesResponse&);

    //
    // Call this function on every node
    //
    void RequestVote(const RequestVoteRequest&, RequestVoteResponse&);

    //
    // Call this function on leader
    //
    void OnRequestVote(RequestVoteRequest&, RequestVoteResponse&);

    //
    // Call this function on every node
    //
    void PreVote(const RequestVoteRequest&, RequestVoteResponse&);

    //
    // Call this function on leader
    //
    void OnPreVote(RequestVoteRequest&, RequestVoteResponse&);

    void Propose(const std::string& cmd);

    void StartPrevote();
    void StartRequestVote();
    void StartAppendEntries(const std::string& cmd);

private:
    void ToFollower();
    void ToCandidater();
    void ToLeader();
    void Tick();
    void TickOnHeartBeat();
    void TickOnElection();
    void ResetTimer();

    void ResetElectionTime();
    void ResetHeartbeatTime();

    bool NewestLog(const RequestVoteRequest& request);
    bool Majority() const;
    void SetCurrentTerm(int64_t term);
    void SetVotedFor(int64_t id);

    int64_t heartbeatTimeout_;
    int64_t electionTimeout_;
    int64_t randomizedElectionTimeout_;
    int64_t elpasedTime_;
    const int hz = 100;

private:
    // 
    // State identifier
    //
    enum Role {
        kLeader,
        kCandidater,
        kFollower,
    } role_;

    //
    // For persist
    //
    Storage* storage_;

    //
    // Updated on stable storage before responding to RPCs
    //
    // use protobuf repeated field
    // PersistState persistState_;
    int64_t currentTerm_;
    int64_t votedFor_; 
    RaftLog logs_;
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
    // peer raft instance
    // 
    std::vector<RaftPeer*> peers_;

    // 
    // Extra infomation exclude raft paper
    //
    int64_t id_;
    int64_t voted_;

    //
    // for randomize electiontimeout
    //
    Random random_;
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
    std::vector<RaftPeer*> peers_;
};

}


#endif
