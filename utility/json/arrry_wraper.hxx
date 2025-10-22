#pragma once
#include <stddef.h>
#include <stdexcept>
#include <stdint.h>
#include <type_traits>

namespace Anycubic::utility::json {

template <typename T> struct ArrayWrapper : public std::true_type {
  typedef T value_type;
  typedef std::add_pointer_t<value_type> pointer;

  typedef std::add_pointer_t<const value_type> const_pointer;

  // 迭代器相关
  const_pointer begin() const { return data_; }
  const_pointer end() const { return data_ + size_; }
  pointer begin() { return data_; }
  pointer end() { return data_ + size_; }

  // 元素访问
  value_type &at(int idx) {
    if (idx < 0 || idx >= static_cast<int>(size_))
      throw std::out_of_range("ArrayWrapper index out of range");
    return this->operator[](idx);
  }

  value_type &operator[](int idx) { return data_[idx]; }

  const value_type &operator[](int idx) const { return data_[idx]; }

  // 容量相关
  size_t size() const { return size_; }
  bool empty() const { return size_ == 0; }

  pointer data_ = nullptr;
  uintptr_t size_ = 0;
};

template <typename T> struct ArrayWrapper<T *> : public std::false_type {
  typedef T *value_type;
  typedef value_type *pointer;

  typedef const value_type *const_pointer;

  // 迭代器相关
  const_pointer begin() const { return data_; }
  const_pointer end() const { return data_ + size_; }
  pointer begin() { return data_; }
  pointer end() { return data_ + size_; }

  // 元素访问
  value_type at(int idx) {
    if (idx < 0 || idx >= static_cast<int>(size_))
      throw std::out_of_range("ArrayWrapper index out of range");
    return this->operator[](idx);
  }

  value_type operator[](int idx) { return data_[idx]; }
  const value_type operator[](int idx) const { return data_[idx]; }

  // 容量相关
  size_t size() const { return size_; }
  bool empty() const { return size_ == 0; }

  pointer data_ = nullptr;
  uintptr_t size_ = 0;
};
} // namespace Anycubic::utility::json