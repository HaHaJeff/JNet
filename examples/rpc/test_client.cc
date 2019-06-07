#include "echo.pb.h"
#include "tcpconn.h"
#include "rpc_channel.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <string>
using namespace jrpc;
using namespace std::placeholders;
using namespace jnet;

class RpcClient {
public:
    RpcClient(EventLoop* loop, const Ip4Addr& local, const Ip4Addr& peer) : conn_(nullptr), channel_(new RpcChannel), stub_(channel_.get()){
        conn_ = TcpConn::CreateConnection(loop, local, peer);
        conn_->OnRead(std::bind(&RpcChannel::OnMessage, channel_.get(), _1));
        channel_->SetConn(conn_);
    }

    void Echo(const std::string& msg) {
        ::echo::EchoResponse *response = new ::echo::EchoResponse;
        ::echo::EchoRequest   request;
        request.set_msg(msg);
        std::cout << "Echo" << std::endl;
        stub_.Echo(nullptr, &request, response, NewCallback(this, &RpcClient::Done, response));
    }

    void Done(::echo::EchoResponse* resp) {
        std::cout << resp->msg() << std::endl;
    }
    
private:
    TcpConnPtr conn_;
    RpcChannelPtr channel_;
    ::echo::EchoService::Stub stub_;
};

int main()
{
    SETLOGLEVEL("FATAL");
    EventLoop loop;
    
    Ip4Addr local("127.0.0.1", 8888);
    Ip4Addr peer("127.0.0.1", 9999);
    RpcClient client(&loop, local, peer);
    loop.RunAfter(1, [&](){client.Echo("hello JRpc");});
    loop.Loop();
}
