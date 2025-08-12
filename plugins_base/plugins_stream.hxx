#pragma once
#include "plugins_base_export.hxx"

#include <stddef.h>

#include <type_traits>

namespace Anycubic::Plugins {
struct PLGINS_EXPORT IStream {
  IStream(const void *data, size_t size);
  IStream(size_t size);
  ~IStream(void);
  size_t Read(void *data, size_t size);
  size_t Read(char *buffer, size_t size);
  size_t Size() const;
  size_t Tellg(void) const;
  size_t Seekg(size_t pos);
  const void *Data(void) const;

  template <typename T> bool Read(T *data) {
    if constexpr (std::is_arithmetic_v<T> ||
                  (std::is_standard_layout_v<T> && std::is_trivial_v<T>)) {
      return sizeof(T) == Read(data, sizeof(T));
    } else if (std::is_pointer_v<T>) {
      return sizeof(T) == Read(*data, sizeof(T));
    } else {
    }
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
  size_t Write(const char *buffer, size_t size);
  size_t Size() const;
  void *Data(void);
  size_t Tellp(void) const;
  size_t Seekp(size_t pos);
  size_t Resize(size_t size);

  template <typename T> bool Write(const T &data) {
    if constexpr (std::is_arithmetic_v<T> ||
                  (std::is_standard_layout_v<T> && std::is_trivial_v<T>)) {
      return sizeof(T) == Write(&data, sizeof(T));
    } else if (std::is_pointer_v<T>) {
      return sizeof(T) == Write(data, sizeof(T));
    } else {
    }
  }

private:
  void *data_;
  size_t size_;
  size_t pos_;
  bool is_owner_;
};
} // namespace Anycubic::Plugins