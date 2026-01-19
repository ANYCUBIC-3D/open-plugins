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

#pragma once

#include <string>

#include <stdint.h>

#include <iguana/detail/traits.hpp>
#include <iguana/json.hpp>

template <typename T>
inline constexpr bool is_squence_container_v =
    iguana::is_sequence_container<T>::value;
template <typename T>
inline constexpr bool is_associative_container_v =
    iguana::is_associat_container<T>::value;

template <typename T>
inline constexpr bool is_c_string_v =
    std::is_same_v<const char *, T> || std::is_same_v<char *, T>;

template <typename T>
inline constexpr bool is_string_v =
    is_c_string_v<T> || std::is_same_v<std::decay_t<T>, std::string>;

template <typename T> auto ac_to_string_if_need(T &&t) {
  using type_v = std::decay_t<T>;
  if constexpr (is_string_v<type_v> || std::is_arithmetic_v<type_v>) {
    return std::forward<T>(t);
  } else if constexpr (iguana::is_reflection_v<type_v> ||
                       is_squence_container_v<type_v> ||
                       is_associative_container_v<type_v>) {
    iguana::string_stream ss;
    iguana::json::to_json(ss, t);
    return ss.str();
  } else {
    return;
  }
}

template <typename T> auto ac_to_string(T &&t) {
  using type_v = std::decay_t<T>;
  if constexpr (std::is_arithmetic_v<type_v>) {
    return std::to_string(t);
  } else if constexpr (is_string_v<type_v>) {
    return std::forward<T>(t);
  } else {
    return ac_to_string_if_need(std::forward<T>(t));
  }
}