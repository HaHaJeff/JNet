# service
protobuf rpc service的抽象接口，有client的stub以及server负责实现
## client stub
实现service_stub类，涉及到rpcchannel(rpcchannel负责网络通信, 组成对象包括：TcpConn)
## server
管理具体的services_，一般采用map实现，key为string，value为services

### rpcchannel
- 在client处理response消息
- 在server处理request消息
