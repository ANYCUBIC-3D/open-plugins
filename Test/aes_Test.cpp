#include "gtest/gtest.h"

#include "utility/encrypt/aes.hxx"
#include <utility/codec/base64.hxx>

TEST(AesTest, EncryptDecrypt) {
  const std::string password = "0123456789012345";
  const std::string plaintext = "Hello, World!";

  auto encrypted = aesEncrypt(plaintext, password);
  auto base64_encrypted = base64Encode(encrypted);

  auto decrypted = aesDecrypt(base64Decode(base64_encrypted), password);
  EXPECT_EQ(decrypted, plaintext);
}
