# 密码学工具箱

本节将对上一节提到的，密码学中六种主要的密码技术分别顺序进行描述，并同时配以此种技术中的一个或多个样例代码。

## 单向散列函数 SHA256

单向散列函数根据消息的内容计算出消息的散列值，散列值作为消息的摘要，可以用来检查消息的完整性(Integrity)。同一个单向散列函数算法输出的散列值是固定的，不随消息长度的不同而变化。常见的单向散列算法有 MD4/5 系列与 SHA 系列。MD4/5 系列算法由于安全性问题而不应该使用，应该主要使用 SHA 系列算法。MD5 与 SHA 算法家族均基于 MD4 的基本原理设计实现，mbedtls 的单向散列通用接口均赋以 md 前缀可能就源于此。由单向散列函数的名称可知，`单向`意味着从消息的散列值反推消息本身，在安全的单向散列函数中几乎是不可能的事情。单向散列函数和其他密码技术息息相关，许多其他密码技术都用到了单向散列函数。

在消息传递过程中，常将消息本身与计算出的消息散列值一同发送给对方，对方在接收到消息后计算其散列值，并与接收到的散列值比较，如果二者相同，则说明消息没有经过篡改或者产生传输错误。

如下算法计算消息"abc"的散列值，采用 SHA256 算法。其计算流程为经典的 start->update->finish 模式。填入消息在 update 步骤进行，update 函数可以被调用多次以多次填入消息。

[sha256](../src/libmbedtls/sha256.c ':include')

除此之外，新推出的 SHA-3 算法与以往的 MD 系列的单向散列算法内部结构完全不同，采用海绵结构可以让 SHA-3 算法免疫针对 SHA-1 系列的攻击。

## 对称加密算法 AES_128_CBC / AES_128_CTR

> ECB/CBC 模式本身是脆弱的，不要在实际场景使用。除此之外，单独使用流密码模式已经被证实是不安全的，实际场景中需要使用 AEAD 模式<sup>[[1]](https://shadowsocks.org/en/wiki/Stream-Ciphers.html)</sup>。此小节仅为教学用途，不要在实际场景中使用。

高级加密标准 AES 算法是当前流行的对称加密算法，用以保证消息的机密性(Confidentiality)。对称加密算法的双方使用相同的密进行加密或解密，发送方使用共享密钥加密消息并传输密文，接收方使用共享密钥解密接收到的密文。

对称加密算法如 AES 单次只能处理一个固定长度的分组数据，单次只能加密或解密 128 位数据，而实际场景中被操作的消息通常都不是 128 位的整数倍(注意，如 AES_128_CBC 中的 128 指的是密钥长度，并不是分组长度)。为了解决这个问题，需要使用到`分组密码模式`与`消息填充方法`两种技术。在需要加密的明文长度超出分组长度时，就需要对明文进行分组处理，然后分别对各组进行加解密。不同分组模式的计算过程不同，安全性也不同。

分组模式有多种，如 ECB/CBC/CFB/OFB/CTR 等等。所有分组模式中的密码算法又分为分组密码与流密码。前两种为分组密码，后三种为流密码。流密码还有 chacha20/rc4-md5 等。

如下代码使用 mbedtls 的 cipher 通用接口实现 AES_128_CBC 与 AES_128_CTR 的加密过程。注意，CBC 模式进行了填充，而 CTR 模式不需要填充。

[aes128cbc_ctr](../src/libmbedtls/aes128cbc_ctr.c ':include')

## 消息认证码 HMAC / AES_128_GCM

通过单向散列函数实现的消息认证码，此类方法统称为 HMAC，如 HMAC-SHA1,HMAC-SHA256 等。另外一类方式为使用分组加密模式构造消息认证码算法，如 CBC-MAC\CMAC 等。最后，流密码与公钥非对称密码也可用来实现消息认证码。以上这些方法可以提供信息安全中的完整性与真实性(认证机制)的保障。

最后，更重要的是应用更广泛的 AE(Authenticated Encryption)/AEAD(Authenticated Encryption with Associated Data) 加密认证模式，如 AES_128_GCM。不同于单独的消息认证码，这种算法兼顾真实性与完整性，还可同时提供机密性的保障。GCM 使用 CTR 模式进行加密，同时使用散列函数 GHASH 进行 MAC 值计算。在 CTR 加密与 MAC 值计算时，使用的是相同的密钥。

HMAC 算法需要两个参数，一个称为秘钥，此处为 secret，另一个称为消息，此处为 msg，消息认证码保留在 mac 数组中。可以看到输出中 HMAC 消息认证码长度和内部的单向散列算法 SHA256 的消息摘要长度是相等的，所以 HMAC 的计算结果一定为 32 字节。在 mbedtls 中，消息认证码的生成分为三个步骤：

- mbedtls_md_hmac_starts 设置密钥
- mbedtls_md_hmac_update 填充消息，本示例仅填充了一次
- mbedtls_md_hmac_finish 生成消息认证码，结果保存至 mac 中

注意，在 mbedtls_md_setup 函数中赋值 1，为使用 HMAC。

[hmac](../src/libmbedtls/hmac.c ':include')

---

目前 shadowsocks 建议使用的 AEAD 模式中的 AES_128_GCM。认证加密算法可同时输出密文与消息认证码，如今已被广泛应用。

[aes128gcm](../src/libmbedtls/aes128gcm.c ':include')

## 伪随机数生成器

CTR_DRBG 是一种常用的伪随机数生成算法，它使用 AES-256 为生成器的基础算法，可以将其理解为一个加密过程，加密的结果为期望的随机数序列。

使用 CTR_DRBG 生成随机数时，基础流程是首先使用一个强熵源，硬件真随机数发生器一般被定义为强熵源。在 linux 上，mbedtls 默认使用/dev/urandom 作为强熵源，注意，如今/dev/random 和 /dev/urandom 已经近乎相似。如果某个平台没有默认的强熵源，则需要使用 mbedtls_entropy_add_source 函数进行添加自定义强熵源。在 mbedtls_ctr_drbg_seed 生成种子后，最后使用 mbedtls_ctr_drbg_random 生成随机数。

[ctr_drbg_random](../src/libmbedtls/ctr_drbg_random.c ':include')

使用 CTR_DRBG 生成大素数:

[ctr_drbg_big_prime](../src/libmbedtls/ctr_drbg_big_prime.c ':include')

## 公钥非对称密码算法 RSA

RSA 算法是一种常见的公钥密码算法，可用于公钥加密以及数字签名(本节后续会讲到)。与对称加密中共享密钥的方式不同，RSA 加密解密过程中使用不同的密钥。在 RSA 用于公钥加密时，公钥用来加密，私钥用来解密。

RSA 算法的加密过程比对称加密算法慢很多，通常不适合对大量数据进行加解密。这是因为 RSA 加解密的过程存在大量的模幂运算，解决方式是指定一个固定的短公开指数进行快速计算，短公开指数通常被标记为 e,常用的值为 3\7\65537。RSA 算法在实际应用中常用于加解密小数据片段，如密钥协商中密钥的配送：首先服务器传递公钥，客户端用公钥加密后续要用到的对称式加密的共享密钥，发送给服务器。服务器接收到后，用私钥解密，从而双方均得到对称式加密的共享密钥用于后续通信。

> RSA 算法没有[向前安全性](https://en.wikipedia.org/wiki/Forward_secrecy)，在 TLS1.3 中已经禁止使用 RSA 作为密钥协商算法。

RSA 算法通常也需要包含填充方案，通过填充动作把随机性注入明文中，这样在公钥和明文相同的情况下，密文也会不同。常用的填充方案为 OAEP。

注意，实际上非对称式加密中，私钥实际上已经包含了公钥的信息，可以通过如 openssl 等一些工具很方便的从私钥中提取公钥。对私钥进行 decode 并进行一些计算后也可得到公钥。在后续的一些使用中，如果看到需要密钥对的情况但是只使用了私钥，其实也是可以达到相同目的的。

下面的代码通过用公钥加密明文字符串，再用私钥解密密文，填充方式为 OAEP。填充过程需要使用到上一节描述过的伪随机数生成器。可以看到，在生成密钥对时，需要指定密钥长度(即模数，位表示)和公开短指数。

[rsa_encrypt](../src/libmbedtls/rsa_encrypt.c ':include')

## DH 密钥协商算法

DH 密钥协商算法是基于离散对数问题的密钥协商算法的另一种实现,其只能作为密钥协商算法，而不像 RSA 一样可以同时用于数据加解密以及身份认证。DH 无法解决认证问题，需要配合使用 RSA 或 ECDSA 数字签名来解决。

DH 密钥协商的流程如下:

1. 双方先选择相同的大素数 P 与生成元 G 作为共享参数。这两个参数非常重要，可以从标准文件 RFC 7919 中获取标准数据。为了保证安全性，大素数长度应与 RSA 的模数长度相同，实际应用中应不小于 2048bit。
2. 双方均生成一个随机密钥作为私密参数，通过模幂运算计算出要进行交换的公开参数。
3. 接收到对方的公开参数后，通过计算得出共享密钥，随后即可进行通信。

如下代码演示了 DH 密钥协商算法的实现流程。由于是模拟实现，并没有发生网络通信的步骤。可以看到最终生成的公开参数与共享密钥均为 2048bit。可以看到，DH 密钥协商的缺点在于密钥的尺寸太大，这在很多场景下是种限制。

[dh](../src/libmbedtls/dh.c ':include')

## ECDH 密钥协商算法

ECDH 密钥协商基于 ECC 椭圆密码系统，较短的密钥长度即可达到和 RSA 或 DH 算法同等级的强度，ECC 具有密钥尺寸短，安全性高等特点，是近些年密码学应用领域的研究热点。ECDH 也无法解决认证问题，需要配合使用 RSA 或 ECDSA 数字签名来解决。

首先需要初始化椭圆曲线的群结构、以及双方公钥对应的椭圆曲线点结构。接下来可以看到，ECDH 在指定标准椭圆曲线后可通过函数直接生成公钥和私钥，而不是像 DH 一样需要指定生成元和大素数的标准参数，因为实际上标准椭圆曲线已经包含了这些参数，椭圆曲线一般推荐使用标准的 secp256r1 与 secp384r1。客户端和服务端进一步通过对方的公钥直接计算共享密钥。

[ecdh](../src/libmbedtls/ecdh.c ':include')

## 数字签名 RSA/DSA/ECDSA

数字签名可以识别消息是否被篡改，保障完整性，同时也可以保证消息的真实性。这看起来和前面讲过的消息验证码是一样的作用。实际上，数字签名与消息验证码最大的区别在于，数字签名可以防止否认，因为数字签名的私钥只有签名者持有，而消息验证码的私钥由双方共享。

RSA 数字签名算法基于 RSA 密钥系统实现。RSA 数字签名过程与 RSA 加密解密有类似的地方，但是公钥和私钥的使用方式却有明显的区别。在 RSA 数字签名中，私钥用来加密消息，也称签名密钥。公钥用于解密消息，也称验证密钥。而在 RSA 加密解密中，公钥用于加密消息，私钥用于解密消息，这与 RSA 数字签名恰恰相反。不变的是，公钥总是可以公开的，私钥是不能公开的。

如下代码是 RSA 签名与验证的过程，可以看到一些步骤是和 RSA 加解密一样的。随后进行签名与验证的库函数调用。注意，RSA 数字签名存在潜在伪造的问题，解决方案是使用改进过的 RSA-PSS 签名，其对消息散列值签名，而不是对消息本身签名。除此之外，在计算散列值时还会对消息加盐。

[rsa_sign](../src/libmbedtls/rsa_sign.c ':include')

---

DSA 是另外一种数字签名方式。基于 DSA,如果计算过程在椭圆曲线群上完成，则是另外一种应用广泛的数字签名方式:ECDSA。ECDSA 数字签名和 ECDH 类似，因在椭圆曲线群上进行计算，从而较短的密钥长度即可达到 RSA 较长的密钥长度的安全强度。DSA 和 ECDSA 均只提供数字签名功能，不能用于加解密或密钥交换。

ECDSA 首先要对消息摘要进行结算，随后选择椭圆曲线来生成密钥对。接下来进行签名步骤，签名结果中重要的参数为 r 和 s。最后对使用公钥对签名进行验证，可以看到验证函数需要依次输入椭圆曲线结构体、消息、消息长度、公钥 ctx.Q、以及签名结果 r 和 s。

[ecdsa](../src/libmbedtls/ecdsa.c ':include')

## 数字证书 X.509

数字签名和公钥非对称算法使通信变得越来越安全，但是公钥的分发仍存在安全问题。如果攻击者将公钥替换，则可以很容易的对系统发起攻击,如常见的中间人攻击。数字签名过程中需要进行公钥的交换，安全的公钥交换方式为通过可信机构(CA)颁发证书、或者面对面交换。数字证书解决的就是公钥安全分发的问题，它其是用来证实公钥持有者身份的电子文件。数字证书包含公钥的信息，用户身份信息以及来自发行者的数字签名。

一个证书的颁发与验证流程如下:

1. 服务器端生成密钥对，用公钥生成证书签名请求(CSR)，将 CSR 发送给证书认证机构(CA)
2. CA 收到请求，生成服务器端证书，计算服务器端证书的哈希值，并用 CA 的私钥加密该哈希值，加密后的内容叫做签名(Signature)。注意数字证书包含两部分，服务器端公钥、以及用 CA 的私钥加密过的数字签名。
3. 服务器端收到 CA 下发的证书，并在有客户端请求时将自己的证书发给客户端。
4. 客户端收到服务器端证书，并请求 CA 证书，用 CA 证书中的 CA 公钥解密服务器端证书中的 CA 签名，获得一个哈希值。将这个哈希值与服务器端证书的哈希值进行比较，若一致则证明服务器端的证书是有效的。
5. 此时客户端已经可以使用服务器端公钥进行消息加密，服务器端使用自己的私钥进行解密。

如下代码展示一个验证服务器端证书的过程，mbedtls 的函数接口使用起来非常简单，它屏蔽了很多细节。mbedtls_x509_crt_verify 函数中需要指定一个回调函数，在验证成功或失败时进行调用，这里为 my_verify。

[cert](../src/libmbedtls/cert.c ':include')

---

Ref:

- [How to add entropy sources to the entropy pool](https://tls.mbed.org/kb/how-to/add-entropy-sources-to-entropy-pool)
- [mbedtls entropy_poll.c source code](https://github.com/ARMmbed/mbedtls/blob/55a4d938d055a44cdac6baab1b62d1dcc804da0a/library/entropy_poll.c)
- [Arch wiki: Random number generation](https://wiki.archlinux.org/title/Random_number_generation)
- [Is the CBC mode of operation a stream cipher mode?](https://crypto.stackexchange.com/questions/51251/is-the-cbc-mode-of-operation-a-stream-cipher-mode)
- [ShadowSocks 重定向攻击](http://iv4n.cc/shadowsocks/)
- [Redirect attack - Shadowsocks 流密码的不安全因素](https://blog.rexskz.info/redirect-attack-weakness-of-ss-stream-cipher.html#toc-link-2)
- [Block cipher mode of operation](https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation)
- [Authenticated_encryption](https://en.wikipedia.org/wiki/Authenticated_encryption)
