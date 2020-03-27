#include <openssl/err.h>
#include <openssl/ssl.h>
#include <net/SSL.h>

using namespace eomaia::net;

void init_openssl() {
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl() { EVP_cleanup(); } // TODO

SSL_CTX *create_context() {
  const SSL_METHOD *method;
  SSL_CTX *ctx;

  method = SSLv23_server_method();

  ctx = SSL_CTX_new(method);
  if (!ctx) {
    perror("Unable to create SSL context");
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }

  return ctx;
}

void configure_context(SSL_CTX *ctx) {
  SSL_CTX_set_ecdh_auto(ctx, 1);

  /* Set the key and cert */
  if (SSL_CTX_use_certificate_file(ctx, "/home/shen/c/cert.pem", SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }

  if (SSL_CTX_use_PrivateKey_file(ctx, "/home/shen/c/key.pem", SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }
}

void SSLOperation::initServerCTX() {
  client = false;
  init_openssl();
  ctx = create_context();
  configure_context(ctx);
}



SSL_CTX *InitCTX(void) {
  const SSL_METHOD *method;
  SSL_CTX *ctx;

  OpenSSL_add_all_algorithms();   /* Load cryptos, et.al. */
  SSL_load_error_strings();       /* Bring in and register error messages */
  method = SSLv23_method(); /* Create new client-method instance */
  ctx = SSL_CTX_new(method);      /* Create new context */
  if (ctx == NULL) {
    ERR_print_errors_fp(stderr);
    printf("Eroor: %s\n", stderr);
    abort();
  }
  return ctx;
}

void ShowCerts(SSL *ssl) {
  X509 *cert;
  char *line;
  cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
  if (cert != NULL) {
    printf("Server certificates:\n");
    line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    printf("Subject: %s\n", line);
    free(line); /* free the malloc'ed string */
    line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
    printf("Issuer: %s\n", line);
    free(line);      /* free the malloc'ed string */
    X509_free(cert); /* free the malloc'ed certificate copy */
  } else
    printf("No certificates.\n");
}

void SSLOperation::initClientCTX(){
  client = true;
  SSL_library_init();
  ctx = InitCTX();
}


void SSLOperation::clean_CTX(){
  SSL_CTX_free(ctx); /* release context */
  if(!isClient()){
    cleanup_openssl();
  }
}
SSL_CTX * SSLOperation::ctx = nullptr;
bool SSLOperation::client = false;;
