#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "mbedtls/gcm.h"
#include <mbedtls/cipher.h>
#include <mbedtls/platform.h>

int main() {
  mbedtls_gcm_context aes;
  char *key = "abcdefghijklmnop";
  char *input = "WTF AES GCM Example code!";
  char *iv = "abababababababab";
  char *add = "zxczxxxxxxxsdfadascxz";
  unsigned char output[64] = {0};
  unsigned char fin[64] = {0};
  puts("[i] Encrypted into buffer:");
  // init the context...
  mbedtls_gcm_init(&aes);
  // Set the key. This next line could have CAMELLIA or ARIA as our GCM mode
  // cipher...
  mbedtls_gcm_setkey(&aes, MBEDTLS_CIPHER_ID_AES, (const unsigned char *)key,
                     strlen(key) * 8);
  // Initialise the GCM cipher...
  mbedtls_gcm_starts(&aes, MBEDTLS_GCM_ENCRYPT, (const unsigned char *)iv,
                     strlen(iv), (const unsigned char *)add, strlen(add));
  // Send the intialised cipher some data and store it...
  mbedtls_gcm_update(&aes, strlen(input), (const unsigned char *)input, output);
  // Free up the context.
  mbedtls_gcm_free(&aes);
  printf("\n%zu\n", strlen((char *)output));
  for (int i = 0; i < strlen(input); i++) {
    char str[3];
    sprintf(str, "%02x", (int)output[i]);
    printf("%s", str);
  }
  printf("\n");
  puts("[i] Decrypted from buffer:");
  mbedtls_gcm_init(&aes);
  mbedtls_gcm_setkey(&aes, MBEDTLS_CIPHER_ID_AES, (const unsigned char *)key,
                     strlen(key) * 8);
  mbedtls_gcm_starts(&aes, MBEDTLS_GCM_DECRYPT, (const unsigned char *)iv,
                     strlen(iv), (const unsigned char *)add, strlen(add));
  mbedtls_gcm_update(&aes, 64, (const unsigned char *)output, fin);
  mbedtls_gcm_free(&aes);
  for (int i = 0; i < strlen(input); i++) {
    char str[3];
    sprintf(str, "%c", (int)fin[i]);
    printf("%s", str);
  }
  printf("\n");
}
