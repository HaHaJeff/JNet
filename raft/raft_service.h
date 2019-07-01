#ifndef RAFTSERVICE_H
#define RAFTSERVICE_H

#include "raft/raft.pb.h"
#include "raft/callback.h"

namespace jraft {

class RaftServiceImpl : public RaftService {
public:
    //
    // override google protobuf
    //
    virtual void PreVote(::google::protobuf::RpcController* controller,
                       const ::jraft::RequestVoteRequest* request,
                       ::jraft::RequestVoteResponse* response,
                       ::google::protobuf::Closure* done);

    virtual void RequestVote(::google::protobuf::RpcController* controller,
                       const ::jraft::RequestVoteRequest* request,
                       ::jraft::RequestVoteResponse* response,
                       ::google::protobuf::Closure* done);

    virtual void AppendEntries(::google::protobuf::RpcController* controller,
                       const ::jraft::AppendEntriesRequest* request,
                       ::jraft::AppendEntriesResponse* response,
                       ::google::protobuf::Closure* done);
private:
    PreVoteCallback preVote_;
    RequestVoteCallback requestVote_;
    AppendEntriesCallback appendEntries_;
};

}

#endif