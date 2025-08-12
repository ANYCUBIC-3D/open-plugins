#pragma once

#include <functional>
#include <tuple>
#include <type_traits>

namespace Anycubic::Plugins {
template <typename T>
using remove_const_reference_t =
    typename std::remove_const<typename std::remove_reference<T>::type>::type;

template <typename T> struct function_traits;

template <typename Ret, typename... Args> struct function_traits<Ret(Args...)> {
public:
  enum { arity = sizeof...(Args) }; ///< 参数数量
  typedef Ret return_type;          ///< 返回值类型
  typedef Ret (*pointer)(Args...);  ///< 函数指针

  typedef std::tuple<remove_const_reference_t<Args>...>
      bare_tuple_type; ///< 简单参数类型（反序列化不能的引用和const)
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
