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
#include <mbedtls/platform.h>
#include <mbedtls/ssl.h>

// Root CA cart pem
#define ca_cert_pem                                                            \
  "-----BEGIN CERTIFICATE-----\r\n"                                            \
  "MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB\r\n"       \
  "iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\r\n"       \
  "cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\r\n"       \
  "BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw\r\n"       \
  "MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV\r\n"       \
  "BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU\r\n"       \
  "aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy\r\n"       \
  "dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK\r\n"       \
  "AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B\r\n"       \
  "3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY\r\n"       \
  "tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/\r\n"       \
  "Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2\r\n"       \
  "VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT\r\n"       \
  "79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6\r\n"       \
  "c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT\r\n"       \
  "Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l\r\n"       \
  "c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee\r\n"       \
  "UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE\r\n"       \
  "Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd\r\n"       \
  "BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G\r\n"       \
  "A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF\r\n"       \
  "Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO\r\n"       \
  "VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3\r\n"       \
  "ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs\r\n"       \
  "8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR\r\n"       \
  "iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze\r\n"       \
  "Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ\r\n"       \
  "XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/\r\n"       \
  "qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB\r\n"       \
  "VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB\r\n"       \
  "L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG\r\n"       \
  "jjxDah2nGN59PRbxYvnKkKj9\r\n"                                               \
  "-----END CERTIFICATE-----\r\n"

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

  // Set the random engine and debug function.
  mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
  mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

  // Set the authentication mode. Here not checking anything.
  mbedtls_x509_crt_parse(&cacert, (unsigned char *)ca_cert_pem,
                         sizeof(ca_cert_pem));
  mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
  mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);

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

  /*
   * 4. Handshake
   */
  mbedtls_printf("  . Performing the SSL/TLS handshake...");
  fflush(stdout);

  while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
      mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n",
                     (unsigned int)-ret);
      goto exit;
    }
  }

  mbedtls_printf(" ok\n");

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