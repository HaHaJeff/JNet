#include "raft_service.h"

namespace jraft{

void RaftServiceImpl::PreVote(::google::protobuf::RpcController* controller,
                       const ::jraft::RequestVoteRequest* request,
                       ::jraft::RequestVoteResponse* response,
                       ::google::protobuf::Closure* done)
{
    preVote_(*request, *response);
    done->Run();
}

void RaftServiceImpl::RequestVote(::google::protobuf::RpcController* controller,
                       const ::jraft::RequestVoteRequest* request,
                       ::jraft::RequestVoteResponse* response,
                       ::google::protobuf::Closure* done)
{
    requestVote_(*request, *response);
    done->Run();
}

void RaftServiceImpl::AppendEntries(::google::protobuf::RpcController* controller,
                       const ::jraft::AppendEntriesRequest* request,
                       ::jraft::AppendEntriesResponse* response,
                       ::google::protobuf::Closure* done)
{
    appendEntries_(*request, *response);
    done->Run();
}

}