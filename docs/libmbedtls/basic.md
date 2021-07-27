# mbedtls 初步

mbedtls 是一个跨平台的 [TLS](https://en.wikipedia.org/wiki/Transport_Layer_Security)/[DTLS](https://en.wikipedia.org/wiki/Datagram_Transport_Layer_Security) 协议实施库，同时也是一个加密库与 X.509 证书处理库。对比与其他 TLS 实现如 OpenSSL，其更为针对嵌入式平台，因为它的体积可以很小。在 Arch Linux 上安装包[mbedtls](https://archlinux.org/packages/community/x86_64/mbedtls/)即可使用。

本章的主要目的是使用 mbedtls 来实践一些密码学工具，并以此使你可以得到对密码学相关知识的大体了解。本节先给出一些 mbedtls 的简单应用，代码和描述不能全部看懂也没关系，这仅仅是想使你可以得到一个较为直观的感受，后续小节会详细描述。

在编译本章代码时，需要根据需求添加-l 参数链接动态链接库，它们为`-lmbedtls`、`-lmbedx509`以及`-lmbedcrypto`，分别对应 mbedtls 的 tls 实现，X.509 证书处理以及加解密部分。

## 密码学简述

本书不会描述任何密码学相关理论知识，这是密码学家需要关心的事情，本书仅从程序员的角度讲述必须掌握的内容。在密码学中有六种主要的密码技术是在软件工程中经常使用的，这六种技术需要牢记，并理清它们之间的区别和关联：

- 单向散列函数
- 对称加密算法
- 消息认证算法
- 伪随机数生成器
- 公钥密码算法(非对称加密)
- 数字签名算法(数字证书)

这些技术的具体实现互相组合，就形成了安全框架，如 TLS 以及 DTLS。在本章中将详细描述这些技术应如何实现。

在信息安全领域，如下三点被认为是信息安全的核心基础:

- 机密性(Confidentiality)
- 完整性(Integrity)
- 可用性(Availability)

机密性确保信息未经未授权的查看与访问，这一般由加密算法来实现。完整性确保信息的内容是完整且正确的，这一般由单向散列函数来实现。可用性确保服务或信息须保持可用状况(能用)，并能满足使用需求(够用)。在本章中将详细描述如何确保这些信息安全基础要求。

## Base64 编码

Base64 并不是一种加解密算法，其为一种编码算法，结果没有任何保密性。但是其被广泛应用在对保密性要求不是太高的场景，也可以起到混淆视听的效果，如很多机场订阅均以 Base64 的形式下发。如下代码仅仅简单的调用了 mbedtls 中 base64 编码与解码的函数，将数组和 ASCII 编码进行互相的转换。

[base64](../src/libmbedtls/base64.c ':include')

## AES_ECB_128 加密解密

AES_ECB_128 是一种对称加密算法，这里使用 AES_ECB_128 对一条消息进行加密和解密。这种加密算法存在安全风险，这里仅作简单演示用，不要在实际场景使用 AES_ECB_128。

[aes_ecb](../src/libmbedtls/aes_ecb.c ':include')

## 遍历 mbedtls 安全套件

以下代码列出了默认情况下 mbedtls 所支持的全部用于网络通信的安全套件。可以看到默认有 127 项。拿 TLS-ECDHE-ECDSA-WITH-AES-256-GCM-SHA384 举例来说，它的各部分含义如下:

- 安全框架 TLS
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

- [tls.mbed.org Knowledge Base](https://tls.mbed.org/kb/how-to)
- [Comparison of TLS implementations](https://en.wikipedia.org/wiki/Comparison_of_TLS_implementations)
- [Comparison of cryptography libraries](https://en.wikipedia.org/wiki/Comparison_of_cryptography_libraries)
- [Arch wiki Mbed_TLS](https://wiki.archlinux.org/title/Mbed_TLS)
- [mbedtls 安装与入门](https://blog.csdn.net/xukai871105/article/details/72795126)
- [Confidentiality, Integrity and Availability – The CIA Triad](https://www.certmike.com/confidentiality-integrity-and-availability-the-cia-triad/)
- [Information security](https://en.wikipedia.org/wiki/Information_security)
