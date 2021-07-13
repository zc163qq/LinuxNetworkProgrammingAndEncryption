#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Mbed TLS
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>
#include <mbedtls/entropy.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>

#define SERVER_PORT "7788"
#define SERVER_NAME "ifuckgfw.com"
#define GET_REQUEST "GET / HTTP/1.0\r\n\r\n"

static void my_debug(void *ctx, int level, const char *file, int line,
                     const char *str) {
  ((void)level);

  fprintf((FILE *)ctx, "%s:%04d: %s", file, line, str);
  fflush((FILE *)ctx);
}

int main(void) {

  int ret, len;
  unsigned char buf[1024];

  /*
   * creation and initialization of the Mbed TLS structures
   */

  mbedtls_net_context server_fd;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config conf;

  mbedtls_x509_crt cacert;

  const char *pers = "ssl_client1";

  mbedtls_net_init(&server_fd);
  mbedtls_ssl_init(&ssl);
  mbedtls_ssl_config_init(&conf);
  mbedtls_x509_crt_init(&cacert);
  mbedtls_ctr_drbg_init(&ctr_drbg);

  mbedtls_entropy_init(&entropy);
  if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                   (const unsigned char *)pers,
                                   strlen(pers))) != 0) {
    printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
    goto exit;
  }

  /*
   * Start the connection with mbedtls
   */
  printf("\n  . Connecting to tcp/%s/%4s...", SERVER_NAME, SERVER_PORT);
  fflush(stdout);

  if ((ret = mbedtls_net_connect(&server_fd, SERVER_NAME, SERVER_PORT,
                                 MBEDTLS_NET_PROTO_TCP)) != 0) {
    printf(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
    goto exit;
  }

  /*
   * Configuring SSL/TLS
   */

  // Set the endpoint and transport type and security parameters.
  if ((ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
                                         MBEDTLS_SSL_TRANSPORT_STREAM,
                                         MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
    printf(" failed\n ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
    goto exit;
  }

  // Set the authentication mode. Here not checking anything.
  mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
  // mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);

  // Set the random engine and debug function.
  mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
  mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

  // Set up the SSL context to use it.
  if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
    printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
    goto exit;
  }
  if ((ret = mbedtls_ssl_set_hostname(&ssl, SERVER_NAME)) != 0) {
    printf(" failed\n ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
    goto exit;
  }

  // Set Input and output functions it needs to use for sending out network
  // traffic.
  mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv,
                      NULL);

  printf(" ok\n");

  /*
   * Reading and writing data
   */

  /*
   * Write the GET request
   */
  printf("  > Write to server:");
  fflush(stdout);

  len = sprintf((char *)buf, GET_REQUEST);

  while ((ret = mbedtls_ssl_write(&ssl, buf, len)) <= 0) {
    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
      printf(" failed\n  ! write returned %d\n\n", ret);
      goto exit;
    }
  }

  len = ret;
  printf(" %d bytes written\n\n%s", len, (char *)buf);

  /*
   * Read the HTTP response
   */
  printf("  < Read from server:");
  fflush(stdout);

  do {
    len = sizeof(buf) - 1;
    memset(buf, 0, sizeof(buf));
    ret = mbedtls_ssl_read(&ssl, buf, len);

    if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
      continue;

    if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
      break;

    if (ret < 0) {
      printf("failed\n  ! mbedtls_ssl_read returned %d\n\n", ret);
      break;
    }

    if (ret == 0) {
      printf("\n\nEOF\n\n");
      break;
    }

    len = ret;
    printf(" %d bytes read\n\n%s", len, (char *)buf);

  } while (1);

exit:

  /*
   * Teardown
   */
  mbedtls_net_free(&server_fd);

  mbedtls_x509_crt_free(&cacert);
  mbedtls_ssl_free(&ssl);
  mbedtls_ssl_config_free(&conf);
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);

  return (ret);
}