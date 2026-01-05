#include "query.hxx"
#include "string.hxx"

#include <iterator>
#include <vector>

#include <curl/curl.h>

#include <fmt/format.h>
namespace Anycubic::utility {
void append(query_type &) {}

static std::string escape(const std::string &src) {
  auto pSrc = ::curl_escape(src.data(), static_cast<int>(src.size()));

  std::string res(pSrc);

  ::curl_free(pSrc);
  return res;
}
static std::string unescape(const std::string &src) {
  auto pSrc = ::curl_unescape(src.data(), static_cast<int>(src.size()));
  std::string res(pSrc);
  ::curl_free(pSrc);
  return res;
}

std::string make_query(const query_type &query) {

  std::vector<std::string> out;
  out.resize(query.size());
  std::transform(query.begin(), query.end(), out.begin(),
                 [](const query_type::value_type &kv) {
                   auto v = fmt::format("{}={}", escape(kv.first),
                                        escape(kv.second));
                   return std::move(v);
                 });
  return Anycubic::utility::Join(out, "&");
}

query_type parse_query(const std::string &query) {
  query_type q;
  std::string key;
  std::string value;
  std::string *pCurrent = &key;

  for (auto ch : query) {
    if (ch == '?') {
      continue;
    } else if (ch == '=') {
      pCurrent = &value;
      continue;
    } else if (ch == '&') {

      q.emplace(unescape(key), unescape(value));
      key.clear();
      value.clear();
      pCurrent = &key;
      continue;
    } else {
      pCurrent->push_back(ch);
    }
  }
  q.emplace(unescape(key), unescape(value));
  return q;
}
} // namespace Anycubic::utility
