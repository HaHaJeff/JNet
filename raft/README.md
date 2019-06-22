# 设计概要

- Node 作为RcpServer运行，接受peer消息
- peer 作为
- raft 核心算法实现，不必考虑网络通信，使用peer进行rpc调用
- storage 持久化模块 use leveldb
- log 日志模块
