# ftp

https://www.jianshu.com/p/7e023dd5fb79
https://www.jianshu.com/p/e6b9481ca754
https://www.cs.bgu.ac.il/~asharf/SPL/Reactor.pdf

https://stackoverflow.com/questions/34073871/socket-programming-whats-the-difference-between-listen-and-accept
https://stackoverflow.com/questions/27014955/socket-connect-vs-bind

https://github.com/embeddedmz/ftpclient-cpp/blob/master/FTP/FTPClient.cpp : as a new client

1. epoll : 电平是怎么回事?
2. sendfile syscall 了解一下

3. 利用 calnender 以及 。。。的代码实现vim gui的效果。

openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365

## Bug

1. 直接在 client 中间使用 exit(2), 导致 server core dump
2. 直接管理 server 而不关闭 client 同样导致问题 ! main.cpp : true

3. 一定需要首先进行一个读取操作，来处理Server 在 Connection 发送的恢复消息，
c/ssl.cpp 进行测试，效果惊人。
4. Socket 对象是什么时候注销的 ?
    1. 除非理解 shared_ptr 



https://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl

1. 原则:
  1. 私钥生成公钥
  2. 公钥加密，需要私钥解。反之也是。

3. A 发送消息给 B，首先发送 CA 使用私钥加密的消息，B 接受到之后，使用CA的公钥解密，得到 A 的公钥
此时，如果 B 成为中间人，那么 B 重做的信息需要 被 CA 的公钥解密之后正好是 B 的公钥，但是生成这个需要
A 的私钥。

4.  A 的公钥加上⼀些 A 的个⼈信息，被 O 私钥签名之后,
就得到 A 的证书，因为⼈⼈都有 O 的公钥，所以⼈⼈都
可解密 A 的证书，换⾔之，证书⾥必有公钥。



## 设计
> 形成更好的封装

1. TcpClient : 类似于 Connect 提供两个 Interface : write 和 [[Notification 机制，当接收到消息之后，将消息利用 Contitional Variable 将消息上传。写一个消息队列的东西，当消息上传之后]]
2. Protocal : 统一封装报文消息。

Socket fd 和 IOEvent 三者是一一对应的关系，为什么不采用统一的封装 ?
当处理网络编程的时候，是的，但是
当处理 timer 的时候并不是 !


IOEvent : 持有 fd，实际上是fd 对应的 events 这个 flags 的管理者而已。
events 的使用方法 :
1. 持有 Loop ，进而持有IOCtrl 的，
2. enable 函数 : 
3. handle 函数 : epoll 会告知所有的出现消息的 fd 是什么，IOEventCtrl 中间存在的 fd 和 IOEvent 的对应函数。

IOEventLoopThread 的实际含义:
1. 每一个 IOEventLoopThread 需要对应一个 nullptr
2. 每一个 IOEventLoopThread 创建对象的时候，会自动创建 thread，并且执行 beginRun
3. beginRun 需要等待该线程的对应的 loop 初始化，初始化之后，进行 loop.run
    1. 主线程的 loop 是手动初始化的

## 有待整理的
1. SSL 和 TSL 的关系是什么 ?
2. 涉及到的 socket 系统调用功能整理
3. read 和 readv 各自是什么 ?
4. 命名空间
    1. 什么时候需要套上 namespace 的空间。
    2. using 的使用实际
    3. 头文件为什么需要增加上 : <net/Socket.h>
    4. 即使头文件在外面，但是依旧可以使用namespace
        1. 猜测: 添加namspace 相当于 C 语言中添加前缀而已。
5. 各种系统调用
6. 虚基函数 和 Bind 是不是在配合使用呀 ?

7. 找到 timer_fd 相关内容，为什么在内核不记得相关的内容呀 !
8. 其 Log 系统的设计原则是什么 ?

9. shared_ptr 的含义到底是什么:
    1. get() ?

10. 静态变量为什么初始化的位置为什么需要放到 .cpp 中间 ?

11. 这个函数子类不实现会存在什么问题 ?
    virtual void connectCallback(std::shared_ptr<TcpConnect> tcpConnect)=0;
    1. 调用该函数总是首先判断 if(connectCallback), 那么将其赋值为其他，是否可以 ?

12. TcpConnect 的 wirte 实现中间 : 有没有考虑两个写同时出现的情况 ? 或者正在读，然后进行写的操作 ?

13. 各种标准库函数 :
    1. assign
    2. bind

14. http://everythingshakesme.blogspot.com/2013/04/c-terminate-called-without-active.html
    1. 开启的线程需要合适的回收。
