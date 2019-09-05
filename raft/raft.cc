#include "raft.h"
#include "callback.h"
#include "raft_peer.h"
#include "storage.h"

namespace jraft
{

Raft::Raft(const Config &config, const std::vector<RaftPeer *> &peers) : storage_(new Storage(config.path_)),
                                                                         currentTerm_(0),
                                                                         votedFor_(-1),
                                                                         logs_(storage_),
                                                                         commitIndex_(0),
                                                                         lastApplied_(0),
                                                                         peers_(peers),
                                                                         random_(0, 0, 0)
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

    reply.set_peerid(id_);
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
        votedFor_ = request.peerid();
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
    if (role_ != kCandidater || reply.term() > currentTerm_ || reply.votegranted() == false)
    {
        return;
    }
    if (reply.votegranted() == false)
    {
        return;
    }

    INFO("node %lld received RequestVoteResponse from node %lld, response term = %lld, currentTerm = %lld, granted = %d",
          id_, reply.peerid(), reply.term(), currentTerm_, reply.votegranted());
    ++voted_;

    if (Majority()) 
    {
        ToLeader();
    }
}

void Raft::PreVote(const RequestVoteRequest &request, RequestVoteResponse &reply)
{
}

void Raft::OnPreVote(const RequestVoteRequest &request, const RequestVoteResponse &reply)
{
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

bool Raft::Majority() const
{
    return voted_ > (peers_.size() + 1) / 2;
}

void Raft::SetCurrentTerm(int64_t term)
{
}

void Raft::SetVotedFor(int64_t id)
{
}

} // namespace jraft
