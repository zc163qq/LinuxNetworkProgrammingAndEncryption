# libmbedtls 初步

mbed TLS 是一个跨平台的 TLS 协议实施库，同时也是一个加密库与 X.509 证书处理库。对比与其他 TLS 实现，其更为针对嵌入式平台，因为它的体积可以很小。在 Arch Linux 上安装包[mbedtls](https://archlinux.org/packages/community/x86_64/mbedtls/)即可使用。

本节先给出一些 mbedtls 的简单应用，使得可以得到一个较为直观的感受。

在编译本章代码时，需要根据需求添加-l 参数链接动态链接库，它们分别为`-lmbedtls`、`-lmbedx509`以及`-lmbedcrypto`。

## Base64 编码

Base64 并不是一种加解密算法，其为一种编码算法，结果没有任何保密性。但是其被广泛应用在对保密性要求不是太高的场景，也可以起到混淆视听的效果，如很多机场订阅均以 Base64 的形式下发。

[base64](../src/libmbedtls/base64.c ':include')

## AES_ECB_128 加密解密

这里使用 aes-ecb-128 对一条消息进行加密和解密，这里仅作简单演示用，不要在实际场景使用 aes-ecb-128。

[aes_ecb](../src/libmbedtls/aes_ecb.c ':include')

## 遍历 mbedtls 安全套件

以下代码列出了默认情况下 mbedtls 所支持的全部用于网络通信的安全套件。可以看到默认有 127 项。拿 TLS-ECDHE-ECDSA-WITH-AES-256-GCM-SHA384 举例来说:

- 密钥协商算法 ECDHE
- 身份认证算法 ECDSA
- 对称加密算法 AES_256
- 消息认证算法 GCM
- 伪随机数算法 SHA384

[ciphersuite_list](../src/libmbedtls/ciphersuite_list.c ':include')

## 大数运算

大数运算是密码学中广泛应用的手段，是公钥密码和数字签名算法的基础。以下代码分别进行乘法运算、模指数运算以及模逆运算

[bignum](../src/libmbedtls/bignum.c ':include')

---

Ref:

1. [tls.mbed.org](https://tls.mbed.org/kb/how-to)
2. [Comparison of TLS implementations](https://en.wikipedia.org/wiki/Comparison_of_TLS_implementations)
3. [Comparison of cryptography libraries](https://en.wikipedia.org/wiki/Comparison_of_cryptography_libraries)
4. [Arch wiki Mbed_TLS](https://wiki.archlinux.org/title/Mbed_TLS)
5. [mbedtls 安装与入门](https://blog.csdn.net/xukai871105/article/details/72795126)
6. [OpenSSL 简单思路和函数笔记](https://segmentfault.com/a/1190000005933931)
7. [mbedTLS 简单思路和函数笔记](https://segmentfault.com/a/1190000005998141)
