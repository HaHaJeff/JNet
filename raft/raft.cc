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
                                                                         id_(config.id_),
                                                                         random_(0, 0, 0)
{
}

void Raft::AppendEntries(const AppendEntriesRequest &request, AppendEntriesResponse &reply)
{
    INFO("node %lld received AppendEntries from node %lld, term = %lld, \
          prevLogIndex = %lld, prevLogTerm = %lld, comittedIndex = %lld",
         id_, request.peerid(), request.term(), request.prevlogindex(),
         request.prevlogterm(), request.commitedindex());

    // reply false if term < currentTerm
    // reply false if log doesn't contain an entry at prevLogIndex whose
    // term matches prevLogTerm
    // if an existing entry conflicts with a new one(same index but different
    // terms), delete the existing entry and all that follow it
    // append any new entries not already in the log
    // if leaderCommit > commitIndex, set commitIndex = min(leaderCommit,
    // index of last new entry)
    reply.set_term(currentTerm_);
    reply.set_peerid(id_);
    if (request.term() < currentTerm_ ||
        !logs_.ContainLog(request.prevlogindex(), request.prevlogterm()))
    {
        logs_.Truncate(request.prevlogindex());
        reply.set_success(false);
        return;
    }
}

void Raft::OnAppendEntries(const AppendEntriesRequest &request, const AppendEntriesResponse &reply)
{
}

void Raft::RequestVote(const RequestVoteRequest &request, RequestVoteResponse &reply)
{
    // raft when don't allow node participates requestVote except it's candidater
    // if (role_ != kCandidater)
    // {
    //     INFO("node was not candidater, but has requested vote");
    //     return;
    // }
    INFO("node %lld received RequestVote from node %lld, term = %lld, currentTerm = %lld",
         id_, request.peerid(), request.term(), currentTerm_);
    reply.set_peerid(id_);
    reply.set_term(currentTerm_);

    // reply false if request.term < currentTerm
    // if votedFor is null or candidateId &&
    // candidate's log is at least as up-to-date as receive's log
    // gran vote
    if (request.term() > currentTerm_ &&
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
    INFO("node %lld received RequestVoteResponse from node %lld, response term = %lld, currentTerm = %lld, granted = %d",
         id_, reply.peerid(), reply.term(), currentTerm_, reply.votegranted());
    // prevent expired vote
    if (role_ != kCandidater || reply.votegranted() == false || currentTerm_ > reply.term())
    {
        return;
    }
    if (reply.votegranted() == false)
    {
        return;
    }

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
    ToCandidater();
}

void Raft::StartPrevote()
{

}

void Raft::StartRequestVote()
{
    RequestVoteRequest request;
    request.set_term(currentTerm_);
    request.set_peerid(id_);
    request.set_candidateid(id_);
    // set last log index;
    // set last log term
    request.set_lastlogindex(0);
    request.set_lastlogterm(0);
    for (int i = 0; i < peers_.size(); i++)
    {
        RequestVoteResponse *response = new RequestVoteResponse;
        peers_[i]->RequestVote(request, response);
    }
}

void Raft::StartAppendEntries(const std::string &cmd)
{
    if (role_ != kLeader)
    {
        INFO("node %lld start log replica but not leader", id_);
        return;
    }

    AppendEntriesRequest request;
    AppendEntriesResponse *response = new AppendEntriesResponse;
    request.set_term(currentTerm_);
    request.set_peerid(id_);
    request.set_entries(0, cmd);
    request.set_commitedindex(commitIndex_);
    for (int i = 0; i < peers_.size(); i++)
    {
        request.set_prevlogindex(nextIndex_[i]-1);
        request.set_prevlogterm(request.prevlogindex());
        peers_[i]->AppendEntries(request, response);
    }
}

void Raft::ToFollower()
{
    role_ = kFollower;
    // clear some info of candidater
    voted_ = 0;
    votedFor_ = -1;
}

void Raft::ToCandidater()
{
    INFO("node %lld to candidater", id_)
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
    INFO("node %lld become leader", id_);
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
    int64_t index = request.lastlogindex();
    int64_t term = request.lastlogterm();
    int64_t newestIndex = logs_.LastIndex();
    int64_t newestTerm = logs_.Term(newestIndex);
    if (term != newestTerm)
        return newestTerm < term;
    return newestIndex < index;
}

bool Raft::Majority() const
{
    return voted_ > (peers_.size() + 1) / 2;
}

void Raft::SetCurrentTerm(int64_t term)
{
    currentTerm_ = term;
}

void Raft::SetVotedFor(int64_t id)
{
    votedFor_ = id;
}

} // namespace jraft
