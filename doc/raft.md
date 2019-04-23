
# raft算法流程

- follower
- candidater
- leader

**raft采用强leader保证算法的简洁性以及正确性**

## leader选举

**raft使用心跳出发leader选举。当服务器启动时，初始化为follower。leader向followers周期性发送heartBeat。如果follower在选举超时时间内没有收到leader的heartBeat，就会等待一段随机的时间后发起一次leader选举**

随机等待的目的是**减少选举冲突**。

- 只有包含最新**commited log**的candidater才能竞选成功，raft如何保证？
  - RequestVote会携带prevLogIndex以及prevLogTerm，其他节点在收到消息时：
    - 如果发现自己的日志比请求中携带的更新，则拒绝投票。比较原则：如果本地的最后一条的log entry的term更大，则term大的更新。如果term一样大，则log index更大的更新
    - 否则，同意该candidater的选举

## 日志复制

leader把客户端请求作为日志条目，然后并行的向其他服务器发起AppendEntries RPC复制条目。当这条日志被复制到大多数服务器上时，leader将这条日志应用到它的状态机中，并向client返回结果。

raft日志同步保证以下两点：

- 如果不同日志中的两个条目有着相同的索引号和任期号，则它们所存储的命令相同
- 如果不同日志中的两个条目有着相同的索引号和任期号，则它们之前的所有条目都是一样的

第一特性源于leader在一个term内，在一个给定的log index最多创建一条日志条目，同时该条目在日志中的位置也从来不会改变

第二条特性源于AppendEntries的一个简单的一致性检查。当发送一个AppendEntries RPC时，Leader会把新日志条目紧接着之前的条目的log index和term都包含在里面。如果Follower没有在它的日志中找到log index和term都相同的日志，它就会拒绝新的日志条目。

- leader通过强制followers复制它的日志来处理日志的不一致，followers上的不一致的日志会被leader的日志覆盖。
- leader为了使followers的日志同自己的一致，leader需要找到followers同它的日志一致的地方，然后覆盖followers在该位置之后的条目。
- leader会从后往前尝试，每次AppendEntries失败后尝试前一个日志条目，直到成功找到每个follower的日志一致位点，然后向后覆盖

## 安全性

- 拥有最新的已提交的log entry的follower才能成为leader
- leader只能推进commit index来提交当前term，旧term日志的提交要等到当前term的日志间接提交

# Client interaction

## find leader

- 当client启动时，随机的连接一个server。如果该server不是leader，那么client的request将会被拒绝，同时得到server的response，该response中包含了最近一次的leader位置
- client向leader发起连接，如果leader已经crash了，那么client的request将会超时，此时，client回到第一步

## linearizable

**定义：**在线性一致性系统中，任何操作都能在调用和返回之间原子的执行完成

- 瞬间完成（原子性）
- 发生在invode与response两个事件之中
- 反映出“最新值”

![](https://raw.githubusercontent.com/HaHaJeff/HaHaJeff.github.io/master/img/raft/linearizable.png)

上图满足线性一致

- 对于x，其初始值为1，客户端ABCD并发的进行请求

- 对于每个请求，线段表示处理request花费的时间，线性一致性没有规定request具体在线段上的哪个点执行，只需要在**invode与response之间**的某个时间点**原子**的执行完成

- 最新的值，从client B的角度看：

  ![](https://raw.githubusercontent.com/HaHaJeff/HaHaJeff.github.io/master/img/raft/example_B.png)

  

  x的值分为三个阶段，在invoke w(2)和response之间对x的读取可能是1也可能是2，但是在response之后对x的读取一定是2

## raft的线性一致

- 对于write操作，leader会生成对应的op log，并将其序列化，以次顺序commit，并apply后返回client，当write被commit以后，raft保证其前面所有的write已经被commit。所以，所有的write操作都是严格有序的。综上，raft满足线性一致写

- 对于read操作，采用read log的方式一定是满足线性一致读的，但是这样的话有点小题大作（开销很大），因为read操作其实并不改变状态机

  - 使用**read index**减少开销，由于read log需要涉及到网络以及磁盘开销，采用read index可以减少磁盘开销，从而提升效率。具体实现方式：

    1. leader记录当前的commit index，称为read index
    2. 向follower发起以此心跳，证明自己还是leader  **十分必要**
    3. 等待状态机**至少**应用到read index记录的log
    4. 执行read，将结果返回给client

    ![](https://raw.githubusercontent.com/HaHaJeff/HaHaJeff.github.io/master/img/raft/linearizable_leader_valid.png)

    第二点的必要性，考虑ABCDE5个节点

    1. 刚开始的时候A作为leader对外提供服务
    2. 发生网络隔离，集群被分割成两部分，A和B以及CDE。虽然A会持续向其他几个节点发送heartbeat，但是由于网络隔离，CDE将无法收到A的heartbeat。默认的，A不处理向follower节点发送heartbeat失败(此处为网络超时)的情况(协议没有明确说明heartbeat是一个必须收到follower ack的双向过程)
    3. CDE组成的分区在经过一定时间没有收到leader的heartbeat后，触发election timeout，此时C成为leader。此时，原来的5节点集群因网络分区分割成两个集群：小集群：A和B，A为leader；大集群：C、D和E，C为leader
    4. 

    第三点中的**至少**是关键要求，因为：read log可以保证线性一致性读，该请求发生的点为commit index，也就是说这个点能使read log满足线性一致，那显然发生在这个点之后的read index也能满足线性一致读

  - 使用**lease read**，lease read与read index类似，但更进异步，不仅省却了log，还省去了网络交互。它可以大幅度提升读的吞吐也能显著降低延时。基本思路：leader取一个比election time out更小的租期，在租期内不会发生选举，确保leader不会变，所以可以跳过read index的第二步，也就降低了延时。lease read的正确性和时间挂钩，因此时间的实现至关重要，如果漂移严重，这套机制就会有问题

  - **wait free**，lease read省去了read index的第二步，实际还能再进一步，省去第三步。这样的lease read在收到请求后立刻进行读请求，不取commit index也不等状态机。由于raft的强leader特性，在租期内的client收到的response由leader的状态机产生，所以只要状态机保证线性一致，那么在lease内，不管任何时候发生读都能满足线性一致。



---

# 参考

[1]. [In Search of an Understandable Consensus Algorithm](https://raft.github.io/raft.pdf)

[2]. [raft youtube](https://www.youtube.com/watch?v=JEpsBg0AO6o&t=182s)

[3]. [etcd线性一致性读](https://zhengyinyong.com/etcd-linearizable-read-implementation.html)

[4]. [线性一致性和raft](https://pingcap.com/blog-cn/linearizability-and-raft/)
