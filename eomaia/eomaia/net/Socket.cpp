#include <arpa/inet.h>
#include <base/LogInterface.h>
#include <net/SSL.h>
#include <net/Socket.h>
#include <net/SocketOperation.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

  using namespace eomaia::net;

// 创建 Socket 的时机:
// 类 Socket 实际上持有的内容就是 fd
// fd 的创建来自于两个位置 : 一个是TcpAccept : Server 主动创建用于接受资源的
// 所以 Socket 的创建也是在这两个位置。

Socket::Socket(int fd) : socketFd(fd) {
#ifdef CONFIG_SSL
  if (!SSLOperation::isClient()) {
    auto ctx = SSLOperation::getCTX();
    ssl = SSL_new(ctx);        /* create new SSL connection state */
    SSL_set_fd(ssl, socketFd); /* attach the socket descriptor */
    if (SSL_accept(ssl) <= 0) {
      ERR_print_errors_fp(stderr);
    }
  }
#endif
}

Socket::~Socket() {
  SocketOperation::close(socketFd);
#ifdef CONFIG_SSL
  SSL_shutdown(ssl);
  SSL_free(ssl);
#endif
}

int Socket::getFd() { return socketFd; }

ssize_t Socket::write(const void *buf, size_t count) {
#ifdef CONFIG_SSL
  return SSL_write(ssl, buf, count);
#else
  return SocketOperation::write(socketFd, buf, count);
#endif
}

ssize_t Socket::read(void *base, size_t count) {
#ifdef CONFIG_SSL
  return SSL_read(ssl, base, count);
#else
  return SocketOperation::read(socketFd, base, count);
#endif
}

void Socket::bind(SocketAddr &addr) {
  SocketOperation::bind(socketFd, addr.getAddrPtr());
}

/* 调用位置 TcpAccept::acceptHandle */
int Socket::accept(SocketAddr &addr) {
  struct sockaddr_in clientAddr;
  int rst = SocketOperation::accept(socketFd, &clientAddr);
  if (rst > 0) {
    addr.setAddr(clientAddr);
    // 并不能在此处使用 SSL_accept :
    // 从设计上，所有的 SSL 初始化完成的位置都是在 Socket 创建的过程中间
    // 此时使用的是 ssl 变量
  }

  return rst;
}

void Socket::listen() { SocketOperation::listen(socketFd); }

void Socket::setTcpNoDelay(bool enable) {
  SocketOperation::setTcpNoDelay(socketFd, enable);
}

int Socket::shutdownWrite() { return SocketOperation::shutdownWrite(socketFd); }

void Socket::connect(struct sockaddr *addr) {
#ifdef CONFIG_SSL
  if (!SSLOperation::isClient()) {
    std::cerr << "This function can be called by Client !" << std::endl;
    abort();
  }
#endif

  if (SocketOperation::connect(socketFd, addr) != 0) {
    close(socketFd);
    base::Log::OUT(base::Log::Error) << "Failed to connect to server !\n";
    abort();
  }
#ifdef CONFIG_SSL
  auto ctx = SSLOperation::getCTX();
  ssl = SSL_new(ctx);        /* create new SSL connection state */
  SSL_set_fd(ssl, socketFd); /* attach the socket descriptor */
  if (SSL_connect(ssl) == -1) {
    printf("Eroor: %s\n", stderr);
    ERR_print_errors_fp(stderr);
  }
#endif
}
