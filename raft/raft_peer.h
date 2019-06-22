#include "raft.pb.h"
#include "tcpconn.h"
#include "rpc_channel.h"
#include "raft.pb.h"
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

    void PreVote(const RequestVoteRequest&, RequestVoteResponse&);
    void RequestVote(const RequestVoteRequest&, RequestVoteResponse&);
    void AppendEntries(const AppendEntriesRequest&, AppendEntriesResponse&);

private:
    TcpConnPtr conn_;
    RpcChannelPtr channel_;
    RaftService::Stub stub_;
};
}
