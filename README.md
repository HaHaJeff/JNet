# JNet

**一个基于C++11的Linux网络库**
- 使用epoll
- 优雅的实现功能定制
- 基于reactor模式

**以完成组件，但为添加到代码中**
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

