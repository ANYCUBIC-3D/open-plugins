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

#include <tuple>
#include <type_traits>
#include <utility>

// 递归实现的辅助函数
template <typename F, typename Tuple, std::size_t... I>
inline void tuple_for_each_impl(Tuple &&tuple, F &&func,
                                std::index_sequence<I...>) {
  // 折叠表达式
  (func(std::get<I>(std::forward<Tuple>(tuple))), ...);
}

// 主函数 - 左值版本
template <typename F, typename... Args>
inline void tuple_for_each(std::tuple<Args...> &tuple, F &&func) {
  // 编译期判断 tuple 是否为空，空则不执行
  if constexpr (sizeof...(Args) > 0) {
    tuple_for_each_impl(tuple, std::forward<F>(func),
                        std::make_index_sequence<sizeof...(Args)>{});
  }
}

// 重载：右值版本
template <typename F, typename... Args>
inline void tuple_for_each(std::tuple<Args...> &&tuple, F &&func) {
  if constexpr (sizeof...(Args) > 0) {
    tuple_for_each_impl(std::move(tuple), std::forward<F>(func),
                        std::make_index_sequence<sizeof...(Args)>{});
  }
}