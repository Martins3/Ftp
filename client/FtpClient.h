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

// FtpClient 设计可以仔细想一下:
class FtpClient {
  void interface() {
    {
      // TODO 理解C++ 的 client 机制
      std::unique_lock<std::mutex> lock(start_mutex);
      //如果在wait函数执行前执行notify则不会有效，所以需要先判断started状态。
      if (!started) {
        //如果在这里还未执行wait时，notify被执行，则这里会永远阻塞。所以需要加锁
        start_condtion.wait(lock);
      }
    }

    string user, pass;
    while (true) {
      cout << "NAME :" << endl;
      cin >> user;

      cout << "PASS :" << endl;
      cin >> pass;

      string msg = "111 " + user + " " + pass;
      client->write(msg);
      if (client->waitResponse() == TcpClient::LOGIN_OK) {
        break;
      } else {
        cout << "Login failed, you may try again\n";
      }
    }

    string line;
    cout << "> ";
    cout.flush();
    while (getline(std::cin, line)) {
      if (line == "ls") { // 222

      } else if (line == "u") { // 333

      } else if (line == "d") { // 444

      } else {
        cout << "unsupported command !" << endl;
      }
    }
  }

  void ls() {
    client->write("222");
    client->waitResponse();
  }

  void upload() {
    cout << "File Path : ";
    cout.flush();

    char filename[256];
    if (scanf("%s", filename) == 1) {
      FILE *file = fopen(filename, "rb");
      if (file != NULL) {

        int fd = fileno(file);
        struct stat st;
        stat(filename, &st);
        size_t size = st.st_size;

        client->write("333");

        if (client->waitResponse() == TcpClient::UPLOAD_OK) {
          client->sendfile(fd, size);
        } else {
          cout << "upload declined !" << endl;
        }

      } else {
        cout << "Open file failed" << endl;
      }
    } else {
      cout << "Read file path failed" << endl;
    }
  }

  void download() {
    cout << "File Path : ";
    cout.flush();

    char filename[256];
    memset(filename, 0, sizeof(filename));
    filename[0] = '4';
    filename[1] = '4';
    filename[2] = '4';

    if (scanf("%s", filename + 4) == 1) {
      client->write(filename);
      if (client->waitResponse() == TcpClient::DOWNLOAD_OK) {

      } else {
        cerr << "We never failed !" << endl;
      }
    } else {
      cout << "Read file path failed" << endl;
    }
  }

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
      printf("write thread : [%s] [%d]\n",msg.getData(), msg.getLength());
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
  FtpClient()
      : started(false), write_thread(new std::thread(
                            std::bind(&FtpClient::write_queue_thread, this))),
        connection_thread(
            new std::thread(std::bind(&FtpClient::initConnection, this))) {}

  void getDir() {
    Message msg;
    msg.clientDir();
    addMsg(msg);
  }

  // 正确打开并且获取到文件名，将文件名和数据组装起来，然后发送给
  void upload(string path){
    Message msg;
    // FIXME !
    const std::string base = "/home/shen/Core/ftp/eomaia/www/";
    msg.fileMsg(Message::C_UPLOAD, base,  path);
    addMsg(msg);
  }


  // 封装数据，发送请求。
  void download(string filename){
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

  void initConnection() {
    SocketAddr addr("127.0.0.1", 2333, false);
    client = new TcpClient(&loop, addr);
    client->connect();
    loop.run();
  }
};

/**
int testMain(int argc, char *argv[]) {
  SSLOperation::initClientCTX();
  FtpClient client;

  int cmd;
  while (scanf("%d", &cmd) == 1) {
    switch (cmd) {
    case 1:
      client.login("aaa", "bbb");
      break;
    case 2:
      client.getDir();
      break;
    case 3:
      client.download("a.md");
      break;
    case 4:
      client.upload("b.md");
      break;
    case 5:
    defatul:
      // TODO 为什么直接退出就会造成 server crash ?
      exit(0);
    }
  }
  return 0;
}
*/
