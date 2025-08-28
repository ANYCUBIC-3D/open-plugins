#pragma once
#include "plugins_base_export.hxx"

#include <assert.h>
#include <stddef.h>

#include <numeric>
#include <string>
#include <type_traits>

namespace Anycubic::Plugins {

template <typename T>
constexpr bool is_trivial_v =
    std::is_arithmetic_v<T> ||
    (std::is_class_v<T> && std::is_standard_layout_v<T> &&
     std::is_trivial_v<T>);
template <typename T>
constexpr bool is_std_string_v = std::is_same_v<std::decay_t<T>, std::string>;

template <typename T>
constexpr bool is_pointer_v =
    std::is_pointer_v<T> || std::is_same_v<T, const char *>;

struct PLGINS_EXPORT IStream {
  IStream(const void *data, size_t size);
  IStream(size_t size);
  ~IStream(void);
  size_t Read(void *data, size_t size);

  size_t Size() const;
  size_t Tellg(void) const;
  size_t Seekg(size_t pos);
  const void *Data(void) const;

  template <typename T> std::enable_if_t<is_trivial_v<T>, bool> Read(T &data) {
    return sizeof(T) == Read(&data, sizeof(T));
  }

  bool Read(std::string &data) {
    uint16_t length = 0;
    if (!Read(length)) {
      return false;
    }
    data.resize(length);
    return Read(data.data(), data.size()) == length;
  }

  template <typename T> std::enable_if_t<is_pointer_v<T>, bool> Read(T &data) {
    intptr_t ptr = 0;
    if (!Read(ptr)) {
      return false;
    }
    data = reinterpret_cast<T>(ptr);
    return true;
  }

private:
  const void *data_;
  size_t size_;
  size_t pos_;
  bool is_owner_;
};

struct PLGINS_EXPORT OStream {
  OStream(void *buffer, size_t size);
  OStream(size_t size = 0);
  ~OStream(void);
  size_t Write(const void *data, size_t size);

  size_t Size() const;
  void *Data(void);
  size_t Tellp(void) const;
  size_t Seekp(size_t pos);
  size_t Resize(size_t size);

  template <typename T>
  std::enable_if_t<is_trivial_v<T>, bool> Write(const T &data) {
    return sizeof(T) == Write(&data, sizeof(T));
  }

  bool Write(const std::string &data) {
    assert(data.size() <= std::numeric_limits<uint16_t>::max());
    uint16_t length = static_cast<uint16_t>(data.size());
    return Write(length) && Write(data.data(), length);
  }

  template <typename T>
  std::enable_if_t<is_pointer_v<T>, bool> Write(const T &data) {
    if (data == nullptr) {
      return false;
    }
    intptr_t value = reinterpret_cast<intptr_t>(data);
    return Write(value);
  }

private:
  void *data_;
  size_t size_;
  size_t pos_;
  bool is_owner_;
};
} // namespace Anycubic::Plugins
