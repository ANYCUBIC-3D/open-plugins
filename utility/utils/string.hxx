#pragma once
#include <iterator>
#include <string>

#include <wx/string.h>
namespace Anycubic::utility {
template <typename ContainerType>
std::string Join(const ContainerType &items, const std::string &delimiter) {
  static_assert(std::is_same_v<typename ContainerType::value_type, std::string>,
                "ContainerType must be a container of strings");

  if (std::empty(items))
    return {};
  int total_size = 0;
  for (auto i : items) {
    total_size += i.size();
  }
  total_size +=
      (items.size() - 1) * delimiter.size(); // delimiter between items

  auto it = std::begin(items);
  std::string result;
  result.reserve(total_size);
  result.append(*it);
  std::for_each(std::next(it), std::end(items),
                [&result, &delimiter](const std::string &item) {
                  result.append(delimiter);
                  result.append(item);
                });
  return result;
}

template <typename OutputIt>
bool Split(OutputIt output, const std::string &str,
           const std::string &delimiter) {
  // 检查输入有效性
  if (str.empty() || delimiter.empty()) {
    return false;
  }

  // 单次遍历字符串，避免预先计算元素数量
  size_t start = 0;
  for (size_t end = str.find(delimiter); end != std::string::npos;
       end = str.find(delimiter, start)) {
    // 只添加非空子串
    if (end > start) {
      *output++ = std::string(str.begin() + start, str.begin() + end);
    }
    start = end + delimiter.length();
  }

  // 添加最后一个子串（如果存在）
  if (start < str.length()) {
    *output++ = std::string(str.begin() + start, str.end());
  }
  return true;
}
wxString FromUtf8(const std::string &str);
std::string ToUtf8(const wxString &str);

} // namespace Anycubic::utility