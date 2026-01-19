// Copyright (c) [Year] [name of copyright holder]
// Open-Plugin is licensed under Mulan PSL v2.
// You can use this software according to the terms and conditions of the Mulan
// PSL v2.
// You may obtain a copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
// See the Mulan PSL v2 for more details.

#include "base64.hxx"

#include <assert.h>
extern "C" {
#include <openssl/evp.h>
}

#include <boost/algorithm/string/replace.hpp>
#include <boost/scope_exit.hpp>

#include <math.h>

std::string base64Encode(const std::string &src) {
  assert(!src.empty());

  // 计算编码后数据的最大长度
  size_t bufferLength = src.size();
  // Base64 编码会将每 3 个字节转换为 4 个字符
  bufferLength = ((bufferLength + 2) / 3) * 4;

  std::string out;
  out.resize(bufferLength);

  auto pBuffer = reinterpret_cast<unsigned char *>(out.data());

  // 使用 EVP_EncodeBlock 进行编码
  int result = ::EVP_EncodeBlock(
      pBuffer, reinterpret_cast<const unsigned char *>(src.data()),
      static_cast<int>(src.size()));

  if (result == -1) {
    // 失败了
    return std::string();
  }

  // EVP_EncodeBlock 返回的是编码后的实际字符数
  out.resize(result);
  return out;
}

std::string base64Decode(const std::string &src) {
  assert(!src.empty());

  // 计算解码后数据的最大长度
  size_t bufferLength = src.size();

  std::string out;
  out.resize(bufferLength);

  auto pBuffer = reinterpret_cast<unsigned char *>(out.data());

  // 使用 EVP_DecodeBlock 进行解码
  // 根据 OpenSSL 文档，EVP_DecodeBlock 会自动处理填充字符
  int result = ::EVP_DecodeBlock(
      pBuffer, reinterpret_cast<const unsigned char *>(src.data()),
      static_cast<int>(src.size()));

  if (result == -1) {
    // 失败了
    return std::string();
  }

  // EVP_DecodeBlock 返回的就是解码后的实际字节数
  out.resize(result);
  return out;
}