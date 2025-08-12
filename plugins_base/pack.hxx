#pragma once
#include "plugins_stream.hxx"

#include <boost/pfr.hpp>

#include <type_traits>

#include <wx/string.h>

namespace Anycubic::Plugins {
template <typename Ty> int decode(Ty &ret, struct IStream *data) {

  if constexpr (std::is_arithmetic_v<Ty>) {
    data->Read(&ret);
  } else if constexpr (std::is_pointer_v<Ty>) {
    decode(*ret, data);
  } else if constexpr (std::is_same_v<Ty, std::string>) {
    uint16_t bytes;
    auto pos = data->Tellg();
    data->Read(&bytes);
    data->Seekg(pos);
    ret.resize(bytes);
    data->Read(ret.data(), bytes);
  } else if constexpr (std::is_same_v<Ty, wxString>) {
    std::string val;
    decode(val, data);
    ret = wxString::FromUTF8(val);
  } else {
    boost::pfr::for_each_field(ret,
                               [data](auto &field) { decode(field, data); });
  }
  return 0;
}

template <int32_t I = 0, typename... Args>
void unpack_args(std::tuple<Args...> &args, struct IStream *data) {
  if constexpr (I < sizeof...(Args)) {
    decode(std::get<I>(args), data);
    unpack_args<I + 1>(args, data);
  }
}

template <typename Ty> void pack_result(struct OStream *result, Ty &&ret) {
  if constexpr (std::is_arithmetic_v<Ty>) {
    result->Write(ret);
  } else if constexpr (std::is_pointer_v<Ty>) {
    result->Write(ret);
  } else if constexpr (std::is_same_v<Ty, std::string>) {
    result->Write(ret.data(), ret.size());
  } else {
    boost::pfr::for_each_field(
        ret, [result](auto &field) { pack_result(result, field); });
  }
}

template <int32_t I = 0, typename... Args>
void pack_result(struct OStream *result, const std::tuple<Args...> &args) {
  if constexpr (I < sizeof...(Args)) {
    pack_result(result, std::get<I>(args));
    pack_result<I + 1>(result, args);
  }
}
template <typename T, typename... Args> size_t get_bytes(T &t, Args... args) {
  size_t bytes = 0;
  if constexpr (std::is_arithmetic_v<T>) {
    bytes = sizeof(T);
  } else if constexpr (std::is_same_v<T, std::string>) {
    bytes = sizeof(uint16_t) + t.size();
  } else {
    boost::pfr::for_each_field(
        t, [&bytes](auto &field) { bytes += get_bytes(field); });
  }

  if constexpr (sizeof...(args) > 0)
    return bytes + get_bytes(args...);
  else
    return bytes;
}
} // namespace Anycubic::Plugins
