# libmbedtls 初步

mbed TLS 是一个跨平台的 TLS 库，同时也是一个密码库。对比与其他 TLS 实现，其更为针对嵌入式平台，因为它的体积可以很小。在 Arch Linux 上安装包[mbedtls](https://archlinux.org/packages/community/x86_64/mbedtls/)即可使用。

首先附上一个简单的使用方法，对 mbedtls 有一个初步的认识。这里使用 aes-ecb-128 对一条消息进行加密和解密，这里仅作简单演示用，不要在实际场景使用 aes-ecb-128。

[aes_ecb](../src/libmbedtls/aes_ecb.c ':include')

Ref:

1. [Comparison of TLS implementations](https://en.wikipedia.org/wiki/Comparison_of_TLS_implementations)
2. [Comparison of cryptography libraries](https://en.wikipedia.org/wiki/Comparison_of_cryptography_libraries)
3. [Arch wiki Mbed_TLS](https://wiki.archlinux.org/title/Mbed_TLS)
