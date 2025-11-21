#pragma once

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <errno.h> // Windows 平台
#else
#include <sys/errno.h> // Unix-like 平台
#endif

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
namespace Anycubic::utility {
namespace detail {

struct Converter {
  template <typename To>
  static std::enable_if_t<std::is_arithmetic_v<To>, bool>
  convert(const char *from, To &to) {
    using result_type = std::decay_t<To>;

    char *end = nullptr;
    if constexpr (std::is_signed_v<To>) {
      auto result = strtol(from, &end, 10);
      // 检查转换是否完全成功
      if (from == end || (end && *end != '\0')) {
        return false;
      }
      // 检查溢出
      if (errno == ERANGE || result < std::numeric_limits<result_type>::min() ||
          result > std::numeric_limits<result_type>::max()) {
        return false;
      }
      to = static_cast<result_type>(result);
    } else if constexpr (std::is_unsigned_v<To>) {
      auto result = strtoul(from, &end, 10);
      // 检查转换是否完全成功
      if (from == end || (end && *end != '\0')) {
        return false;
      }
      // 检查溢出
      if (errno == ERANGE || result < std::numeric_limits<result_type>::min() ||
          result > std::numeric_limits<result_type>::max()) {
        return false;
      }
      to = static_cast<result_type>(result);
    } else if constexpr (std::is_floating_point_v<To>) {
      auto result = strtod(from, &end);
      // 检查转换是否完全成功
      if (from == end || (end && *end != '\0')) {
        return false;
      }
      // 检查溢出
      if (errno == ERANGE || result < std::numeric_limits<result_type>::min() ||
          result > std::numeric_limits<result_type>::max()) {
        return false;
      }
      to = static_cast<result_type>(result);
    } else {
      static_assert(std::is_arithmetic_v<To>, "To must be arithmetic type");
    }
    // 确保转换完全成功
    return end && *end == '\0';
  }
  template <typename To> static bool convert(const std::string &from, To &to) {
    return convert(from.c_str(), to);
  }

  template <typename From>
  static std::enable_if_t<std::is_arithmetic_v<From>, bool>
  convert(const From &from, std::string &to) {
    if constexpr (std::is_same_v<From, char>) {
      // 对 char 特殊处理
      to = {1, from};
    } else {
      to = std::to_string(from);
    }
    return true;
  }
};

} // namespace detail

template <typename To, typename From>
typename std::enable_if_t<!std::is_same_v<To, From>, To>
lexical_cast(const From &from) {
  To to;
  if (detail::Converter::convert(from, to)) {
    return to;
  }
  throw std::invalid_argument("argument is invalid");
}

template <typename To, typename From>
typename std::enable_if_t<std::is_same_v<To, From>, To>
lexical_cast(const From &from) {
  return from;
}
} // namespace Anycubic::utility
