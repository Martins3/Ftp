#ifndef EOMAIA_DOME_ECHOSERVER
#define EOMAIA_DOME_ECHOSERVER

#include <net/TcpServer.h>
#include <unordered_map>
#include <fstream>

class EchoServer : public eomaia::net::TcpServer {
public:
  EchoServer(eomaia::net::IOEventLoop *loop, eomaia::net::SocketAddr &addr);
  //考虑到tcpConnect可能关闭后内存被自动释放，传shared_ptr比较安全。
  virtual void connectCallback(std::shared_ptr<eomaia::net::TcpConnect> tcpConnect);
  virtual void messageCallback(std::shared_ptr<eomaia::net::TcpConnect>, eomaia::net::Buffer &);
  virtual void writeCompletCallback(std::shared_ptr<eomaia::net::TcpConnect> tcpConnect);
  virtual void connectCloseCallback(std::shared_ptr<eomaia::net::TcpConnect>);

  
  
private:
  std::unordered_map<std::string, std::string> kv;
  void load();
  std::ofstream log;
};

#endif // EOMAIA_DOME_ECHOSERVER


