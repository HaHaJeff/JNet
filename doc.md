# 基本组建
### channel
channel作为poller调度的基本单位，是网络编程中必不可少的一环。负责注册与相应IO事件。handy和muduo在该方面都有自己的实现，两者的实现方式类似，都是将EventBase(handy+PollerBase，EventLoop-muduo)作为channel成员变量，在channel构造的时候直接将this添加到EventBase(EventLoop中)

**channel调用方控制加入EventLoop以及从EventLoop中删除**

**在handy中，channel析构自动关闭其负责的fd；在muduo中，channel不负责fd的关闭，而是由更高层的TcpConnect等负责,TcpConect通过设置回调从而在EventLoop(EventPoller)中自动调用该函数，从而将fd关闭,需要自己调用TcpConnect提供的关闭函数**

### Poller
Poller是IO multiplexing的封装，负责管理channel的活动,需要一个容器管理channel **libevent将poller作为基类，后端分别对应不同平台实现不同的多路复用结构**

### EpollPoller 继承自Poller

**TODO:调用addchannel与updatechannel应该可以合为一次调用，参考muduo**

- AddChannel: 向epollfd添加一个struct epoll_event结构体，epoll_event.data.ptr = channel
- RemoveChannel: 从Poller中移除特定的channel，使用epoll_ctl(del)，**但是handy中处理删除只是将对应epoll_event.data.ptr置nullptr**
- UpdateChannel: 更新对应channel的event
- Poll: Poller不做具体事件的执行，而是仅仅将活动的channel返回

### PollPoller: 继承自Poller
- AddChanne: 向pollfds_中添加channel，其实会将channel转换成struct pollfd，为了更好的管理pollfds_，我们将channel对应的pollfds_中的index加入到channel中，方便管理
- RemoveChannel: 从pollfds_中移除channel对应的struct pollfd

- AddChannel: 将channel添加到pollfds_中，pollfds_是一个vector<struct pollfd>,不同于epoll，epoll的Addhannel需要将channel对应的epoll_event直接加入到epollfd中，系统调用，而poll在轮询的时候会将pollfds_全部传入到内核中；
- RemoveChannel: 从pollfds_中删除对应的channel,然而如何删除？从基类中的channels_中查找对应channel对应的fd(channels_: map<Channel, int>),然后根据fd==pollfd.fd在pollfds_查找对应的struct pollfd，然后删除，muduo提供了一种简单高效率的做法，在Channel中添加一个字段index_，在Addhannel的时候设置index为pollfds_对应的位置，这样删除会非常迅速
- UpdateChannel: 更新pollfds_中channel对应的struct pollfd

### Timer
封装了timer的序号以及到期时间和一个回调函数

### TimerQueue
使用timerfd_create创建一个用于定时的文件描述符，并将此文件描述符封装为channel加入到EventLoop中, AddTimer接口作为TimerQueue的开放接口，每次添加Timer的时候，将timer加入到std::set中，以到期时间作为key值。**但是仅以到期时间作为key，会出现到期时间重复的问题，于是采用key=到期时间+Timer指针**

### 事件处理机制 参照libevent
```
if (events & (POLLHUP | POLLERR | POLLNVAL)) {
    read_write();
}
if (events & POLLIN) {
    read();
}
if (events & POLLOUT) {
    write();
}
```

### EventLoop
封装事件循环，也是事件分派的中心。使用Poller作为IO Multiplexing


### TcpConn
> 封装Tcp connection
- 如果文件描述符是非阻塞的，对connect需要进行额外的一次poll判断
- 封装了send调用，优先会往ouput buffer写,在出发handlewrite时会对output进行一次consume操作
- 文件描述符的可读可写回调由用户设置


### util
一些基础功能，例如时间相关，以及字符串转换成int

### Timer设计
**参考muduo**

- 以到期时间作为key,如何处理到期时间相同的Timer?
    - Timer中包含一个独一无二的sequence_id以区别不同的Timer
    - 为了方便管理，使用TimerID，其中包含sequence_id和Timer*，加速查找删除等操作。
    - TimerQueue, 管理所有的timer.

### Socket
封装了socket的基本操作
- BindAddress(const Ip4Addr& localAddr) 绑定struct sockaddr结构体
- Listen(); 开启监听
- Accept(); 从连接建立完成队列中抽取一个已完成连接建立的套接字


### 为什么需要应用层缓冲？
要想不阻塞当前IO线程（负责poll），必须使用nonblocking配合应用层buffer 考虑如下场景：
- 如果用户需要发送10KB数据，但是由于TCP滑动窗口的原因只能发送8KB
，剩下的2KB只能不断的write，然后返回EAGING或EWOULDBLOCK，这样就相当与阻塞了当前发送线程，然而采用buffer就不一样了，当数据写入buffer内后，发送线程就可以去干其他事情了。当位于poll的TCPCONN可写时自动从buffer中发送数据。这就解耦了应用程序与网络库，应用程序只管数据的生成，而不去操行数据是分几次发送的。
- 如果对方发送了两条1KB的消息，那么接收方收到数据的情况可能是：
    - 一次性收到2KB数据
    - 分两次收到，第一次600B，第二次1400B
    - 分两次收到，第一次1400B，第二次600B
    - 分两次收到，第一次1KB，第二次1KB
    - 分三次收到，第一次600B，第二次800B，第三此600B
    - 其他任何可能。一般而言，长度为n字节的消息分块到达的可能性有2^(n-1)
所以如果应用程序需要自己去负责消息的完整性会有许多不便利的地方。提供input buffer则可以有效解决。当input buffer内的数据构成一条完整的消息时再通知程序的业务逻辑，这通常是有codec负责。

**已完成组件，但为添加到代码中**
- 同步队列以及线程池

**未完成功能**
- 多线程操作EventLoop

**实现思想：为EventLoop添加一个SafeQueue，当其他线程想要操作tcpconnptr时，只是将任务push进SafeQueue并调用wakeup唤醒IO线程**

- 如何唤醒？

为EventLoop添加一个wakeupfd_并将fd加入eventloop中，当调用wakeup函数时候，向其中写入一个字节，这样wakeupfd_就可读了，产生读时间，pop safequeue并执行

- wakeupfd_如何选择？
pipe可以，生成一队fd，并写readfd加入epoll中。
采用eventfd可以完成时间通知，哈哈，配合timerfd感觉瞬间高大上哈。

**处理读事件**
- muduo：在handlRead中显式调用messageCallback_，如果没有设置messageCallback_，那么数据会在inputBuffer累积（因为defaultMessageCallback除了写日志啥也不做)
- handy: 在handleRead中只是将数据读入buffer中，只有设置onRead，才会改变readcb_，而OnMsg其实就是改变readcb_, 在handleRead中是while的，当read读完之后才会调用readcb_

### 优雅的关闭连接
- close：递减描述符引用计数，当计数为0时，开始实施清理工作。
    - 如果关闭监听套接字，将会向那些处于SYN_RECV状态的fd发送RST报文
    - 如果关闭普通套接字
        - 接收缓冲区内有数据，那么直接丢弃并向对方发送RST报文
        - 发送缓冲区内有数据，close操作将在最后一个报文出加上FIN字段，但是此时连接没了，进程没有办法感知到数据能否被对端确认
- shutdown：断开连接
    - 如果关闭监听套接字，且关闭选项是read，那么将会向那些处于SYN_RECV状态的fd发送RST报文,否则啥也不干
    - 如果关闭普通套接字，且关闭选项是read，仅仅只是在本端做个标记，无法read数据而已
    - 如果关闭普通套接字，且关闭选项是write
        - 如果接收缓冲区内有数据，无所谓吗，因为还可以读呀
        - 如果发送缓冲区内有数据，同理还是在最后一个数据报处添加FIN字段，那么对断read返回0后，采取关闭措施，本端还是可以通过read 0感知到，哦上一次的数据被接收了

### rpc
使用protobuf预留的rpc框架，并使用protobuf作为IDL
需要实现的有RpcChannel, SpecificService, RpcServer, ProtobufCodec

调用流程
1. Echo，对应着使用EchoSerivce_Stub的CallMethod
2. 在CallMethod中调用channel_->CallMethod
3. channel_->CallMethod负责网络逻辑，这里可以使用TcpConnPtr对象，设置messageback以及rpcmessagecallback
    1. messagecallback用于conn->HandleRead中调用，rpcmessagecallback用于在messagecallback中调用，即具体的rpc调用将在这里产生。分为request以及response两个逻辑
    2. client端负责response，server负责request
    3. rpcchannel需要和map<services>耦合，因为在request中需要确定具体的service，如何确定具体的service可以借鉴muduo的思路，即利用protobuf提供的反射机制完成
4. 调用send函数将消息发送出去
5. 服务端处理HandleRead调用messagecallback，满足codec条件时调用rpcmessagecallback,构造response并send回client
6. 同理客户端调用，不过这里还会过一次额Donecallback，为CallMethod方法的最后一个参数，因为在第4步并不是同步等待的，Donecallback需要进行最后的处理工作,即完成对response的处理，所以response需要分配在heap上或者在调用Donecallback前分配

#### rpc设计
- service
protobuf rpc service的抽象接口，有client的stub以及server负责实现
- client stub
实现service_stub类，涉及到rpcchannel(rpcchannel负责网络通信, 组成对象包括：TcpConn)
- server
管理具体的services_，一般采用map实现，key为string，value为services
- rpcchannel
    - 在client处理response消息
    - 在server处理request消息

两端消息格式
``` cpp
enum MessageType {
    REQUEST=1;
    RESPONSE=2;
}

message RpcMessage {
    required MessageType type=1;
    required fixed64 id=2;

    optional string service=3;
    optional string method=4;  // MethodDescriptor->name()
    optional bytes request=5;     // 对应request的序列化
    optional bytes response=6; //对应response的序列话
}

```

- buffer
在buffer中添加一个额外的字段，用于保存应急信息
