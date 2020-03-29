# ftp 设计

## Server 的多线程设计
[one loop per thread](https://www.jianshu.com/p/7e023dd5fb79)

[Reactor 模式介绍](https://www.cs.bgu.ac.il/~asharf/SPL/Reactor.pdf)

[eomaia](https://github.com/wlgq2/eomaia)

1. Socket fd 和 IOEvent 三者是并不是一一对应的关系，因为其中支持了 timer 机制，所以 Socket 和 fd 不是一一对应的，IOEvent 总是和 fd 是一一对应的。
2. IOEvent : 核心成员就是 epoll_wait 的 events 这个参数管理，从而实现对于部分参数选择性读取。
    1. enable 函数 : 通过 epoll_ctl 来修改 epoll_wait 的内容
    2. handle 函数 : epoll 会告知所有的出现消息的 fd 是什么，IOEventCtrl 中间存在的 fd 和 IOEvent 的对应表，然后就调用 IOEvent::handle，IOEvent::handle 分析 events 上的 flags，确定需要执行的函数。

3. 每一个 IOEventLoopThread 创建对象的时候，会自动创建 thread，并且执行 beginRun
beginRun 需要等待该线程的对应的 loop 初始化，初始化之后，进行 loop.run
主线程的 loop 是手动初始化的

4. 主线程也就是 TcpAccept 的 loop 负责处理 socket accept 的事情，每当获取到一个新的链接，那么就创建一个新的TcpConnect，并且从 thread pool 中间选择一个 thread 与之对应。
5.

## openssl
https://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl

openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365

1. 原则:
  1. 私钥生成公钥
  2. 公钥加密，需要私钥解。反之也是。
3. A 发送消息给 B，首先发送 CA 使用私钥加密的消息，B 接受到之后，使用CA的公钥解密，得到 A 的公钥 此时，如果 B 成为中间人，那么 B 重做的信息需要 被 CA 的公钥解密之后正好是 B 的公钥，但是生成这个需要 A 的私钥。
4. A 的公钥加上⼀些 A 的个⼈信息，被 O 私钥签名之后, 就得到 A 的证书，因为⼈⼈都有 O 的公钥，所以⼈⼈都 可解密 A 的证书，换⾔之，证书⾥必有公钥。
5. TLS 和 SSL 的关系是什么 : https://en.wikipedia.org/wiki/Transport_Layer_Security

## 网络相关的系统调用基本功能整理
https://stackoverflow.com/questions/34073871/socket-programming-whats-the-difference-between-listen-and-accept
https://stackoverflow.com/questions/27014955/socket-connect-vs-bind

1. 牢记基本套路：
  1. socket(), close(): 生成 file descriptor 和关闭
  2. read() write() 
  3. client 特有 : connect 
  4. server 特有 : bind listen accept
      1. bind : 让 server 监听某个端口
      2. listen : 表示对于某个 fd 是存在
      3. connec : 获取到链接服务器的 addr 并且分配 fd 来管理
2. readv 和 read 的区别 : readv 就是 scatter 和 gather 操作，内核中间的 read_iter 的含义还是不知道的。

## 重新学习一个 C++ 
1. namespace : 相当于给添加前缀
    1. 如何使用 uppper 层次的 namespace ?
    2. 基本的使用原则: 头文件的class 总是被 namespace 的申明包含，而cpp 中间总是直接 using，从而
    3. 如果使用 using，相当于将该空间的符号 export 出来。
    4. 可以通过 namespace fs = std::filesystem; 来简化
    5. 如果出现命名重叠，那么使用 :: 重新从最上层访问

2. shared_ptr : 对于指针的封装
    1. 初始化依赖于 : new 
        1. 对于同一个指针，如果两个 shared_ptr 对象使用其初始化，那么程序执行会出现 double free 的问题 : 因为两个 shared_ptr 在销毁的时候指向相同的内存对象，所以释放的时候销毁同一个位置，但是两者的引用奇数是分离开来的。
    2. 基本结构: 拷贝的时候，增加引用计数，销毁的时候减少。
    3. shared_ptr 的指针的含义 : 可以绕过其ptr 的功能。
```cpp
  // 输出 2 2
  std::shared_ptr<int> foo2 (new int);
  shared_ptr<int> g = shared_ptr<int>(new int);
  cout << foo2.use_count() << " ";
  cout << g.use_count() << endl;

  // 输出 1
  std::shared_ptr<int> foo2 (new int);
  cout << foo2.use_count() << endl;

  // 拷贝真的是一个神奇的东西呀 !
```
  4. weak_ptr : 进行拷贝的时候，不会增加引用计数。
  5. https://stackoverflow.com/questions/4984381/shared-ptr-and-weak-ptr-differences : strong 等于 0 管理的结构体被删除，week 也等于0，计数器被删除。

3. 静态变量相当于全局变量。

## C++ STL 相关内容
1. c++17 提供 filesystem library 
2. 通过IO library 提供的 string stream 以及 file stream
3. thread library :
    1. 为什么需要使用条件变量，而不是mutex 来实现 ?
      1. 并发队列 : 只能使用 mutex 而不是 spin_lock，否则性能存在问题。
      2. 当新的元素到来的时候，mutex up，那么每一次 fifo 操作都需要一次 mutex 操作。
      3. 从 ucore 的实现中间可以知道，mutex，monitor，condition variable 都是可以源自于 spin lock 的使用，condition variable 非常完美的实现，当无事可做的时候释放锁的原则。
    2. thread : 创建线程 mutex,  cv, future 实现线程间同步。
4. bind 函数和继承 :   
    1. virtual 函数 : 如果没有被重载，那么该函数就是一个数值。
    2. bind 由于是动态执行的，其作用只是替换函数的参数，当 bind(&TcpServer::messageCallback, this) 的时候，替换参数，在本项目中间的具体作用一般是处理 this 指针。

## Linux AIO 机制
https://github.com/littledan/linux-aio

## cmake 
1. cmake 的生存问题 :
  1. 告知到底那些 .cpp 需要依赖 : 
  2. 依赖的头文件和库的路径指定 : 
  3. 制定生成的内容
  4. 一键make run 的方法 :
  5. 制定生成二进制文件的名称

## Qt图形界面总结
1. MainWindow 的设计成为了标准模板
    1. QDialogButtonBox : 添加Button 以及 关联信号的方法
    2. 大致可以猜测 [slots 和 signal 的实现](https://doc.qt.io/qt-5/signalsandslots.html)，emit A 的函数，然后会调用 B 的函数，可能存在一个表格，将这种联系构建起来，其实没有 slots 和 signal 机制也是问题不打，但是一个对象如何才可以调用另一个对象的函数，这很难变得优雅。
    3. QListWidget 的使用
    4. QGridLayout centralwidget 以及 MainWindow 构建基本布局的方法。

#### 了解一下新概念
1. [trait class](https://stackoverflow.com/questions/3979766/how-do-traits-classes-work-and-what-do-they-do) 和 Rust 的 trait 没有任何关系，只是为了提供类型的信息而已。


## Bug
1. 直接在 client 中间使用 exit(2), 可能导致 server core dump
2. 直接关闭 server 而不关闭 client 同样导致问题 ! main.cpp : true
5. server 的 crash，client 无法正确的处理 errro 的情况。
4. Socket 对象是什么时候注销的 ?
    14. http://everythingshakesme.blogspot.com/2013/04/c-terminate-called-without-active.html

## TODO
7. 找到 timer_fd ，eomaia 和 内核中间的内容都需要总结一下。
8. 其 Log 系统的设计原则是什么 ?
12. TcpConnect 的 wirte 实现中间 : 有没有考虑两个写同时出现的情况 ? 或者正在读，然后进行写的操作 ?
