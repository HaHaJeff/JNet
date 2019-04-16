# rpc实现
参考[muduod](https://github.com/chenshuo/muduo.gita)的基于protobuf的rpc实现

## protobuf 传输格式
 |--------|
 |   len  |
 |--------|
 |  name  |
 |   len  |
 |--------|
 | message|
 |   len  |
 |--------|
 |protobuf|
 |  data  |
 |--------|
 |  check |
 |   sum  |
 |--------|

codec需要按照上述格式进行解析

## protobuf_codec
- 只是负责将RpcMessage序列化并发送出去,通过OnMessage调用判断当前packet是否可以满足解析条件，如果满足，完成包的解析之后调用注册的messagecb_，该函数有RcpChannel定制
- RcpChannel中的OnRpcMessage作为核心解析函数
    - 如果RpcMessage类型为request，则服务端需要通过RpcMessage中的service字段查找到对应Service
        - 根据Service以及RpcMessage的method字段可以得到Method对象
        - 根据Method可以得到Response以及Resquest类型，完成函数调用，具体调用CallMethod方法，服务端会对该方法进行override，具体完成函数体以及DonCallback调用  DoneCallback负责完成消息序列化并返回到Client
    - 如果RpcMessage类型为response，
        - 客户端通过RpcMessage中的id字段查找对应的OutStandingCall上下文信息，并调用其Done()函数
        - OutStanding用于保存调用上下文信息，当Client调用CallMethod方法时，会构造一个OutStandingCall对象
