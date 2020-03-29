#include "EchoServer.h"
#include <iostream>

#include <base/LogInterface.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <net/SSL.h>
#include <sys/stat.h>
#include <iostream>
#include <chrono>
#include <ctime>    

using namespace eomaia::net;
using namespace std;

EchoServer::EchoServer(IOEventLoop *loop, SocketAddr &addr)
    : TcpServer(loop, addr) {
  load();
}

void EchoServer::connectCallback(std::shared_ptr<TcpConnect> tcpConnect) {
  cout << "new connect:" << tcpConnect->getAddr().toString() << "<count>"
       << getConnectCount() << endl;
  /* tcpConnect->write("I get the connection !\n"); */
}

void EchoServer::load() {
  kv["huxueshi"] = "pass1234";
  log.open ("/home/shen/Downloads/log.txt", ios::out | ios::app);
}

void EchoServer::messageCallback(std::shared_ptr<TcpConnect> tcpConnect,
                                 Buffer &buffer) {

  auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  const std::string base = "/home/shen/Downloads/server/";
  Message msg = buffer.readAllAsMessage();
  auto cmd = msg.getCmd();
  Message reply;
  if (cmd == Message::C_LOGIN) {
    char name[50];
    char pass[50];
    memset(name, 0, sizeof(name));
    memset(pass, 0, sizeof(pass));
    msg.serverLogin(name, pass);
    printf("[Login] : %s %s\n", name, pass);
    string k(name);
    string v(pass);
    if (kv[k] == v) {
      reply.LoginOk();
      tcpConnect->setUserName(name);
      log << std::ctime(&time) << tcpConnect->getUserName() << " Login successed ! "  << endl;
    } else {
      reply.LoginFailed();
    }
  } else if (cmd == Message::C_DOWNLOAD) {
    char t[50];
    sscanf(msg.getData(), "%s", t);
    reply.fileMsg(Message::S_DOWNLOAD, base, t);

    log << std::ctime(&time) << tcpConnect->getUserName() << " download : " << t << endl;
  } else if (cmd == Message::C_UPLOAD) {
    // FIXME change snippet into function : create a new io file
    char filename[50];
    sscanf(msg.getData(), "%s", filename);
    auto path = base + filename;
    auto file =
        std::fstream(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (file.is_open()) {
      printf("write [%s]\n", msg.getData() - 3 + 100);
      file.write(msg.getData() - 3 + 100, msg.getLength() - 100);
      file.close();
      reply.server_upload(Message::S_UPLOAD, base, filename);
      log << std::ctime(&time) << tcpConnect->getUserName() << "upload : " << filename << endl;
    } else {
      std::cout << "Server receive file failed !\n";
    }
    // FIXME end
  } else if (cmd == Message::C_DIR) {
    reply.server_dir(Message::S_DIR, base);
    log << std::ctime(&time) << tcpConnect->getUserName() << " access directory" << endl;
  } else {
    cout << "Server receive unrecognized cmd !\n";
  }

  tcpConnect->writeMsg(reply);
  reply.free();
  msg.free();
}

void EchoServer::writeCompletCallback(std::shared_ptr<TcpConnect> tcpConnect) {
  cout << "thread id:" << std::this_thread::get_id() << endl;
  string addr = tcpConnect->getAddr().toString();
  cout << addr << ":"
       << "write complet " << endl;
  base::Log::OUT(base::Log::Info) << addr << ":"
                                  << "write complet ";
}

void EchoServer::connectCloseCallback(std::shared_ptr<TcpConnect> connect) {
  string addr = connect->getAddr().toString();
  cout << "close connect :" << addr << endl;
  base::Log::OUT(base::Log::Info) << "close connect : " << addr;
}
