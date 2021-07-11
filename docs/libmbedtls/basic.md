# libmbedtls 初步

mbed TLS 是一个跨平台的 TLS 协议实施库，同时也是一个加密库与 X.509 证书处理库。对比与其他 TLS 实现，其更为针对嵌入式平台，因为它的体积可以很小。在 Arch Linux 上安装包[mbedtls](https://archlinux.org/packages/community/x86_64/mbedtls/)即可使用。

## 使用 mbedtls 加密解密

首先附上一个简单的使用方法，对 mbedtls 有一个初步的认识。这里使用 aes-ecb-128 对一条消息进行加密和解密，这里仅作简单演示用，不要在实际场景使用 aes-ecb-128。

[aes_ecb](../src/libmbedtls/aes_ecb.c ':include')

## 使用 mbedtls 生成消息认证码

- hmac 算法需要两个参数，一个称为秘钥，此处为 secret，另一个称为消息，此处为 buffer
- 消息认证码保留在 digest 数组中
- 此处 hmac 算法选择 sha256 算法作为单向散列函数，所以 hmac 的计算结果一定为 32 字节。
- 在 mbedtls 中，消息认证码的生成分为三个步骤
  - mbedtls_md_hmac_starts 设置密钥
  - mbedtls_md_hmac_update 填充消息，本示例仅填充了一次
  - mbedtls_md_hmac_finish 生成消息认证码，结果保存至 digest 中

[hmac](../src/libmbedtls/hmac.c ':include')

---

Ref:

1. [Comparison of TLS implementations](https://en.wikipedia.org/wiki/Comparison_of_TLS_implementations)
2. [Comparison of cryptography libraries](https://en.wikipedia.org/wiki/Comparison_of_cryptography_libraries)
3. [Arch wiki Mbed_TLS](https://wiki.archlinux.org/title/Mbed_TLS)
4. [mbedtls 安装与入门](https://blog.csdn.net/xukai871105/article/details/72795126)
