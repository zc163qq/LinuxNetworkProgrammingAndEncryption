# 密码学工具箱

## 单项散列函数 SHA256

[sha256](../src/libmbedtls/sha256.c ':include')

## 对称加密算法 AES_128_CBC / AES_128_CTR

[aes128cbc_ctr](../src/libmbedtls/aes128cbc_ctr.c ':include')

注意，CBC 模式进行了填充，而 CTR 模式不需要填充。

## 消息认证码 HMAC / AES_128_GCM

[hmac](../src/libmbedtls/hmac.c ':include')

可以看到输出中 HMAC 消息认证码长度和内部的单项散列算法 SHA256 的消息摘要长度是相等的，均为 32 字节。

---

[aes128gcm](../src/libmbedtls/aes128gcm.c ':include')

目前 shadowsocks 建议使用的 AEAD 模式中的 AES_128_GCM。

## 伪随机数生成器

CTR_DRBG 生成随机数

[ctr_drbg_random](../src/libmbedtls/ctr_drbg_random.c ':include')

CTR_DRBG 生成大素数

[ctr_drbg_big_prime](../src/libmbedtls/ctr_drbg_big_prime.c ':include')

## 公钥非对称密码算法 RSA/DH/ECDH

[rsa_encrypt](../src/libmbedtls/rsa_encrypt.c ':include')

[dh](../src/libmbedtls/dh.c ':include')

[ecdh](../src/libmbedtls/ecdh.c ':include')

ECDH 密钥协商基于 ECC 椭圆密码系统，较短的密钥长度即可达到和 RSA 或 DH 算法同等级的强度。

可以看到，ECDH 在指定标准椭圆曲线后可通过函数直接生成公钥和私钥，而不是像 DH 一样需要指定生成元和大素数的标准参数。客户端和服务端进一步通过对方的公钥直接计算会话密钥。

DH 与 ECDH 均无法解决认证问题，需要配合使用 RSA 或 ECDSA 数字签名来解决。

## 数字签名 RSA/DSA/ECDSA

RSA 数字签名算法基于 RSA 密钥系统实现。RSA 数字签名过程与 RSA 加密解密有类似的地方，但是公钥和私钥的使用方式却有明显的区别。在 RSA 数字签名中，私钥用来加密消息，也称签名密钥。公钥用于解密消息，也称验证密钥。而在 RSA 加密解密中，公钥用于加密消息，私钥用于解密消息，这与 RSA 数字签名恰恰相反。不变的是，公钥总是可以公开的，私钥是不能公开的。

[rsa_sign](../src/libmbedtls/rsa_sign.c ':include')

ECDSA 数字签名和 ECDH 类似，计算过程在椭圆曲线群上完成，从而较短的密钥长度即可达到 RSA 较长的密钥长度的安全强度。DSA 和 ECDSA 均只提供数字签名功能，不能用于加解密或密钥交换。

[ecdsa](../src/libmbedtls/ecdsa.c ':include')

注意，ECDSA 首先要对消息摘要进行结算。生成的密钥结果中重要的参数为 ctx.Q 与 ctx.d。签名结果中重要的参数为 r 和 s。

---

Ref:

1. [How to add entropy sources to the entropy pool](https://tls.mbed.org/kb/how-to/add-entropy-sources-to-entropy-pool)
2. [mbedtls entropy_poll.c source code](https://github.com/ARMmbed/mbedtls/blob/55a4d938d055a44cdac6baab1b62d1dcc804da0a/library/entropy_poll.c)
3. [Arch wiki: Random number generation](https://wiki.archlinux.org/title/Random_number_generation)
