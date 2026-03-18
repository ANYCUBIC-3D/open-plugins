// Copyright (c) 2026 深圳市纵维立方科技有限公司
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

#include <iterator>

namespace Anycubic::utility {

class range {
private:
  class iterator {
  private:
    int current_;
    int step_;

  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = int;
    using difference_type = int;
    using pointer = int *;
    using reference = int &;

    iterator(int current, int step = 1) : current_(current), step_(step) {}

    int operator*() const { return current_; }

    iterator &operator++() {
      current_ += step_;
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      current_ += step_;
      return tmp;
    }

    bool operator==(const iterator &other) const {
      // 对于正向迭代，当current >= other.current时认为相等
      if (step_ > 0) {
        return current_ >= other.current_;
      }
      // 对于反向迭代，当current <= other.current时认为相等
      else {
        return current_ <= other.current_;
      }
    }

    bool operator!=(const iterator &other) const { return !(*this == other); }
  };

  int start_;
  int stop_;
  int step_;

public:
  // 单参数构造函数：range(stop)
  explicit range(int stop) : start_(0), stop_(stop), step_(1) {
    if (stop_ < start_) {
      step_ = -1;
    }
  }

  // 双参数构造函数：range(start, stop)
  range(int start, int stop) : start_(start), stop_(stop), step_(1) {
    if (stop_ < start_) {
      step_ = -1;
    }
  }

  // 三参数构造函数：range(start, stop, step)
  range(int start, int stop, int step)
      : start_(start), stop_(stop), step_(step) {
    if (step_ == 0) {
      step_ = 1; // 避免除零错误
    }
  }

  iterator begin() const { return iterator(start_, step_); }

  iterator end() const {
    // 计算结束位置，考虑步长
    if (step_ > 0) {
      return iterator(stop_ + (stop_ - start_) % step_, step_);
    } else {
      return iterator(stop_ + (stop_ - start_) % step_, step_);
    }
  }
};

// 便捷函数，用于创建range对象

/**
 * @brief 创建一个范围对象
 *
 * @param stop 范围结束值（不包含在范围内）
 * @return range 范围对象
 */
inline range make_range(int stop) { return range(stop); }

/**
 * @brief 创建一个范围对象
 *
 * @param start 范围开始值（包含在范围内）
 * @param stop 范围结束值（不包含在范围内）
 * @return range 范围对象
 */
inline range make_range(int start, int stop) { return range(start, stop); }

/**
 * @brief 创建一个范围对象
 *
 * @param start 范围开始值（包含在范围内）
 * @param stop 范围结束值（不包含在范围内）
 * @param step 范围步长
 * @return range 范围对象
 */
inline range make_range(int start, int stop, int step) {
  return range(start, stop, step);
}

} // namespace Anycubic::utility
