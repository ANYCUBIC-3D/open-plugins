#pragma once
#include <iterator>
#include <string>
namespace Anycubic::utility {
template <typename ContainerType>
std::string Join(const ContainerType &items, const std::string &delimiter) {
  if (std::empty(items))
    return {};

  auto it = std::begin(items);
  std::string result = *it++;

  for (; it != std::end(items); ++it) {
    result += delimiter;
    result += *it;
  }
  return result;
}
} // namespace Anycubic::utility