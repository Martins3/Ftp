#include "EchoServer.h"
#include <iostream>

#include <base/LogInterface.h>
#include <cstdio>
#include <iostream>
#include <net/SSL.h>

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

void EchoServer::load() { kv["aaa"] = "bbb"; }

void EchoServer::messageCallback(std::shared_ptr<TcpConnect> tcpConnect,
                                 Buffer &buffer) {

  const std::string base = "/home/shen/Core/ftp/eomaia/server/";
  Message msg = buffer.readAllAsMessage();
  auto cmd = msg.getCmd();
  Message reply;
  if(cmd == Message::C_LOGIN){
    char name [50];
    char pass [50];
    msg.serverLogin(name, pass);
    printf("[Login] : %s %s\n", name, pass);
    reply.LoginOk();
    // TODO and reply LoginFailed
  }else if(cmd == Message::C_DOWNLOAD) {
    char t[50];
    sscanf(msg.getData(), "%s", t);
    reply.fileMsg(Message::S_DOWNLOAD, base, t);
  } else if(cmd == Message::C_UPLOAD){

   // FIXME change snippet into function : create a new io file
        char filename [50];
        sscanf(msg.getData(), "%s", filename);
        auto file = std::fstream(base + filename, std::ios::out | std::ios::binary | std::ios::trunc);
        if(file.is_open()){
          printf("write [%s]\n", msg.getData() - 3 + 100);
          file.write(msg.getData() - 3 + 100, msg.getLength() - 100);
          file.close();
        }else {
          std::cout << "Server receive file failed !\n";
        }
    // FIXME end
  }else if(cmd == Message::C_DIR){

  }else{
    cout << "TODO\n";
  }

  tcpConnect->writeMsg(reply);
  reply.free();
  msg.free();

  /* cout << "thread id:" << std::this_thread::get_id() << endl; */
  /* string addr = tcpConnect->getAddr().toString(); */
  /* string data; */
  /* buffer.readAllAsString(data); */
  /* cout << "receive data form " << addr << ":" << data << endl; */
  /* base::Log::OUT(base::Log::Info) */
  /*     << "receive data form " << addr << ":" << data; */
  /* int cmd; */
  /* switch (tcpConnect->clientState) { */
  /* case TcpConnect::NOT_LOGIN: */
  /*   sscanf(data.c_str(), "%d", &cmd); */
  /*   if (cmd != 111) { */
  /*     tcpConnect->write("UNPRIVILEGED_OP"); */
  /*   } else { */
  /*     char name[256]; */
  /*     char pass[256]; */
  /*     sscanf(data.c_str() + 3, "%s %s", name, pass); */
  /*     string n(name); */
  /*     string p(pass); */

  /*     cout << "name : " <<  n << "pass" << p << endl; */
  /*     cout << "name : " << kv[n] << endl; */

  /*     if (kv[n] != p) { */
  /*       tcpConnect->write("LOGIN_FAILED"); */
  /*     } else { */
  /*       tcpConnect->write("LOGIN_OK"); */
  /*       cout << "So, does it success ?" << endl; */
  /*     } */
  /*   } */
  /*   break; */
  /* case TcpConnect::LOGIN: */
  /*   if (data == "222") { */
  /*     // TODO get data .... */
  /*   } else if (data == "333") { */
  /*     tcpConnect->clientState = TcpConnect::UPLOADING; */
  /*     tcpConnect->write("UPLOAD_OK"); */
  /*   } else { */
  /*     char filename[256]; */
  /*     if (sscanf(data.c_str(), "%d %s", &cmd, filename) == 2) { */
  /*       if (cmd == 444) { */

  /*         // TODO change this into configuation */
  /*         FILE *file = fopen("/tmp/", "r+"); */

  /*         if (file == NULL) { */
  /*           cerr << "you should avoid this purposely" << endl; */
  /*         } else { */
  /*         } */
  /*       } */
  /*     } */
  /*   } */
  /*   break; */
  /* case TcpConnect::UPLOADING: */
  /*   // TODO 输入成为文件 */
  /* default: */
  /*   tcpConnect->write("ERR\n"); */
  /* } */
}

void EchoServer::writeCompletCallback(std::shared_ptr<TcpConnect> tcpConnect) {
  cout << "这里会输出吗 ?" << endl;
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
