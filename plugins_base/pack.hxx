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

/**
 * @file pack.hxx
 * @brief 这里代码是给funcation.hxx中使用，其他地方使用要慎重
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "plugins_stream.hxx"
#include "tuple_foreach.hxx"

#include <utility/json/arrry_wraper.hxx>
#include <utility/json/reader.hxx>

#include <iguana/detail/string_stream.hpp>
#include <iguana/json.hpp>

#include <wx/string.h>

#include <string.h>

namespace iguana::json {
template <typename Stream, typename T> auto to_json(Stream &ss, const T &o) {
  return render_json_value(ss, o);
}
} // namespace iguana::json

namespace Anycubic::Plugins {

template <typename _Ty>
constexpr bool is_json_type_v =
    iguana::is_sequence_container<_Ty>::value ||
    iguana::is_associat_container<_Ty>::value || iguana::is_reflection_v<_Ty>;
namespace {
template <typename _Ty> size_t get_type_size(_Ty &&v) {
  using value_t = std::decay_t<_Ty>;
  if constexpr (is_json_type_v<value_t>) {
    iguana::string_stream ss;
    try {
      iguana::json::to_json(ss, v);
      return get_type_size(ss.str()); // 递归
    } catch (...) {
      return 0;
    }
  } else if constexpr (std::is_same_v<value_t, wxString>) {
    auto tmp = v.utf8_string();
    return get_type_size(tmp);
  } else if constexpr (std::is_same_v<value_t, std::string>) {
    return v.length() + sizeof(uint16_t);
  } else if constexpr (is_c_string_v<value_t>) {
    if (v == nullptr) {
      return sizeof(uint16_t);
    }
    return strlen(v) + sizeof(uint16_t);
  } else if constexpr (std::is_arithmetic_v<value_t> ||
                       std::is_pointer_v<value_t> || std::is_pod_v<value_t>) {
    return sizeof(value_t);
  } else {
    static_assert(sizeof(value_t) == 0,
                  "unsupported type: type must be arithmetic, pointer, POD, "
                  "wxString, std::string, C-string, or JSON-compatible");
  }
}
template <typename _Ty> inline bool read_value(struct IStream *stream, _Ty &v) {
  if constexpr (is_json_type_v<std::decay_t<_Ty>>) {
    std::string json;
    if (!stream->Read(json)) {
      return false;
    }
    return utility::json::load_from_json(v, json.data(), json.size());
  } else {
    return stream->Read(v);
  }
}

template <typename _Ty>
inline bool write_value(struct OStream *stream, const _Ty &v) {
  if constexpr (is_json_type_v<std::decay_t<_Ty>>) {
    iguana::string_stream ss;
    try {
      iguana::json::to_json(ss, v);
    } catch (...) {
      return false;
    }
    return stream->Write(ss.str());
  } else {
    return stream->Write(v);
  }
}

} // namespace

template <typename... Args> size_t get_bytes(Args &&...args) {
  return (get_type_size(args) + ...);
}

template <typename... Args>
void unpack_args_read(struct IStream *stream, Args &...args) {
  auto result = (read_value(stream, args) && ...);
  assert(result);
}

template <typename... Args>
bool unpack_args(std::tuple<Args...> &args, struct IStream *stream) {
  auto result = std::apply(
      [stream](Args &...tuple_args) {
        return (read_value(stream, tuple_args) && ...);
      },
      args);
  assert(result);
  return result;
}

// 保持原有的pack_result和get_bytes函数
template <typename... Args>
void pack_result(struct OStream *stream, Args &&...args) {
  auto bytes = get_bytes(std::forward<Args>(args)...);
  stream->Resize(bytes);
  auto result = (write_value(stream, std::forward<Args>(args)) && ...);
  assert(result);
}

template <typename Tuple> inline void pack_free(Tuple &&tuple) {
  tuple_for_each(tuple, [](auto &&elem) {
    using value_t = std::decay_t<decltype(elem)>;
    if constexpr (is_c_string_v<value_t>) {
      free(const_cast<char *>(elem));
    }
  });
}

} // namespace Anycubic::Plugins