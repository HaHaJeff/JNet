# JNet

**一个基于C++11的Linux网络库**
- 使用epoll
- 优雅的实现功能定制
- 基于reactor模式

**测试**
- 与libevent2.0中的bench_http做对比，吞吐不相上下，每秒httpserver可以处理16000个http请求包
- 进行pingpong测试，采用本地回环地址，单线程吞吐量145MB/s
