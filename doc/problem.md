- listen fd ，没有设置成nonblock，导致多路复用实效
- http, 报文解析出错
- CPU利用率与libevent一样，但是libevent的内存利用是0.1%，但是JNet是5.0%
- 没有判断事件错误码，如果出现IN+HUP错误表示对端已经断开链接了(对一个关闭的套接字写数据，第一次返回RST，read的时候应该要对这种错误进行捕捉并进行适当的处理)
    - 如果客户端突然断开，那么服务端会出现segment fault，原因是对一个read返回-1的套接字没有做正确处理 **应该cleanup 这个tcp链接**
    - 如果服务端突然断开，那么客户端也会出现segment fault，原因同样，不过客户端可以多实现一个功能：reconnect **暂时没有实现** 
- 如果在发送过程中，如果crtl+c掉客户端或者服务端，那么抓包可能会出现
    - FIN+ACK FIN+ACK ACK
    - RST(接受缓冲区还有数据没有被读取，服务器在close时会导致RST)

- 增加多线程操作IO loop功能，出现assert(tid_ == gettid())断言错误，原因是：在EventLoop没有构造完成时，发生AddToLoop调用
- 客户端主动断开连接，再次运行出现address in use

- 记lambda表达式中& =误用引起的segment faul,程序中使用TcpConnPtr对TcpConn对象进行保存(shared_ptr)
原因是：在lambda中采用&捕获了一个for循环中的局部变量i，在lambda被调用时已经失效了，所以你通过下标i访问的shared_ptr可能是未初始化的值

- 不经常错误，如果对断断开连接，可能出现segment fault，原因是在调用HandleWrite时出现 TcpConnPtr的count=1,但是this=0x0
- 配置：双核 i5
- bug场景：server开启一个eventloop线程
- 记tcpserver在client断开时可能出现core dump：也是由于shared_ptr使用不当造成的，每次server处理accept事件的时候，需要创建一个tcpconnptr对象并为该对象绑定Attach函数，为了保证线程安全，需要调用ioLoop的RunInLoop函数，然后将con insert到server的map中，这里会出现问题
- 在HandleAccept不使用lambda完成tcpconn的初始化 ，为什么？ 

服务端在read clientfd返回0时，不应该直接调用tcpconn::close函数关闭tcpconn，而是应该采用RAII机制完成

- 一个bug
    - 在做rpc功能是需要使用Message的向下转换，但是我的Message*是采用std::shared_ptr来做的，最开始采用raw pointer结合dynamic_cast，然后运行出segment fault，于是core gdb乎，问题定位于ProtobufCodec::OnMessage的messageCallback_回调后的shared_ptr回调后，发现其dtor函数core dump了，重复释放内存，这很危险
    - 调试了一上午，发现在ProtobufCodeT::OnRpcMessage中需要一次Message的down_cast，于是自己造了个shared_ptr down_cast的轮子

- protobuf NewCallback

在raft中使用protobuf提供的NewCallback函数时遇到的问题

```
struct A {
    typedef std::function<void(Arg1*, Arg2*)> OnRequestCallback;
    OnRequestCallback onRequest_;
    NewCallback(nullptr, this, onRequest_, arg1, arg2);
}
```

编译报错，无法实例化template NewCallback    问题解决：查看protobuf的NewCallback定义，首先类型必须匹配，参数类型必须为pointer

```
struct A {
    typedef std::function<void(Arg1*, Arg2*)> OnRequestCallback;
    OnRequestCallback onRequest_;
    NewCallback(nullptr, this, onRequest_, &arg1, &arg2);
}
```

编译报错：invalid use no static member function

```
struct A {
    typedef std::function<void(Arg1*, Arg2*)> OnRequestCallback;
    OnRequestCallback onRequest_;
    NewCallback(nullptr, this, A::onRequest_, &arg1, &arg2);
}
```

使用node1 node2 node3三个节点测试Raft，发现网络连接错误，排查原因发现：
使用同一个port同时作为listen以及connect端口绑定
**三种解决方案**

- 修改tcpconn.cc源码，对connect不使用bind
- 修改config.cc源码，增加localAddress字段 (使用这种方案，不需要改变基础库JNet的源码)
- 修改tcpconn.cc源码，使用Net::SetReusePort对fd进行设置 
