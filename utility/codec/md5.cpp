// Copyright (c) 2026 深圳市纵维立方科技有限公司
// Open-Plugin is licensed under Mulan PSL v2.
// You can use this software according to the terms and conditions of the Mulan
// PSL v2.
// You may obtain a copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
// See the Mulan PSL v2 for more details.

#include "md5.hxx"

#include <assert.h>

extern "C" {
#include <openssl/md5.h>
#include <openssl/ssl.h>
}

#include <boost/nowide/cstdio.hpp>
#include <boost/scope_exit.hpp>

#if _WIN32
#include <locale.h>
#endif // _WIN32

void md5Sum(const char *src, size_t srcLen, char md5[MD5LEN]) {
  cMd5 ctx;
  ctx.write(src, srcLen);
  ctx.sum(md5);
}

bool md5File(const char *filename, char md5[MD5LEN]) {
  auto hFile = boost::nowide::fopen(filename, "rb");
  if (hFile == nullptr) {
    return false;
  }
  BOOST_SCOPE_EXIT(hFile) { ::fclose(hFile); }
  BOOST_SCOPE_EXIT_END
  std::vector<char> buffer(1024 * 1024);
  cMd5 ctx;
  do {

    if (auto size = ::fread(buffer.data(), 1, buffer.size(), hFile); size > 0) {
      ctx.write(buffer.data(), size);
    }
    if (::feof(hFile)) {
      break;
    }
  } while (true);
  ctx.sum(md5);
  return true;
}

void cMd5::write(const char *src, size_t srcLen) {
  ::EVP_DigestUpdate(evpCtx, src, srcLen);
}

void cMd5::sum(char md5[MD5LEN]) {
  static_assert(MD5LEN == MD5_DIGEST_LENGTH);
  unsigned int relen = 0;
  ::EVP_DigestFinal_ex(evpCtx, reinterpret_cast<unsigned char *>(md5), &relen);
}

cMd5::cMd5(void) {
  evpCtx = ::EVP_MD_CTX_new();
  ::EVP_DigestInit_ex(evpCtx, ::EVP_md5(), nullptr);
}

cMd5::~cMd5(void) { ::EVP_MD_CTX_free(evpCtx); }
