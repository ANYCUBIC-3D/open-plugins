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

#pragma once
#include "plugins_base_export.hxx"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <numeric>
#include <string>
#include <type_traits>

#include <wx/string.h>

namespace Anycubic::Plugins {

template <typename T>
constexpr bool is_trivial_v =
    std::is_arithmetic_v<T> ||
    (std::is_class_v<T> && std::is_standard_layout_v<T> &&
     std::is_trivial_v<T>);

template <typename T>
constexpr bool is_std_string_v = std::is_same_v<std::decay_t<T>, std::string>;

template <typename T>
constexpr bool is_c_string_v =
    std::is_pointer_v<T> && (std::is_same_v<std::decay_t<T>, char *> ||
                             std::is_same_v<std::decay_t<T>, const char *>);

template <typename T>
constexpr bool is_pointer_v = std::is_pointer_v<T> && !is_c_string_v<T>;

/**
 * @brief 输出流
 *
 */
struct PLGINS_EXPORT IStream {
  /**
   * @brief 输入流构造函数
   *
   * @param data 输入数据指针
   * @param size 输入数据大小
   */
  IStream(const void *data, size_t size);
  /**
   * @brief 输出流构造函数
   *
   * @param size 输出数据大小
   */
  IStream(size_t size);
  ~IStream(void);

  /**
   * @brief 读取数据
   *
   * @param data 输出数据指针
   * @param size 输出数据大小
   * @return size_t 实际读取数据大小
   */
  size_t Read(void *data, size_t size);

  /**
   * @brief 获取流大小
   *
   * @return size_t 流大小
   */
  size_t Size() const;

  /**
   * @brief 获取当前读取位置
   *
   * @return size_t 当前读取位置
   */
  size_t Tellg(void) const;
  /**
   * @brief 设置当前读取位置
   *
   * @param pos 新的读取位置
   * @return size_t 新的读取位置
   */
  size_t Seekg(size_t pos);

  /**
   * @brief 获取输入数据指针
   *
   * @return const void* 输入数据指针
   */
  const void *Data(void) const;

  /**
   * @brief 读取数据
   *
   * @param data 输出数据引用
   * @return std::enable_if_t<is_trivial_v<T> && !std::is_const_v<T>, bool>
   */
  template <typename T>
  std::enable_if_t<is_trivial_v<T> && !std::is_const_v<T>, bool> Read(T &data) {
    return sizeof(T) == Read(&data, sizeof(T));
  }

  /**
   * @brief 读取字符串
   *
   * @param data 输出字符串引用
   * @note 必需释放字符串内存
   * @return true 成功
   * @return false 失败
   */
  bool Read(const char *&data) {
    uint16_t length = 0;
    if (!Read(length)) {
      return false;
    }
    if (length == 0) {
      data = nullptr;
      return true;
    }
    auto buffer = (char *)malloc(length + 1);
    data = buffer;
    if (buffer == nullptr) {
      return false;
    }
    memset(buffer, 0, length + 1);

    return Read(buffer, length) == length;
  }

  /**
   * @brief 读取字符串
   *
   * @param data 输出字符串引用
   * @return true 成功
   * @return false 失败
   */
  bool Read(std::string &data) {
    uint16_t length = 0;
    if (!Read(length)) {
      return false;
    }
    if (length == 0) {
      return true;
    }
    data.resize(length);
    return Read(data.data(), data.size()) == length;
  }

  /**
   * @brief 读取wxString
   *
   * @param data 输出wxString引用
   * @return true 成功
   * @return false 失败
   */
  bool Read(wxString &data) {
    std::string buffer;
    if (Read(buffer)) {
      data = wxString::FromUTF8(buffer);
      return true;
    }
    return false;
  }

  /**
   * @brief 读取指针
   *
   * @param data 输出指针引用
   * @return bool 成功true,其他false
   */
  template <typename T>
  std::enable_if_t<std::is_pointer_v<T>, bool> Read(T &data) {
    intptr_t ptr = 0;
    if (!Read(ptr)) {
      return false;
    }
    data = reinterpret_cast<T>(ptr);
    return true;
  }

private:
  const void *data_; ///< 输入数据指针
  size_t size_;      ///< 输入数据大小
  size_t pos_;       ///< 当前读取位置
  bool is_owner_;    ///< 是否拥有数据指针
};

/**
 * @brief 输出流
 *
 */
struct PLGINS_EXPORT OStream {

  /**
   * @brief Construct a new OStream object
   *
   * @param buffer 输出数据缓冲区
   * @param size 输出数据缓冲区大小
   */
  OStream(void *buffer, size_t size);

  /**
   * @brief Construct a new OStream object
   *
   * @param size 输出数据缓冲区大小
   */
  OStream(size_t size = 0);
  ~OStream(void);

  /**
   * @brief 写入数据
   *
   * @param data 输入数据指针
   * @param size 输入数据大小
   * @return size_t 实际写入数据大小
   */
  size_t Write(const void *data, size_t size);

  /**
   * @brief 获取流大小
   *
   * @return size_t 流大小
   */
  size_t Size() const;

  /**
   * @brief 获取输出数据缓冲区指针
   *
   * @return void* 输出数据缓冲区指针
   */
  void *Data(void);

  /**
   * @brief 获取当前写入位置
   *
   * @return size_t 当前写入位置
   */
  size_t Tellp(void) const;
  /**
   * @brief 设置当前写入位置
   *
   * @param pos 新的写入位置
   * @return size_t 新的写入位置
   */
  size_t Seekp(size_t pos);

  /**
   * @brief 调整输出数据缓冲区大小
   *
   * @param size 新的输出数据缓冲区大小
   * @return size_t 新的输出数据缓冲区大小
   */
  size_t Resize(size_t size);

  /**
   * @brief 写入数据
   *
   * @param data 输入数据引用
   * @return bool 成功true,其他false
   */
  template <typename T>
  std::enable_if_t<is_trivial_v<T>, bool> Write(const T &data) {
    return sizeof(T) == Write(&data, sizeof(T));
  }
  /**
   * @brief 写入字符串
   *
   * @param data 输入字符串引用
   * @return true 成功
   * @return false 失败
   */
  bool Write(const std::string &data) {
    assert(data.size() <= std::numeric_limits<uint16_t>::max());
    uint16_t length = static_cast<uint16_t>(data.size());
    if (length == 0) {
      return Write(length);
    } else {
      return Write(length) && Write(data.data(), length);
    }
  }
  /**
   * @brief 写入wxString
   *
   * @param data 输入wxString引用
   * @return true 成功
   * @return false 失败
   */
  bool Write(const wxString &data) { return Write(data.utf8_string()); }

  /**
   * @brief 写入C字符串
   *
   * @param data 输入C字符串指针
   * @return true 成功
   * @return false 失败
   */
  bool Write(const char *data) {
    uint16_t length = 0;
    if (data != nullptr) {
      length = static_cast<uint16_t>(strlen(data));
    }
    // 如果写失败了呢？
    Write(length);
    if (length > 0) {
      return Write(data, length);
    }
    return true;
  }

  /**
   * @brief 写入指针
   *
   * @param data 输入指针引用
   * @return std::enable_if_t<std::is_pointer_v<T>, bool> 成功true,其他false
   */
  template <typename T>
  std::enable_if_t<std::is_pointer_v<T>, bool> Write(T data) {
    // 允许nullptr
    intptr_t value = reinterpret_cast<intptr_t>(data);
    return Write(value);
  }

private:
  void *data_;    ///< 输出数据缓冲区指针
  size_t size_;   ///< 输出数据缓冲区大小
  size_t pos_;    ///< 当前写入位置
  bool is_owner_; ///< 是否拥有数据指针
};
} // namespace Anycubic::Plugins
