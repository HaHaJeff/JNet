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
