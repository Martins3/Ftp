#include "EchoServer.h"
#include <iostream>

#include <base/LogInterface.h>
#include <cstdio>
#include <iostream>
#include <net/SSL.h>
#include <sys/stat.h>

using namespace eomaia::net;
using namespace std;


char* permissions(char *file){
    struct stat st;
    char *modeval = (char *)malloc(sizeof(char) * 9 + 1);
    if(stat(file, &st) == 0){
        mode_t perm = st.st_mode;
        modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
        modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
        modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
        modeval[3] = (perm & S_IRGRP) ? 'r' : '-';
        modeval[4] = (perm & S_IWGRP) ? 'w' : '-';
        modeval[5] = (perm & S_IXGRP) ? 'x' : '-';
        modeval[6] = (perm & S_IROTH) ? 'r' : '-';
        modeval[7] = (perm & S_IWOTH) ? 'w' : '-';
        modeval[8] = (perm & S_IXOTH) ? 'x' : '-';
        modeval[9] = '\0';
        return modeval;     
    }
    else{
        return strerror(errno);
    }   
}



EchoServer::EchoServer(IOEventLoop *loop, SocketAddr &addr)
    : TcpServer(loop, addr) {
      load();
}

void EchoServer::connectCallback(std::shared_ptr<TcpConnect> tcpConnect) {
  cout << "new connect:" << tcpConnect->getAddr().toString() << "<count>"
       << getConnectCount() << endl;
  /* tcpConnect->write("I get the connection !\n"); */
}

void EchoServer::load() { kv["huxueshi"] = "pass1234"; }

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

    string k(name);
    string v(pass);
    if(kv[k] != v){
      reply.LoginOk();
    }else{
      reply.LoginFailed();
    } 
  }else if(cmd == Message::C_DOWNLOAD) {
    char t[50];
    sscanf(msg.getData(), "%s", t);
    reply.fileMsg(Message::S_DOWNLOAD, base, t);
  } else if(cmd == Message::C_UPLOAD){
   // FIXME change snippet into function : create a new io file
        char filename [50];
        sscanf(msg.getData(), "%s", filename);
        auto path = base + filename;
        auto file = std::fstream(path, std::ios::out | std::ios::binary | std::ios::trunc);
        if(file.is_open()){
          printf("write [%s]\n", msg.getData() - 3 + 100);
          file.write(msg.getData() - 3 + 100, msg.getLength() - 100);
          file.close();
          
        }else {
          std::cout << "Server receive file failed !\n";
        }
    // FIXME end
    // 返回一个的内容:
  }else if(cmd == Message::C_DIR){
    // 读取base 下所有的内容

  }else{
    cout << "TODO\n";
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
