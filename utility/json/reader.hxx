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
#include "arrry_wraper.hxx"

#include <iguana/reflection.hpp>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <type_traits>

/**************************************************************************
 * iguana 本身对顺序有要求的，但实际应用中，json
 * 字符字段的顺序是不确定的，这里使用 iguana 一部分机制来重实现的c++14的
 * 反射实现。主要给AnycubicSlicerNext 项目使用。
 */
namespace Anycubic::utility::json {

// ArrayWrapper

using namespace iguana;
using reader = nlohmann::json;

template <typename T>
inline constexpr bool is_array_wrapper_v =
    is_template_instant_of<ArrayWrapper, T>::value;

template <typename T>
inline std::enable_if_t<is_reflection_v<T>, bool> free_memory(T &&t);

namespace detail {

template <size_t Idx, typename T, typename F>
constexpr void call_one(T &&t, F &&f) {
  using M = decltype(iguana_reflect_members(std::forward<T>(t)));
  static_assert(Idx < M::value());
  std::forward<F>(f)(t.*std::get<Idx>(M::apply_impl()),
                     std::integral_constant<size_t, Idx>{});
}
template <typename T, typename F, std::size_t... Idx>
constexpr void for_each(T &&t, F &&f, std::index_sequence<Idx...>) {
  (call_one<Idx>(std::forward<T>(t), std::forward<F>(f)), ...);
}

template <typename T, typename F>
constexpr std::enable_if_t<is_reflection<T>::value> for_each(T &&t, F &&f) {
  using M = decltype(iguana_reflect_members(std::forward<T>(t)));
  for_each(std::forward<T>(t), std::forward<F>(f),
           std::make_index_sequence<M::value()>{});
}
template <typename T>
struct is_cstring
    : std::integral_constant<bool,
                             std::is_same_v<std::remove_pointer_t<T>, char>> {};

template <typename T>
inline constexpr std::enable_if_t<is_reflection_v<T>> do_read(reader &rd,
                                                              T &&t);
// read json to value
template <typename T>
inline std::enable_if_t<is_signed_intergral_like<T>::value>
read_json(reader &rd, T &val) {
  assert(rd.is_number_integer());
  val = rd;
}

template <typename T>
inline std::enable_if_t<is_unsigned_intergral_like<T>::value>
read_json(reader &rd, T &val) {
  assert(rd.is_number_unsigned());
  val = rd;
}

template <typename T>
inline std::enable_if_t<std::is_enum<T>::value> read_json(reader &rd, T &val) {
  using RAW_TYPE = std::underlying_type_t<T>;
  read_json(rd, (RAW_TYPE &)val);
}

template <typename T>
inline std::enable_if_t<std::is_floating_point<T>::value> read_json(reader &rd,
                                                                    T &val) {
  assert(rd.is_number()); ///< 只要是个数字就被允许
  val = rd;
}

inline void read_json(reader &rd, bool &val) {
  assert(rd.is_boolean());
  val = rd;
}
inline void read_json(reader &rd, std::string &val) {
  assert(rd.is_string());
  val = rd;
}

inline void read_json(reader &rd, char *&val) {
  assert(rd.is_string());
  std::string v = rd;
#ifdef _WIN32
#define strdup _strdup
#endif
  val = strdup(v.c_str());
#ifdef _WIN32
#undef strdup
#endif
}

template <typename T>
inline std::enable_if_t<is_reflection_v<T>> read_json(reader &rd, T &val) {
  do_read(rd, val);
}
template <typename T, size_t N> inline void read_json(reader &rd, T (&val)[N]) {
  assert(rd.is_array());
  assert(rd.size() <= N);
  int index = 0;
  for (auto &r : rd) {
    read_json(r, val[index]);
    index++;
  }
}

template <typename T, size_t N>
inline void read_json(reader &rd, std::array<T, N> &val) {
  assert(rd.is_array());
  assert(rd.size() <= N);
  int index = 0;
  for (auto &r : rd) {
    assert(index < N);
    read_json(r, val[index]);
    index++;
  }
}

template <typename T>
std::enable_if_t<is_emplace_back_able<T>::value> emplace_back(T &val) {
  val.emplace_back();
}

template <typename T>
std::enable_if_t<is_template_instant_of<std::queue, T>::value>
emplace_back(T &val) {
  val.emplace();
}

template <typename T>
inline std::enable_if_t<is_sequence_container<T>::value> read_json(reader &rd,
                                                                   T &val) {
  assert(rd.is_array());
  val.reserve(rd.size());
  for (auto r : rd) {
    emplace_back(val);
    read_json(r, val.back());
  };
}

template <typename T>
inline std::enable_if_t<is_array_wrapper_v<T>> read_json(reader &rd, T &val) {
  typedef typename T::value_type value_type;
  assert(rd.is_array());
  val.size_ = rd.size();
  if (val.size_ == 0) {
    return; // 空数组
  }
  val.data_ = new value_type[val.size_];
  int index = 0;
  for (auto r : rd) {
    read_json(r, val.data_[index]);
    index++;
  }
}

template <typename T>
inline std::enable_if_t<is_associat_container<T>::value> read_json(reader &rd,
                                                                   T &val) {
  assert(rd.is_object());
  static_assert(
      is_template_instant_of<std::basic_string, typename T::key_type>::value,
      "key must be std::basic_string");
  for (auto &item : rd.items()) {
    typename T::key_type key = item.key();
    read_json(item.value(), val[key]);
  }
}

template <typename T>
inline constexpr std::enable_if_t<is_reflection_v<T>> do_read(reader &rd,
                                                              T &&t) {

  using M = decltype(iguana_reflect_members(std::forward<T>(t)));
  detail::for_each(std::forward<T>(t), [&rd](auto &v, auto i) {
    constexpr auto Idx = decltype(i)::value;
    constexpr auto Count = M::value();
    static_assert(Idx < Count);

    using type_v = std::decay_t<decltype(v)>;

#if defined(NDEBUG)
    auto name = M::arr()[Idx];
#else
    auto names = M::arr();
    auto name = names[Idx];
#endif
    if (auto itr = rd.find(name); itr != rd.end() && !itr->is_null()) {
      if constexpr (!is_reflection<type_v>::value) {
        read_json(*itr, v);
      } else {
        do_read(*itr, v); // 递归
      }
    }
  });
}
template <typename U, typename T> inline void assign(reader &rd, T &t) {
  if constexpr (!is_reflection<U>::value) {
    read_json(rd, t);
  } else {
    do_read(rd, t);
  }
}

template <typename T> inline void free_memory_impl(T &val) {}

template <typename T>
inline std::enable_if_t<is_reflection_v<T>> free_memory_impl(T &&val) {
  free_memory(std::forward<T>(val));
}

template <typename itr_type>
inline void free_memory_impl(itr_type begin, itr_type end) {
  using value_type = std::decay_t<decltype(*begin)>;
  static_assert(!std::is_const_v<value_type>, "value_type must not be const");
  if constexpr (std::is_same_v<value_type, char *>) {
    std::for_each(begin, end, ::free);
  } else {
    std::for_each(begin, end, [](auto &&v) { free_memory_impl(v); });
  }
}
template <typename T, size_t N> inline void free_memory_impl(T (&val)[N]) {
  free_memory_impl(std::begin(val), std::end(val));
}

template <typename T, size_t N>
inline void free_memory_impl(std::array<T, N> &val) {
  free_memory_impl(std::begin(val), std::end(val));
}

template <typename T>
inline std::enable_if_t<is_sequence_container<T>::value>
free_memory_impl(T &&val) {
  free_memory_impl(std::begin(val), std::end(val));
  val.clear();
}

template <typename T>
inline std::enable_if_t<is_associat_container<T>::value>
free_memory_impl(T &&val) {
  static_assert(
      is_template_instant_of<std::basic_string, typename T::key_type>::value,
      "key must is std::basic_string");
  for (auto &item : val)
    free_memory_impl(item.second);
  val.clear();
}

template <typename T>
inline std::enable_if_t<is_array_wrapper_v<T>> free_memory_impl(T &&val) {
  if (val.data_ && val.size_) {
    free_memory_impl(val.data_, val.data_ + val.size_);
    delete[] val.data_;
    val.data_ = nullptr;
    val.size_ = 0;
  }
}

} // namespace detail
static bool parse_json(reader &rd, const char *buf, size_t len) {
  if (len == -1) {
    len = strlen(buf);
  }
  try {
    rd = reader::parse(buf, buf + len);
    return true;
  } catch (const nlohmann::json::exception &) {
    return false;
  }
}

/**
 * @brief 从JSON对象加载元组
 *
 * @tparam T 元组类型
 * @param t 元组对象
 * @param rd JSON读取器对象
 * @return bool 是否加载成功
 */
template <typename T>
inline std::enable_if_t<is_tuple<std::decay_t<T>>::value, bool>
load_from_json(T &&t, reader &rd) {
  for_each(std::forward<T>(t),
           [&rd](auto &v, auto i) { detail::assign<decltype(v)>(rd, v); });
  return true;
}

/**
 * @brief 从JSON对象加载序列容器
 *
 * @tparam T 序列容器类型
 * @param v 序列容器对象
 * @param rd JSON读取器对象
 * @return bool 是否加载成功
 */
template <typename T>
inline std::enable_if_t<is_associat_container<std::decay_t<T>>::value, bool>
load_from_json(T &v, reader &rd) {
  assert(rd.is_object());
  v.clear();
  using U = typename std::decay_t<T>::value_type;
  using kt = typename U::first_type;
  using vt = typename U::second_type;
  for (auto &n : rd.items()) {
    auto &[key, value] = n;
    detail::assign<vt>(value, v[key]);
  }
  return true;
}

/**
 * @brief 从JSON对象加载关联容器
 *
 * @tparam T 关联容器类型
 * @param v 关联容器对象
 * @param rd JSON读取器对象
 * @return bool 是否加载成功
 */
template <typename T>
inline std::enable_if_t<is_sequence_container<std::decay_t<T>>::value, bool>
load_from_json(T &v, reader &rd) {
  assert(rd.is_array());
  v.clear();
  using U = typename std::decay_t<T>::value_type;
  for (auto &n : rd) {
    detail::emplace_back(v);
    detail::assign<U>(n, v.back());
  }

  return true;
}

/**
 * @brief 从JSON对象加载反射对象
 *
 * @tparam T 反射对象类型
 * @param t 反射对象对象
 * @param rd JSON读取器对象
 * @return bool 是否加载成功
 */
template <typename T>
inline std::enable_if_t<is_reflection_v<T>, bool> load_from_json(T &&t,
                                                                 reader &rd) {
  try {
    detail::do_read(rd, t);
  } catch (std::exception &) {
    return false;
  }
  return true;
}

/**
 * @brief 从JSON字符串加载反射对象
 *
 * @tparam T 反射对象类型
 * @param t 反射对象对象
 * @param buf JSON字符串指针
 * @param len JSON字符串大小
 * @return bool 是否加载成功
 */
template <typename T>
inline bool load_from_json(T &&t, const char *buf, size_t len = -1) {
  reader rd;
  if (!parse_json(rd, buf, len)) {
    return false;
  }
  return load_from_json(std::forward<T>(t), rd);
}

/**
 * @brief 释放反射对象内存
 *
 * @tparam T 反射对象类型
 * @param t 反射对象对象
 * @return bool 是否释放成功
 */
template <typename T>
inline std::enable_if_t<is_reflection_v<T>, bool> free_memory(T &&t) {
  try {
    detail::for_each(std::forward<T>(t), [&t](auto &v, auto i) {
      using value_type = std::decay_t<decltype(v)>;
      if constexpr (std::is_same_v<value_type, char *>) {
        free(v);
        v = nullptr;
      } else {
        detail::free_memory_impl(std::forward<decltype(v)>(v));
      }
    });
  } catch (std::exception &) {
    return false;
  }
  return true;
}

} // namespace Anycubic::utility::json