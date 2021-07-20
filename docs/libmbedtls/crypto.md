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

## 公钥非对称密码算法 RSA

[rsa_encrypt](../src/libmbedtls/rsa_encrypt.c ':include')

---

Ref:

1. [How to add entropy sources to the entropy pool](https://tls.mbed.org/kb/how-to/add-entropy-sources-to-entropy-pool)
2. [mbedtls entropy_poll.c source code](https://github.com/ARMmbed/mbedtls/blob/55a4d938d055a44cdac6baab1b62d1dcc804da0a/library/entropy_poll.c)
3. [Arch wiki: Random number generation](https://wiki.archlinux.org/title/Random_number_generation)
