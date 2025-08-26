#pragma once
#include "plugins_stream.hxx"

#include <wx/string.h>

namespace Anycubic::Plugins {

template <typename... Args>
void unpack_args_read(struct IStream *stream, Args &...args) {
  (stream->Read(args), ...);
}

template <typename... Args>
void unpack_args(std::tuple<Args...> &args, struct IStream *stream) {
  auto &&packed_args = std::tuple_cat(std::make_tuple(stream), args);
  std::apply(
      [](auto &&stream, auto &&...args) {
        return unpack_args_read(std::forward<decltype(stream)>(stream),
                                std::forward<decltype(args)>(args)...);
      },
      packed_args);
}

template <typename... Args>
void pack_result(struct OStream *stream, Args &&...args) {
  (stream->Write(args), ...);
}
template <typename... Args> size_t get_bytes(Args... args) {
  return (sizeof(Args) + ...);
}
} // namespace Anycubic::Plugins
