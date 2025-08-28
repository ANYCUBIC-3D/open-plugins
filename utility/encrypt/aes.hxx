#pragma once

#include <string>
/**
 * @brief AES加密实现（CBC模式+PBKDF2密钥派生）
 * @param plaintext 明文数据
 * @param password 加密口令
 * @return std::string 加密数据(格式：盐+IV+密文)
 */
std::string aesEncrypt(const std::string_view &plaintext,
                       const std::string_view &password);

/**
 * @brief AES解密实现（CBC模式+PBKDF2密钥派生）
 * @param ciphertext 加密数据(格式：盐+IV+密文)
 * @param password 解密口令
 * @return std::string 解密后的原始数据
 */
std::string aesDecrypt(const std::string_view &ciphertext,
                       const std::string_view &password);
