#include "aes.hxx"
#include <boost/scope_exit.hpp>
#include <openssl/evp.h>
#include <openssl/rand.h>

#define PBKDF2_ITERATIONS 10000
#define SALT_SIZE 16
#define IV_SIZE 16

#define SCOPE_FREE(X, f)                                                       \
  BOOST_SCOPE_EXIT(X) {                                                        \
    if (X != nullptr)                                                          \
      f(X);                                                                    \
  }                                                                            \
  BOOST_SCOPE_EXIT_END;

static bool generate_salt(unsigned char *salt) {
  return ::RAND_bytes(salt, SALT_SIZE) == 1;
}

static bool derive_key(const std::string_view &password,
                       const unsigned char *salt, unsigned char *key,
                       int key_size) {
  return ::PKCS5_PBKDF2_HMAC_SHA1(
             password.data(), static_cast<int>(password.size()), salt,
             SALT_SIZE, PBKDF2_ITERATIONS, key_size, key) == 1;
}

std::string aesEncrypt(const std::string_view &plaintext,
                       const std::string_view &password) {
  EVP_CIPHER_CTX *ctx = ::EVP_CIPHER_CTX_new();
  SCOPE_FREE(ctx, ::EVP_CIPHER_CTX_free);
  std::string ciphertext;

  unsigned char salt[SALT_SIZE];
  unsigned char iv[IV_SIZE];
  unsigned char key[32]; // AES-256
  do {
    // 生成盐和IV
    if (!::generate_salt(salt) || ::RAND_bytes(iv, IV_SIZE) != 1)
      break;

    // 密钥派生
    if (!::derive_key(password, salt, key, sizeof(key)))
      break;

    // 加密初始化
    if (::EVP_EncryptInit_ex(ctx, ::EVP_aes_256_cbc(), nullptr, key, iv) != 1)
      break;
    ::EVP_CIPHER_CTX_set_padding(ctx, 1); // 1启用填充（默认），0禁用
    // 构建输出格式：盐(16) + IV(16) + 密文
    ciphertext.resize(SALT_SIZE + IV_SIZE + plaintext.size() +
                      ::EVP_CIPHER_CTX_block_size(ctx));
    auto buffer = reinterpret_cast<unsigned char *>(ciphertext.data());

    // 写入盐和IV
    ::memcpy(buffer, salt, SALT_SIZE);
    ::memcpy(buffer + SALT_SIZE, iv, IV_SIZE);

    int len = 0;
    // 加密数据
    if (::EVP_EncryptUpdate(
            ctx, buffer + SALT_SIZE + IV_SIZE, &len,
            reinterpret_cast<const unsigned char *>(plaintext.data()),
            plaintext.size()) != 1)
      break;
    int total_len = len;

    // 处理最后的数据块
    if (::EVP_EncryptFinal_ex(ctx, buffer + SALT_SIZE + IV_SIZE + len, &len) !=
        1)
      break;
    total_len += len;

    // 调整最终尺寸
    ciphertext.resize(SALT_SIZE + IV_SIZE + total_len);
  } while (false);

  return std::move(ciphertext);
}

std::string aesDecrypt(const std::string_view &ciphertext,
                       const std::string_view &password) {
  EVP_CIPHER_CTX *ctx = ::EVP_CIPHER_CTX_new();
  SCOPE_FREE(ctx, ::EVP_CIPHER_CTX_free);
  std::string plaintext;

  do {
    if (ciphertext.size() <= SALT_SIZE + IV_SIZE)
      break;

    // 提取盐和IV
    const auto *salt =
        reinterpret_cast<const unsigned char *>(ciphertext.data());
    const auto *iv = salt + SALT_SIZE;
    const auto *encrypted = iv + IV_SIZE;

    // 派生密钥
    unsigned char key[32];
    if (!::derive_key(password, salt, key, sizeof(key)))
      break;

    // 解密初始化
    if (auto v = ::EVP_aes_256_cbc();
        ::EVP_DecryptInit_ex(ctx, v, nullptr, key, iv) != 1)
      break;
    ::EVP_CIPHER_CTX_set_padding(ctx, 1); // 1启用填充（默认），0禁用
    // 准备缓冲区
    plaintext.resize(ciphertext.size() - SALT_SIZE - IV_SIZE);
    auto buffer = reinterpret_cast<unsigned char *>(plaintext.data());

    int len = 0;
    if (::EVP_DecryptUpdate(
            ctx, buffer, &len, encrypted,
            static_cast<int>(ciphertext.size() - SALT_SIZE - IV_SIZE)) != 1)
      break;
    int total_len = len;

    if (::EVP_DecryptFinal_ex(ctx, buffer + len, &len) != 1)
      break;
    total_len += len;

    plaintext.resize(total_len);
  } while (false);

  return std::move(plaintext);
}
