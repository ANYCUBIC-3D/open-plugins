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
    if constexpr (std::is_same_v<std::decay_t<To>, std::string>) {
      to = from;
      return true;
    } else {
      return convert(from.c_str(), to);
    }
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

/**
 * @brief 类型转换
 *
 * @tparam To 目标类型
 * @tparam From 源类型
 * @param from 源对象
 * @return To 目标对象
 */
template <typename To, typename From>
typename std::enable_if_t<!std::is_same_v<std::decay_t<To>, std::decay_t<From>>,
                          To>
lexical_cast(const From &from) {
  To to;
  if (detail::Converter::convert(from, to)) {
    return to;
  }
  throw std::invalid_argument("argument is invalid");
}

/**
 * @brief 类型转换（相同类型）
 *
 * @tparam To 目标类型
 * @tparam From 源类型
 * @param from 源对象
 * @return To 目标对象
 */
template <typename To, typename From>
typename std::enable_if_t<std::is_same_v<std::decay_t<To>, std::decay_t<From>>,
                          To>
lexical_cast(const From &from) {
  return from;
}
} // namespace Anycubic::utility
