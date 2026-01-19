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

#include "rsa.hxx"

extern "C" {
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
};

#include <boost/algorithm/string/replace.hpp>
#include <boost/scope_exit.hpp>

#include <math.h>

#define SCOPE_FREE(X, f)                                                       \
  BOOST_SCOPE_EXIT(X) {                                                        \
    if (X != nullptr)                                                          \
      f(X);                                                                    \
  }                                                                            \
  BOOST_SCOPE_EXIT_END;

struct evp_pkey_st *openPublicKeyFile(const char *ca) {
  EVP_PKEY *pkey = nullptr;
  do {
#ifdef _WIN32
    auto local = setlocale(LC_ALL, ".UTF8");
    BOOST_SCOPE_EXIT(local) { ::setlocale(LC_ALL, local); }
    BOOST_SCOPE_EXIT_END;
#endif //  _WIN32
    auto hFile = fopen(ca, "rb");
    if (hFile == nullptr) {
      break;
    }
    SCOPE_FREE(hFile, fclose);
    OpenSSL_add_all_algorithms();

    auto x509 = PEM_read_X509(hFile, nullptr, nullptr, nullptr);
    if (x509 == nullptr) {
      break;
    }
    SCOPE_FREE(x509, X509_free);
    pkey = X509_get_pubkey(x509);
  } while (false);
  return pkey;
}

evp_pkey_st *openPublicKeyMem(const char *data, size_t bytes) {
  EVP_PKEY *pkey = nullptr;
  do {
    OpenSSL_add_all_algorithms();
    auto bio = BIO_new_mem_buf(reinterpret_cast<const void *>(data),
                               static_cast<int>(bytes));
    if (bio == nullptr) {
      break;
    }
    SCOPE_FREE(bio, BIO_free);
    auto x509 = PEM_read_bio_X509(bio, nullptr, nullptr, nullptr);
    if (x509 == nullptr) {
      break;
    }
    SCOPE_FREE(x509, X509_free);
    pkey = X509_get_pubkey(x509);
  } while (false);
  return pkey;
}

void pkey_free(evp_pkey_st *pkey) { EVP_PKEY_free(pkey); }

std::string encryptPubkey(struct evp_pkey_st *pKey, const std::string &src) {
  OpenSSL_add_all_ciphers();
  std::string out;
  do {
    auto keyCtx = EVP_PKEY_CTX_new(pKey, nullptr);
    SCOPE_FREE(keyCtx, EVP_PKEY_CTX_free);
    if (EVP_PKEY_encrypt_init(keyCtx) == -1) {
      break;
    }
    size_t outLen = 0;
    auto ret = EVP_PKEY_encrypt(
        keyCtx, nullptr, &outLen,
        reinterpret_cast<const unsigned char *>(src.data()), src.size());
    if (ret != 1) {
      break;
    }

    out.resize(outLen);

    ret = EVP_PKEY_encrypt(
        keyCtx, reinterpret_cast<unsigned char *>(out.data()), &outLen,
        reinterpret_cast<const unsigned char *>(src.data()), src.size());
    if (ret != 1) {
      out.clear();
    }
  } while (false);
  return std::move(out);
}
