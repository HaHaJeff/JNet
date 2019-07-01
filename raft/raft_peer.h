#include "raft.pb.h"
#include "tcpconn.h"
#include "rpc_channel.h"
#include "raft.pb.h"
#include "callback.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

using namespace std::placeholders;
using namespace jrpc;
using namespace jnet;

namespace jraft {

class RaftPeer {
public:
    RaftPeer(EventLoop* loop, const Ip4Addr& local, const Ip4Addr& peer) 
        : conn_(nullptr),
          channel_(new RpcChannel),
          stub_(channel_.get())
    {
        conn_ = TcpConn::CreateConnection(loop, local, peer);
        conn_->OnRead(std::bind(&RpcChannel::OnMessage, channel_.get(), _1));
        channel_->SetConn(conn_);
    }

    void SetOnPreVote(const OnPreVoteCallback& onPreVote) { onPreVote_ = onPreVote;}
    void SetOnRequestVote(const OnRequestVoteCallback& onRequestVote) { onRequestVote_ = onRequestVote;}
    void SetOnAppendEntries(const OnAppendEntriesCallback& onAppendEntries) { onAppendEntries_ = onAppendEntries;}

    void PreVote(const RequestVoteRequest&, RequestVoteResponse&);
    void RequestVote(const RequestVoteRequest&, RequestVoteResponse&);
    void AppendEntries(const AppendEntriesRequest&, AppendEntriesResponse&);

    //
    // type match 
    //
    void OnPreVote(const RequestVoteRequest* request, RequestVoteResponse* response) { onPreVote_(*request, *response); }
    void OnRequestVote(const RequestVoteRequest* request, RequestVoteResponse* response) { onRequestVote_(*request, *response); }
    void OnAppendEntries(const AppendEntriesRequest* request, AppendEntriesResponse* response) { onAppendEntries_(*request, *response); }

private:
    TcpConnPtr conn_;
    RpcChannelPtr channel_;
    RaftService::Stub stub_;

    OnPreVoteCallback onPreVote_;
    OnRequestVoteCallback onRequestVote_;
    OnAppendEntriesCallback onAppendEntries_;
};
}
