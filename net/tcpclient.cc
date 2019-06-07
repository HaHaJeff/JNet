#include "tcpclient.h"
#include "channel.h"

namespace jnet {

TcpClient::TcpClient(EventLoop* loop,
                     const Ip4Addr& local,
                     const Ip4Addr& peer)
    : loop_(loop),
      conn_(loop, local, peer)
{
    TRACE("TcpClient create!");
}

void TcpClient::Connect() {
}

void TcpClient::Disconnect() {

}

TcpClient::~TcpClient() {}
}
