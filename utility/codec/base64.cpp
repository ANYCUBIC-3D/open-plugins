#include "base64.hxx"

#include <assert.h>
extern "C" {
#include <openssl/evp.h>
}

#include <boost/algorithm/string/replace.hpp>
#include <boost/scope_exit.hpp>

#include <math.h>

std::string base64Encode(const std::string_view &src) {
  assert(!src.empty());
  auto bufferLength = src.size();
  // base64 会把数据变长1/3,搞不了精确就大概就好了
  bufferLength =
      static_cast<size_t>(::ceil(static_cast<double>(bufferLength) * 1.4));
  std::string out;
  out.resize(bufferLength);
  do {
    auto encode = ::EVP_ENCODE_CTX_new();
    if (encode == nullptr) {
      out.clear();
      break;
    }

    BOOST_SCOPE_EXIT(encode) { ::EVP_ENCODE_CTX_free(encode); }
    BOOST_SCOPE_EXIT_END

    auto pBuffer = reinterpret_cast<unsigned char *>(out.data());
    int bytes = 0;
    int total = 0;
    ::EVP_EncodeInit(encode);

    if (int value = ::EVP_EncodeUpdate(
            encode, pBuffer, &bytes,
            reinterpret_cast<const unsigned char *>(src.data()),
            static_cast<int>(src.size()));
        value == -1) {
      break;
    }

    total += bytes;
    ::EVP_EncodeFinal(encode, pBuffer + bytes, &bytes);
    total += bytes;

    out.resize(total);
  } while (false);
  boost::algorithm::replace_all(out, "\n", "");
  return out;
}