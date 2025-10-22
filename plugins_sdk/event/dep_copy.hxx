#pragma once

#include <boost/pfr.hpp>

#include <utility/json/arrry_wraper.hxx>
#include <utility/utils/range.hxx>

#include <memory>

namespace Anycubic::Plugins::SDK {

template <template <typename...> class U, typename T>
struct is_template_instant_of : std::false_type {};

template <template <typename...> class U, typename... args>
struct is_template_instant_of<U, U<args...>> : std::true_type {};

template <typename T>
inline constexpr bool is_array_wrapper_v =
    is_template_instant_of<Anycubic::utility::json::ArrayWrapper, T>::value;

template <typename T, int N>
inline constexpr static size_t array_size(T (&)[N]) {
  return N;
}

template <typename _Ty> static void dep_copy(_Ty &&dst) {
  using type_v = std::decay_t<_Ty>;
  if constexpr (std::is_same_v<type_v, char *>) {
    if (dst != nullptr && strlen(dst) > 0) {
      dst = strdup(dst);
    } else {
      dst = nullptr;
    }
  } else if constexpr (is_array_wrapper_v<type_v>) {
    using range = utility::utils::range;
    auto old_data = dst.data_;
    dst.data_ = new typename type_v::value_type[dst.size()];
    for (auto i : range(dst.size())) {
      if constexpr (std::is_array_v<typename type_v::value_type>) {
        for (auto idx : range(array_size(old_data[i]))) {
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
template <typename _Ty> static void dep_free(_Ty &&dst) {
  using type_v = std::decay_t<_Ty>;
  if constexpr (std::is_same_v<type_v, char *>) {
    if (dst != nullptr) {
      free(dst);
      dst = nullptr;
    }
  } else if constexpr (is_array_wrapper_v<type_v>) {
    using range = utility::utils::range;
    for (auto i : range(dst.size())) {
      if constexpr (std::is_array_v<typename type_v::value_type>) {
        for (auto idx : range(array_size(dst.data_[i]))) {
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

template <typename type_v>
static type_v *copy_array(type_v *src, size_t src_size) {
  using range = utility::utils::range;
  auto dst = new type_v[src_size];
  for (auto i : range(src_size)) {
    dst[i] = src[i];  // 对数组元素执行拷贝
    DepCopy_(dst[i]); // 对数组元素递归执行拷贝
  }
  return make_shared(dst, src_size);
}
template <typename type_v> static type_v *copy_object(type_v *src) {
  auto dst = new type_v;
  *dst = *src;    // 对对象元素执行拷贝
  DepCopy_(*dst); // 对对象元素递归执行拷贝
  return dst;
}

template <typename type_v>
static void free_array(type_v *dst, size_t src_size) {
  using range = utility::utils::range;
  for (auto i : range(src_size)) {
    dep_free(dst[i]); // 对数组元素递归执行释放
  }
  delete[] dst;
}

template <typename type_v> class deleter_array {
public:
  static void release(void *data, void *pthis) {
    auto ptr = reinterpret_cast<type_v *>(data);
    auto this_ = reinterpret_cast<deleter_array<type_v> *>(pthis);
    // 深度释放
    free_array(ptr, this_->size_);
    delete this;
  }

public:
  deleter_array(int size) : size_(size) {}

private:
  int size_;
};

template <typename type_v> class deleter_object {
public:
  static void release(void *data, void *) {
    auto ptr = reinterpret_cast<type_v *>(data);
    // 深度释放
    dep_free(*ptr);
    delete ptr;
  }

} // namespace Anycubic::Plugins::SDK