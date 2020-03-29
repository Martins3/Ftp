#ifndef FTPCLIENT_H_PWPF0NHJ
#define FTPCLIENT_H_PWPF0NHJ
#include "TcpClient.h"
#include <base/LogInterface.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <net/IOEventLoop.h>
#include <net/IOEventLoopThread.h>
#include <net/SSL.h>
#include <net/TcpServer.h>
#include <queue>
#include <sys/stat.h>
#include <thread>

using namespace eomaia::net;
using namespace std;

class LoginDialog;
class MainWindow;

class FtpClient {

  void write_queue_thread() {
    while (true) {
      Message msg;
      { // 需要获取到一个锁之后，然后立刻释放锁，在发送过程中间，待发送队列还是可以继续添加的
        std::unique_lock<std::mutex> lock(msg_mutex);
        while (msgQueue.empty()) {
          msg_condtion.wait(lock);
        }
        msg = msgQueue.front();
        msgQueue.pop();
      }
      printf("write thread : [%s] [%d]\n", msg.getData(), msg.getLength());
      client->write(msg);
      msg.free();
    }
  }

  void addMsg(Message msg) {
    std::unique_lock<std::mutex> lock(msg_mutex);
    msgQueue.push(msg);
    msg_condtion.notify_one();
  }

private:
  //创建主循环事件分发器
  IOEventLoop loop;

  bool started;
  std::mutex start_mutex;
  std::condition_variable start_condtion;

  std::queue<Message> msgQueue;
  std::mutex msg_mutex;
  std::condition_variable msg_condtion;

  std::shared_ptr<std::thread> write_thread; // 其实没有必要创建这个东西!
  std::shared_ptr<std::thread> connection_thread; // 其实没有必要创建这个东西!

  TcpClient *client;
public:
  FtpClient(LoginDialog * l, MainWindow * m)
      : started(false), write_thread(new std::thread(
                            std::bind(&FtpClient::write_queue_thread, this))),
        connection_thread(
            new std::thread(std::bind(&FtpClient::initConnection, this, l, m)))
  {}

  void getDir() {
    Message msg;
    msg.clientDir();
    addMsg(msg);
  }

  // 正确打开并且获取到文件名，将文件名和数据组装起来，然后发送给
  void upload(string path) {
    Message msg;
    // FIXME !
    const std::string base = "/home/shen/Downloads/client/";
    msg.fileMsg(Message::C_UPLOAD, base, path);
    addMsg(msg);
  }

  // 封装数据，发送请求。
  void download(string filename) {
    Message msg;
    msg.clientDownload(filename);
    addMsg(msg);
  }

  // 组装数据，然后发送请求
  void login(const char *name, const char *password) {
    Message msg;
    msg.clientLogin(name, password);
    addMsg(msg);
  }

  void initConnection(LoginDialog *l, MainWindow * m) {
    SocketAddr addr("127.0.0.1", 2333, false);
    client = new TcpClient(&loop, addr);
    client->setupUI(l, m);
    client->connect();
    loop.run();
  }
};

#endif /* end of include guard: FTPCLIENT_H_PWPF0NHJ */
