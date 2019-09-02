# JNet

**一个基于C++11的Linux网络库**
- 使用epoll
- 优雅的实现功能定制
- 基于reactor模式

**新功能**
保证TcpConn的多线程安全，数据读写一定会发生在TcpConn对象的所属的IO线程
新增rpc功能，基于protobuf，增加ProtobufCodec RpcChannel RpcServer完成rpc功能

**测试**
- 与libevent2.0中的bench_http做对比，吞吐不相上下，每秒httpserver可以处理16000个http请求包
- 进行pingpong测试，采用本地回环地址，单线程单连接吞吐量145MB/s，单线程10连接可以达到420MB/s
- 进行rpc功能测试，完成echo测试

**raft**
- 使用protobuf定义了raft需要的消息结构，并且进一步的通过protobuf以及leveldb完成了log的序列化以及持久化
- 使用jrpc完成了消息接口的定义
- 搭建了raft逻辑框架 
