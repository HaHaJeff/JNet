#ifndef RAFTSERVICE_H
#define RAFTSERVICE_H

#include "raft.pb.h"
#include "callback.h"

namespace jraft
{

class RaftServiceImpl : public RaftService
{
public:
    //
    // override google protobuf
    //
    virtual void PreVote(::google::protobuf::RpcController *controller,
                         const ::jraft::RequestVoteRequest *request,
                         ::jraft::RequestVoteResponse *response,
                         ::google::protobuf::Closure *done);

    virtual void RequestVote(::google::protobuf::RpcController *controller,
                             const ::jraft::RequestVoteRequest *request,
                             ::jraft::RequestVoteResponse *response,
                             ::google::protobuf::Closure *done);

    virtual void AppendEntries(::google::protobuf::RpcController *controller,
                               const ::jraft::AppendEntriesRequest *request,
                               ::jraft::AppendEntriesResponse *response,
                               ::google::protobuf::Closure *done);

    void SetPreVote(const PreVoteCallback &prevote) { preVote_ = prevote; }

    void SetRequstVote(const RequestVoteCallback &requestVote) { requestVote_ = requestVote; }

    void SetAppendEntries(const AppendEntriesCallback &appendEntries) { appendEntries_ = appendEntries; }

private:
    PreVoteCallback preVote_;
    RequestVoteCallback requestVote_;
    AppendEntriesCallback appendEntries_;
};

} // namespace jraft

#endif
