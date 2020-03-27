#ifndef TCPCLIENT_H_BF4OPCDJ
#define TCPCLIENT_H_BF4OPCDJ
#include <base/LogInterface.h>
#include <cstring>
#include <net/IOEventLoop.h>
#include <net/Socket.h>
#include <net/TcpServer.h>
#include <string>
#include <sys/sendfile.h>
#include <thread>
#include <iostream>

class LoginDialog;
class MainWindow;
namespace eomaia {
namespace net {

// 协议设计 :
// 1. read queue : readEvent 保存到队列，每次加入都 notity_one !
// 2. write queue :
//
class TcpClient {
public:
  // 问题 :
  // 1. connect 成功是否会触发 epoll
  // 2. 综合 accept 和 connect 的结果
  //
  // 实现 : header 的作用
  // 1. cmd : 字符串 [30]
  // 2. filename / username password : [字符串 50] []
  TcpClient(IOEventLoop *loop, SocketAddr &addr)
      : eventLoop(loop), clientAddr(addr),
        /* socket(new Socket(SocketOperation::createNonblockingSocket())), */
        socket(new Socket(SocketOperation::createTestingSocket())),
        event(new IOEvent(loop, socket->getFd())), state(Disconnected) {
    eventLoop->addEvent(event);
    waiting_for_file = false;
    event->setReadFunc(std::bind(&TcpClient::readEvent, this));
  }

  void connect() {
    /* SocketOperation::listen(socket->getFd()); */
    /* SocketOperation::connect(socket->getFd(), */
    /* (struct sockaddr *)clientAddr.getAddrPtr()); */
    socket->connect((struct sockaddr *)clientAddr.getAddrPtr());
    event->enableReading(true);
    state = Connected;
  }


private:
 LoginDialog * login;
 MainWindow * window;
public:
  void readEvent();
  void setupUI(LoginDialog * l, MainWindow * m){
    login = l;
    window = m;
  }

  // 难道在此处需要部署 SSL 的close callback 吗 ?
  // client 和 server 都是需要 close callback 的 !
  void closeEvent() {
    state = Disconnected;
    /* if (closeCallback) { */
    /*   printf("%s", "closeEvent in TcpConnect"); */
    /*   closeCallback(shared_from_this()); */
    /* } */
  }

  void write(const char *data) {
    int len = ::strlen(data);
    write((void *)data, len);
  }

  void write(const std::string &data) {
    write(&(*data.begin()), data.length());
  }

  void sendfile(int out_fd, size_t count) {
    ::sendfile(socket->getFd(), out_fd, nullptr, count);
  }

  void write(Message &msg) {
    if (msg.getLength() == 0) {
      base::Log::OUT(base::Log::Error)
          << "Check Message::length initlizition\n";
    }
    write(msg.getData(), msg.getLength());
  }

  void write(const void *data, uint32_t length) {
    int n = 0;
    size_t remaining = length;
    bool faultError = false;
    if (state == Disconnected) {
      base::Log::OUT(base::Log::Warn) << "disconnected, give up writing";
      return;
    }

    //如该写数据缓冲区内有数据，直接写入socket缓冲区会导致数据交叠
    if (!event->isWriting() && writeBuf.isEmpty()) {
      // TODO 这里不做替换，为什么会造成 read event 无法被清理掉 ?
      /* n = SocketOperation::write(event->getFd(), data, length); */
      n = socket->write(data, length);
      if (n >= 0) {
        remaining = length - n;
        // TODO 暂时没有注册 !
        /* if (remaining == 0 && writeCompleteCallback) { */
        /*   writeCompleteCallback(shared_from_this()); */
        /* } */
      } else {
        n = 0;
        if (errno != EWOULDBLOCK) {
          base::Log::OUT(base::Log::Error) << "write data error";
          if (errno == EPIPE || errno == ECONNRESET) // FIXME: any others?
          {
            faultError = true;
          }
        }
      }
    }

    if (!faultError && remaining > 0) {
#if 0
        size_t oldLen = outputBuffer_.readableBytes();
        if (oldLen + remaining >= highWaterMark_
            && oldLen < highWaterMark_
            && highWaterMarkCallback_)
        {
        loop_->queueInLoop(std::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
        }
#endif // 0
      writeBuf.append(static_cast<const char *>(data) + n, remaining);
      if (!event->isWriting()) {
        event->enableWriting(true);
      }
    }
  }

  void writeInLoop(const void *data, uint32_t len) {
    eventLoop->runInLoop([this, data, len] { write(data, len); });
  }

  void setNoDelay(bool enable) { socket->setTcpNoDelay(enable); }

  // TODO 深入理解一个状态的含义
  void shutdownWrite() {
    if (state == Connected) {
      state = Disconnecting;
      socket->shutdownWrite();
    }
  }

  // 读取消息，读取想要的消息，如果读取到的不是想要的消息，抛弃。
  // 如果是想要的消息，notify 上层。
  //
  int waitResponse() {
    std::unique_lock<std::mutex> lock(mutex);
    condtion.wait(lock);
    return response;
  }

  enum Response { LOGIN_OK, LOGIN_FAILED, UPLOAD_OK, DOWNLOAD_OK };

private:
  IOEventLoop *eventLoop;
  SocketAddr clientAddr;
  std::shared_ptr<Socket> socket;
  std::shared_ptr<IOEvent> event;

  Buffer readBuf;
  Buffer writeBuf;

  bool waiting_for_file;

  int response;
  std::mutex mutex;
  std::condition_variable condtion;

  int state;
  enum ConnectState { Disconnected, Connecting, Connected, Disconnecting };
};
} // namespace net
} // namespace eomaia
#endif /* end of include guard: TCPCLIENT_H_BF4OPCDJ */
