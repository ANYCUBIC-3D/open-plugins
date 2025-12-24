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

#include <wx/string.h>

#include <string.h>

namespace Anycubic::Plugins {

template <typename _Ty> size_t get_type_size(_Ty &v) {
  using value_t = std::decay_t<_Ty>;
  if constexpr (std::is_same_v<value_t, wxString>) {
    auto tmp = v.utf8_string();
    return get_type_size(tmp);
  } else if constexpr (std::is_same_v<value_t, std::string>) {
    return v.length() + sizeof(uint16_t);
  } else if constexpr (is_c_string_v<value_t>) {
    return strlen(v) + sizeof(uint16_t);
  } else if constexpr (std::is_arithmetic_v<value_t> ||
                       std::is_pointer_v<value_t> || std::is_pod_v<value_t>) {
    return sizeof(value_t);
  }
}

template <typename... Args> size_t get_bytes(Args &&...args) {
  return (get_type_size(args) + ...);
}

template <typename... Args>
void unpack_args_read(struct IStream *stream, Args &...args) {
  auto result = (stream->Read(args) && ...);
  assert(result);
}

template <typename... Args>
bool unpack_args(std::tuple<Args...> &args, struct IStream *stream) {
  auto result = std::apply(
      [stream](Args &...tuple_args) {
        return (stream->Read(tuple_args) && ...);
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
  auto result = (stream->Write(args) && ...);
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
