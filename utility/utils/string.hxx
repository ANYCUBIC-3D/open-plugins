#pragma once
#include <algorithm>
#include <iterator>
#include <string>

#ifndef NOT_WXWIDGETS
#include <wx/string.h>
#endif // NOT_WXWIDGETS
namespace Anycubic::utility {
/**
 * @brief 连接容器中的字符串
 *
 * @tparam ContainerType
 * 容器类型，必须是字符串容器（如std::vector<std::string>）
 * @param items 容器中的字符串项
 * @param delimiter 连接符，用于分隔每个字符串项
 * @return std::string 连接后的字符串
 */
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

/**
 * @brief 分割字符串
 *
 * @tparam OutputIt 输出迭代器类型，必须支持写入std::string
 * @param output 输出迭代器，用于存储分割后的子串
 * @param str 输入字符串，待分割的字符串
 * @param delimiter 分隔符，用于分割字符串
 * @return true 分割成功
 * @return false 分割失败（如空字符串或分隔符为空）
 */
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
#ifndef NOT_WXWIDGETS
/**
 * @brief 从UTF-8编码的字符串创建wxString
 *
 * @param str UTF-8编码的字符串
 * @return wxString wxString对象
 */
wxString FromUtf8(const std::string &str);

/**
 * @brief 将wxString转换为UTF-8编码的字符串
 *
 * @param str wxString对象
 * @return std::string UTF-8编码的字符串
 */
std::string ToUtf8(const wxString &str);
#endif // NOT_WXWIDGETS
} // namespace Anycubic::utility