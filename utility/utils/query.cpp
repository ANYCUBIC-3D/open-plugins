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

  auto begin = query.begin();
  auto end = query.end();

  // 去掉可能存在的?
  if (*begin == '?') {
    begin++;
  }

  do {
    // 找key
    auto pos = std::find_if(begin, end, [](char ch) { return ch == '='; });
    if (pos == end) {
      break;
    }
    key.assign(begin, pos);
    // 找value
    begin = pos + 1;
    pos = std::find_if(begin, end, [](char ch) { return ch == '&'; });
    value.assign(begin, pos);
    q.emplace(unescape(key), unescape(value));
    if (pos == end) {
      break;
    }
    begin = pos + 1;
  } while (true);
  return q;
}
} // namespace Anycubic::utility
