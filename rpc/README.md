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
