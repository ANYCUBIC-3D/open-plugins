#include "plugins_stream.hxx"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace Anycubic::Plugins {
IStream::IStream(const void *data, size_t size)
    : data_(data), size_(size), pos_(0), is_owner_(false) {}
IStream::IStream(size_t size)
    : data_(nullptr), size_(size), pos_(0), is_owner_(true) {
  data_ = malloc(size);
  assert(data_ != nullptr);
}
IStream::~IStream(void) {
  if (is_owner_) {
    free(const_cast<void *>(data_));
    data_ = nullptr;
  }
}
size_t IStream::Read(void *data, size_t size) {
  if (pos_ + size > size_) {
    size = size_ - pos_;
  }
  memcpy(data, (char *)data_ + pos_, size);
  pos_ += size;

  return size;
}
size_t IStream::Read(char *buffer, size_t size) {
  auto pos = Tellg();
  uint16_t bytes = 0;
  if (!Read(&bytes)) {
    Seekg(pos);
    return 0;
  }
  if (bytes > size) {
    // 保证下一次读取有效
    Seekg(pos);
    return 0;
  }
  return Read(buffer, bytes);
}

size_t IStream::Size() const { return size_; }
size_t IStream::Tellg(void) const { return pos_; }
size_t IStream::Seekg(size_t pos) { return pos_ = pos; }
const void *IStream::Data(void) const { return data_; }

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
OStream::OStream(void *buffer, size_t size)
    : data_(buffer), size_(size), pos_(0), is_owner_(false) {}
OStream::OStream(size_t size)
    : data_(nullptr), size_(0), pos_(0), is_owner_(false) {
  Resize(size);
}
OStream::~OStream(void) {
  if (is_owner_) {
    free(data_);
    data_ = nullptr;
  }
}
size_t OStream::Write(const char *buffer, size_t size) {
  if (pos_ + size + sizeof(uint16_t) > size_) {
    return 0;
  }
  Write(static_cast<uint16_t>(size));
  return Write(buffer, size);
}
size_t OStream::Write(const void *data, size_t size) {
  if (pos_ + size > size_) {
    return 0;
  }
  memcpy(static_cast<char *>(data_) + pos_, data, size);
  pos_ += size;
  return size;
}
size_t OStream::Size() const { return size_; }
void *OStream::Data(void) { return data_; }
size_t OStream::Tellp(void) const { return pos_; }
size_t OStream::Seekp(size_t pos) { return pos_ = pos; }
size_t OStream::Resize(size_t size) {
  if (size <= size_) {
    return size_;
  }

  void *new_data = malloc(size);
  if (new_data == nullptr) {
    return size_; // 分配失败返回原大小
  }

  if (data_ != nullptr) {
    memcpy(new_data, data_, size_); // 只拷贝有效数据
    if (is_owner_) {
      free(data_); // 释放旧内存
    }
  }

  data_ = new_data;
  size_ = size;
  is_owner_ = true; // 确保所有权一致
  return size;
}
} // namespace Anycubic::Plugins
