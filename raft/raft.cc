#include "raft.h"
#include "callback.h"
#include "raft_peer.h"
#include "storage.h"

namespace jraft
{

EntryType Parse(const std::string& cmd)
{
    return ENTRY_TYPE_DATA;
}

Raft::Raft(const Config &config, const std::vector<RaftPeer *> &peers) : heartbeatTimeout_(0),
                                                                         electionTimeout_(0),
                                                                         randomizedElectionTimeout_(0),
                                                                         elpasedTime_(0),
                                                                         storage_(new Storage(config.path_)),
                                                                         currentTerm_(0),
                                                                         votedFor_(-1),
                                                                         logs_(storage_),
                                                                         commitIndex_(0),
                                                                         lastApplied_(0),
                                                                         peers_(peers),
                                                                         id_(config.id_),
                                                                         random_(0, 0, 0)
{
    randomizedElectionTimeout_ = random_.Generate();
}

void Raft::AppendEntries(const AppendEntriesRequest &request, AppendEntriesResponse &reply)
{
    INFO("node %lld received AppendEntries request from node %lld, term = %lld, \
          prevLogIndex = %lld, prevLogTerm = %lld, comittedIndex = %lld",
         id_, request.peerid(), request.term(), request.prevlogindex(),
         request.prevlogterm(), request.commitedindex());

    // for leader to update itself
    reply.set_term(currentTerm_);
    reply.set_peerid(id_);

    int64_t insertIndex = request.prevlogindex()+1;
    int64_t term = request.term();
    // reply false if term < currentTerm
    // reply false if log doesn't contain an entry at prevLogIndex whose
    // term matches prevLogTerm
    if (term < currentTerm_ ||
        !logs_.ContainLog(request.prevlogindex(), request.prevlogterm()))
    {
        reply.set_success(false);
        return;
    }

    // if an existing entry conflicts with a new one(same index but different
    // terms), delete the existing entry and all that follow it
    if (!logs_.ContainLog(insertIndex, term))
    {
        logs_.Truncate(insertIndex);
    }

    // append any new entries not already in the log
    // if leaderCommit > commitIndex, set commitIndex = min(leaderCommit,
    // index of last new entry)
    LogEntry entry;
    auto entries = request.entries();
    for (int i = 0; i < entries.size(); i++)
    {
        entry.set_type(request.type(i));
        entry.set_index(insertIndex);
        entry.set_term(term);
        entry.set_command(request.entries(i));
        logs_.Append(entry);
    }
    if (request.commitedindex() > commitIndex_)
    {
        commitIndex_ = std::min(request.commitedindex(), insertIndex);
    }
}

void Raft::OnAppendEntries(AppendEntriesRequest &request, AppendEntriesResponse &reply)
{
    INFO("node %lld received AppendEntries response from node %lld, term = %lld,\
          success = %d", id_, reply.peerid(), reply.term(), reply.success());
    int64_t peerid = reply.peerid();
    if (request.term() > currentTerm_)
    {
        return;
    }
    if (reply.success() == false)
    {
        --nextIndex_[peerid];
        request.set_prevlogindex(nextIndex_[peerid]-1);
        request.set_prevlogterm(logs_.Term(request.prevlogindex()));
        peers_[peerid]->AppendEntries(request, &reply);
        AppendEntries(request, reply);
        return;
    }
    matchIndex_[peerid] = nextIndex_[peerid]++;
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

void Raft::OnRequestVote(RequestVoteRequest &request, RequestVoteResponse &reply)
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

void Raft::OnPreVote(RequestVoteRequest &request, RequestVoteResponse &reply)
{
}

void Raft::Propose(const std::string &cmd)
{
    ToCandidater();
}

void Raft::StartPrevote()
{
    INFO("not implement prevote");
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
    request.set_type(0, Parse(cmd));
    for (int i = 0; i < peers_.size(); i++)
    {
        request.set_prevlogindex(nextIndex_[i]-1);
        request.set_prevlogterm(logs_.Term(request.prevlogindex()));
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

void Raft::ResetTimer()
{
    elpasedTime_ = 0;
    randomizedElectionTimeout_ = random_.Generate();
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
