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
#include "utility/utils/lexical_cast.hxx"

#include <iguana/json.hpp>

#include <algorithm>
#include <string>
#include <type_traits>

namespace iguana::json {
inline void read_json(reader_t &rd, char *&val, bool unorder = false) {
  auto &tok = rd.peek();
  if (tok.type == token::t_string) {
    // tok.str.str, tok.str.len
    val = static_cast<char *>(malloc(tok.str.len + 1));
    strncpy(val, tok.str.str, tok.str.len);
    val[tok.str.len] = '\0';
  } else {
    rd.error("not a valid string.");
  }
  rd.next();
}

template <typename T>
inline void read_json(reader_t &rd,
                      Anycubic::utility::json::ArrayWrapper<T> &val) {
  using value_type = T;
  std::vector<value_type> vec;
  read_array(rd, vec);
  if (vec.empty()) {
    return;
  }
  val.data_ = new value_type[vec.size()];
  val.size_ = vec.size();
  std::copy(std::begin(vec), std::end(vec), val.begin());
}

template <typename T>
inline std::enable_if_t<is_template_instant_of<std::pair, T>::value>
read_json(reader_t &rd, T &t, bool unorder = false) {
  auto &tk = rd.peek();
  if constexpr (is_stdstring<std::decay_t<typename T::first_type>>::value) {
    t.first.assign(tk.str.str, tk.str.len);
  } else {
    t.first = Anycubic::utility::lexical_cast<typename T::first_type>(
        std::string(tk.str.str, tk.str.len));
  }
  rd.next();
  rd.next();
  read_json(rd, t.second, unorder);
}

template <typename U, typename T> inline void assign0(reader_t &rd, T &t) {
  if constexpr (!is_reflection<U>::value) {
    read_json(rd, t, true);
  } else {
    do_read0(rd, t);
    rd.next();
  }

  if (g_has_error)
    return;

  rd.next();
}
template <typename T>
inline std::enable_if_t<is_tuple<std::decay_t<T>>::value, bool>
from_json0(T &&t, const char *buf, size_t len = -1) {
  g_has_error = false;
  reader_t rd(buf, len);
  rd.next();
  for_each(std::forward<T>(t),
           [&rd](auto &v, auto i) { assign0<decltype(v)>(rd, v); });
  return !g_has_error;
}

template <typename T>
inline std::enable_if_t<is_sequence_container<std::decay_t<T>>::value, bool>
from_json0(T &&v, const char *buf, size_t len = -1) {
  v.clear();
  using U = typename std::decay_t<T>::value_type;
  U t{};
  reader_t rd(buf, len);
  rd.next();
  while (rd.peek().type != token::t_end) {
    if (g_has_error)
      return false;

    assign0<U>(rd, t);
    v.push_back(std::move(t));
  }

  return true;
}

template <typename T>
inline std::enable_if_t<is_associat_container<std::decay_t<T>>::value, bool>
from_json0(T &&v, const char *buf, size_t len = -1) {
  v.clear();
  using U = typename std::decay_t<T>::value_type;
  std::pair<std::decay_t<typename U::first_type>,
            std::decay_t<typename U::second_type>>
      t{};
  reader_t rd(buf, len);
  rd.next();
  while (rd.peek().type != token::t_end) {
    if (g_has_error)
      return false;

    assign0<decltype(t)>(rd, t);
    v.emplace(std::move(t.first), std::move(t.second));
  }

  return true;
}
template <typename Stream, typename T>
std::enable_if_t<is_associat_container<T>::value> to_json(Stream &ss,
                                                          const T &o) {
  ss.put('{');
  join(ss, o.cbegin(), o.cend(), ',', [&ss](const auto &jsv) {
    if constexpr (is_stdstring<std::decay_t<decltype(jsv.first)>>::value) {
      render_key(ss, jsv.first);
    } else {
      render_key(ss, std::to_string(jsv.first));
    }

    ss.put(':');
    render_json_value(ss, jsv.second);
  });
  ss.put('}');
}
} // namespace iguana::json
namespace Anycubic::utility::json {

template <typename T>
inline bool load_from_json(T &&t, const char *buf, size_t len = -1) {
  return iguana::json::from_json0(std::forward<T>(t), buf, len);
}

template <typename T>
constexpr bool is_array_wraper_v =
    iguana::is_template_instant_of<ArrayWrapper, T>::value;

template <typename T>
inline std::enable_if_t<iguana::is_associat_container<T>::value, bool>
free_memory_ass(T &&t) {
  for (auto &[_, val] : t) {
    free_memory(val);
  }
  return true;
}
template <typename T>
inline std::enable_if_t<iguana::is_sequence_container<T>::value, bool>
free_memory_seq(T &&t) {
  for (auto &val : t) {
    free_memory(val);
  }
  return true;
}
template <typename T>
inline std::enable_if_t<is_array_wraper_v<T>, bool> free_memory_wrap(T &&t) {
  for (auto &val : t) {
    free_memory(val);
  }
  delete[] t.data_;
  t.data_ = nullptr;
  t.size_ = 0;
  return true;
}

/**
 * @brief 释放反射对象内存
 *
 * @tparam T 反射对象类型
 * @param t 反射对象对象
 * @return bool 是否释放成功
 */
template <typename T>
inline std::enable_if_t<iguana::is_reflection<T>::value, bool>
free_memory_ref(T &&t) {
  try {
    iguana::for_each(std::forward<T>(t), [](auto &v, auto i) {
      using value_type = std::decay_t<decltype(v)>;
      free_memory(std::forward<decltype(v)>(v));
    });
  } catch (std::exception &) {
    return false;
  }
  return true;
}

static inline bool free_memory(char *&t) {
  free(t);
  t = nullptr;
  return true;
}

template <typename T> inline bool free_memory(T &&t) {
  if constexpr (iguana::is_reflection<T>::value) {
    return free_memory_ref(std::forward<T>(t));
  } else if constexpr (iguana::is_sequence_container<T>::value) {
    return free_memory_seq(std::forward<T>(t));
  } else if constexpr (iguana::is_associat_container<T>::value) {
    return free_memory_ass(std::forward<T>(t));
  } else if constexpr (is_array_wraper_v<T>) {
    return free_memory_wrap(std::forward<T>(t));
  } else {
    return true;
  }
}

} // namespace Anycubic::utility::json