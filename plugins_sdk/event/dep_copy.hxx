#pragma once

#include <boost/pfr.hpp>

#include <utility/json/arrry_wraper.hxx>

namespace Anycubic::Plugins::SDK {

template <template <typename...> class U, typename T>
struct is_template_instant_of : std::false_type {};

template <template <typename...> class U, typename... args>
struct is_template_instant_of<U, U<args...>> : std::true_type {};

template <typename T>
inline constexpr bool is_array_wrapper_v =
    is_template_instant_of<Anycubic::utility::json::ArrayWrapper, T>::value;

template <typename T, int N> inline constexpr size_t array_size(T (&)[N]) {
  return N;
}

template <typename _Ty> void dep_copy(_Ty &&dst) {
  using type_v = std::decay_t<_Ty>;
  if constexpr (std::is_same_v<type_v, char *>) {
    if (dst != nullptr && strlen(dst) > 0) {
      dst = strdup(src);
    } else {
      dst = nullptr;
    }
  } else if constexpr (is_array_wrapper_v<type_v>) {
    auto old_data = dst.data_;
    dst.data_ = new typename type_v::value_type[dst.size()];
    for (size_t i = 0; i < dst.size(); ++i) {
      if constexpr (std::is_array_v<typename type_v::value_type>) {
        for (int idx = 0; idx < array_size(old_data[i]); idx++) {
          dst.data_[i][idx] = old_data[i][idx]; // 对数组元素执行拷贝
          DepCopy_(dst.data_[i][idx]);          // 对数组元素递归执行拷贝
        }
      } else {
        dst.data_[i] = old_data[i]; // 对数组元素执行拷贝
        DepCopy_(dst.data_[i]);     // 对数组元素递归执行拷贝
      }
    }
  } else if constexpr (std::is_aggregate_v<type_v>) {
    boost::pfr::for_each_field(dst, [](auto &field) { dep_copy(field); });
  } else {
  }
}
template <typename _Ty> void dep_free(_Ty &&dst) {
  using type_v = std::decay_t<_Ty>;
  if constexpr (std::is_same_v<type_v, char *>) {
    if (dst != nullptr) {
      free(dst);
      dst = nullptr;
    }
  } else if constexpr (is_array_wrapper_v<type_v>) {
    for (size_t i = 0; i < dst.size(); ++i) {
      if constexpr (std::is_array_v<typename type_v::value_type>) {
        for (int idx = 0; idx < array_size(dst.data_[i]); idx++) {
          dep_free(dst.data_[i][idx]); // 对数组元素递归执行释放
        }
      } else {
        dep_free(dst.data_[i]); // 对数组元素递归执行释放
      }
    }
    delete[] dst.data_;
  } else if constexpr (std::is_aggregate_v<type_v>) {
    boost::pfr::for_each_field(dst, [](auto &field) { dep_free(field); });
  }
}
} // namespace Anycubic::Plugins::SDK