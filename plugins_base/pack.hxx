#pragma once
#include "plugins_stream.hxx"

#include <wx/string.h>

namespace Anycubic::Plugins {

template <typename _Ty> size_t get_type_size(const _Ty &v) {
  using value_t = std::decay_t<_Ty>;

  if constexpr (std::is_arithmetic_v<value_t> || std::is_pointer_v<value_t> ||
                std::is_pod_v<value_t>) {
    return sizeof(value_t);
  } else if constexpr (std::is_same_v<value_t, wxString> ||
                       std::is_same_v<value_t, const wxString &>) {
    return v.length() + sizeof(uint16_t);
  } else if constexpr (std::is_same_v<value_t, std::string> ||
                       std::is_same_v<value_t, const std::string &>) {
    return v.size() + sizeof(uint16_t);
  }
}

template <typename... Args> size_t get_bytes(Args... args) {
  return (get_type_size(args) + ...);
}

template <typename... Args>
void unpack_args_read(struct IStream *stream, Args &...args) {
  (stream->Read(args), ...);
}

template <typename... Args>
bool unpack_args(std::tuple<Args...> &args, struct IStream *stream) {
  auto result = std::apply(
      [stream](Args &...tuple_args) {
        return (stream->Read(tuple_args) && ...);
      },
      args);
  return result;
}

// 保持原有的pack_result和get_bytes函数
template <typename... Args>
void pack_result(struct OStream *stream, Args &&...args) {
  auto bytes = get_bytes(std::forward<Args>(args)...);
  stream->Resize(bytes);
  (stream->Write(args), ...);
}

} // namespace Anycubic::Plugins
