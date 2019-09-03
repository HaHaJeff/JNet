#include "raft.h"
#include "callback.h"
#include "raft_peer.h"
#include "storage.h"

namespace jraft
{

Raft::Raft(const Config &config) : storage_(new Storage(config.path_)),
                                   random_(0, 0, 0),
                                   logs_(storage_)
{
}

void Raft::AppendEntries(const AppendEntriesRequest &request, AppendEntriesResponse &reply)
{
}

void Raft::OnAppendEntries(const AppendEntriesRequest &request, const AppendEntriesResponse &reply)
{
}

void Raft::RequestVote(const RequestVoteRequest &request, RequestVoteResponse &reply)
{
    if (role_ != kCandidater)
    {
        INFO("node was not candidater, but has requested vote");
        return;
    }

    reply.set_term(currentTerm_);
    // reply false if request.term < currentTerm
    // if votedFor is null or candidateId && 
    // candidate's log is at least as up-to-date as receive's log
    // gran vote
    if (request.term() < currentTerm_ &&
        (votedFor_ == -1 || votedFor_ == request.peerid()) && 
        NewestLog(request))
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

void Raft::OnRequestVote(const RequestVoteRequest &request, const RequestVoteResponse &reply)
{
    // other's term is greater than currentterm,
    // so change role to follower
    if (reply.term() > currentTerm_)
    {
        ToFollower();
    }
}

void Raft::PreVote(const RequestVoteRequest &request, RequestVoteResponse &reply)
{
}

void Raft::OnPreVote(const RequestVoteRequest &request, const RequestVoteResponse &reply)
{
}

void Raft::StartRequestVote()
{
    RequestVoteRequest request;
}

void Raft::Propose(const std::string &cmd)
{
}

void Raft::ToFollower()
{
    role_ = kFollower;
}

void Raft::ToCandidater()
{
    role_ = kCandidater;
    // increment currentTerm
    SetCurrentTerm(++currentTerm_);
    // voted for self
    SetVotedFor(id_);
    // increment vote count
    voted_ = 1;
    // reset election time
    ResetElectionTime();
    // send requestVoteRpc to all others servers
    StartRequestVote();
}

void Raft::ToLeader()
{
    role_ = kLeader;

}

void Raft::Tick()
{
}

void Raft::TickOnHeartBeat()
{
}

void Raft::TickOnElection()
{
}

void Raft::ResetElectionTime()
{
    electionTimeout_ = 0;
}

void Raft::ResetHeartbeatTime()
{
    heartbeatTimeout_ = 0;
}

bool Raft::NewestLog(const RequestVoteRequest &request)
{
    return true;
}

void Raft::SetCurrentTerm(int64_t term)
{

}

void Raft::SetVotedFor(int64_t id)
{

}

} // namespace jraft
