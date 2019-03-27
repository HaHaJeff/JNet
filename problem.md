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
