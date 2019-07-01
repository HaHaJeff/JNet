#ifndef RAFT_SERVICE_H
#define RAFT_SERVICE_H

namespace jraft {

class RaftServiceImpl : public RaftService {
public:
    RaftService();
    virtual void PreVote(::google::protobuf::RpcController* controller,
                         const RequestVoteRequest* request,
                         RequestVoteResponse* response,
                         ::google::protobuf::Closure* done);
     
    virtual void RequestVote(::google::protobuf::RpcController* controller,
                         const RequestVoteRequest* request,
                         RequestVoteResponse* response,
                         ::google::protobuf::Closure* done);

    virtual void AppendEntries(::google::protobuf::RpcController* controller,
                         const AppendEntriesRequest* request,
                         AppendEntriesResponse* response,
                         ::google::protobuf::Closure* done);
     
};
}

#endif
