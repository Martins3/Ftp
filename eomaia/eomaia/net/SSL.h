#ifndef SSL_H_BVORDVJ9
#define SSL_H_BVORDVJ9

#include <openssl/err.h>
#include <openssl/ssl.h>
namespace eomaia {
namespace net {

class SSLOperation {
public:
  static void initServerCTX();
  static void initClientCTX();
  static SSL_CTX *getCTX() { return ctx; }
  static bool isClient(){return client;}

  static void clean_CTX(); // TODO 到底放在什么位置，析构函数吗 ?
private:
  static SSL_CTX *ctx;
  static bool client;
};
} // namespace net
} // namespace eomaia

#endif /* end of include guard: SSL_H_BVORDVJ9 */
