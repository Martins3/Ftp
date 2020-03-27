#ifndef EOMAIA_SOCKET
#define EOMAIA_SOCKET

#include <net/SocketAddr.h>
#include <net/SocketOperation.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

namespace eomaia {
namespace net {

class Socket {
public:
  Socket(int fd);
  ~Socket();
  void bind(SocketAddr &addr);
  int accept(SocketAddr &addr);
  void listen();
  int getFd();
  void setTcpNoDelay(bool enable);
  int shutdownWrite();

  ssize_t write(const void *buf, size_t count);
  ssize_t read(void * base, size_t count );

  void connect(struct sockaddr *);

#ifdef CONFIG_SSL
  SSL *ssl;
#endif

private:
  int socketFd;
};
} // namespace net
} // namespace eomaia

#endif
