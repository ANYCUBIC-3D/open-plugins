#pragma once

#include <functional>
#include <tuple>
#include <type_traits>

namespace Anycubic::Plugins {
template <typename T>
using remove_const_reference_t =
    std::remove_const_t<std::remove_reference_t<T>>;

template <typename T> struct function_traits;

template <typename Ret, typename... Args> struct function_traits<Ret(Args...)> {
public:
  enum { arity = sizeof...(Args) }; ///< 参数数量
  using return_type = Ret;          ///< 返回值类型
  using pointer = Ret (*)(Args...); ///< 函数指针

  using bare_tuple_type = std::tuple<remove_const_reference_t<
      Args>...>; ///< 简单参数类型（反序列化不能的引用和const)
};

/**
 * 普通函数特征提取
 */
template <typename Ret, typename... Args>
struct function_traits<Ret (*)(Args...)> : function_traits<Ret(Args...)> {};

/**
 * 非const成员函数特征提取
 */
template <typename ReturnType, typename ClassType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...)>
    : function_traits<ReturnType(Args...)> {};

/**
 * const成员函数特征提取
 */
template <typename ReturnType, typename ClassType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const>
    : function_traits<ReturnType(Args...)> {};

/**
 * 仿函数据特征提取
 */
template <typename Ret, typename... Args>
struct function_traits<std::function<Ret(Args...)>>
    : function_traits<Ret(Args...)> {};
} // namespace Anycubic::Plugins
