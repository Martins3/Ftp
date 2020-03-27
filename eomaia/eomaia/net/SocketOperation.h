#ifndef EOMAIA_SOCKET_OPREATION
#define EOMAIA_SOCKET_OPREATION

#define CONFIG_SSL 1

#include <linux/tcp.h>
#include <netinet/in.h>
#include <string>
#include <sys/types.h>

namespace eomaia {
namespace net {
class SocketOperation {
public:
  static const int32_t Ipv4AddrAny;
  static int bind(int sockfd, const struct sockaddr_in *addr);
  static int createNonblockingSocket();
  static int createTestingSocket();
  static int listen(int sockfd);
  static int connect(int sockfd, const struct sockaddr *addr);
  static int accept(int sockfd, struct sockaddr_in *addr);
  static ssize_t read(int sockfd, void *buf, size_t count);
  static ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);
  static ssize_t write(int sockfd, const void *buf, size_t count);
  static void close(int sockfd);
  static int shutdownWrite(int sockfd);
  static void setTcpNoDelay(int fd, bool isEnable);
  static void getAddrAnyIpv4(struct sockaddr_in &addrIn, uint16_t port);
  static bool toAddrIpv4(const std::string &addrIp, struct sockaddr_in &addrIn);
  static bool toAddrIpv4(const std::string &addr, uint16_t port,
                         struct sockaddr_in &addrIn);
  static bool toAddrIpv4(uint16_t port, struct sockaddr_in &addrIn);

static  bool toAddrIpv4T(const char *hostname, int port, struct sockaddr_in &addrIn);

  static std::string ipToString(struct sockaddr_in addr);
  static std::string toString(struct sockaddr_in addr);

  template <typename T> static bool stringToInt(const std::string &str, T &num);
  // private:
};

} // namespace net
} // namespace eomaia

#endif // EOMAIA_SOCKET_OPREATION
