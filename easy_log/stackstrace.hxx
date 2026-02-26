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

#include "log.hxx"

#include <assert.h>

#include <iostream>

template <typename T> auto ac_to_string(T &&t);

#if ENABLE_STRACE

#include <sstream>
#include <string>

extern "C" {
/**
 * @brief Get the stack depth
 *
 * @param entry 是否是入口
 * @return LOG_API 栈深度
 */
LOG_API int get_stack_depth(bool entry);
#ifndef NDEBUG
LOG_API const char *get_module_name(const char *name, const char *funcName,
                                    void *funcAddress);
#else
#define get_module_name(name, ...) name
#endif
}

#ifdef _MSC_VER
    #define GET_CALLER_ADDRESS() _ReturnAddress()
#else
    #define GET_CALLER_ADDRESS() __builtin_return_address(0)
#endif

namespace anycubic::tracer {

template <bool entry, typename... Args>
std::string strace_format(Args &&...args) {

  constexpr int STACKTRACE_SYMBOL_MIN_LENGTH = 9; // 最小符号长度
  std::stringstream ss;

  int stack_depth = get_stack_depth(entry);

  if constexpr (entry) {
    std::string stack_symbol(stack_depth, '>');
    ss << "ENTRY " << stack_symbol << " " << stack_depth << " ";
  } else {
    std::string stack_symbol(stack_depth, '<');
    ss << "LEAVE " << stack_symbol << " " << stack_depth << " ";
  }
  if constexpr (sizeof...(args) > 0) {
    ss << " " << logger::ac_format(std::forward<Args>(args)...);
  }

  return ss.str();
}

} // namespace anycubic::tracer

#ifndef MODULE_NAME
#error "MODULE_NAME must be defined"
#endif

#define FUNC_ENTRY2(...)                                                       \
  LOG_CORE(get_module_name(MODULE_NAME, __func__, GET_CALLER_ADDRESS()),       \
           anycubic::logger::trace, anycubic::tracer::strace_format<true>,     \
           __VA_ARGS__)
#define FUNC_LEAVE2(...)                                                       \
  LOG_CORE(get_module_name(MODULE_NAME, __func__, GET_CALLER_ADDRESS()),       \
           anycubic::logger::trace, anycubic::tracer::strace_format<false>,    \
           __VA_ARGS__)

#define FUNC_ENTRYID(id) FUNC_ENTRY2(ac_to_string(id))
#define FUNC_LEAVEID(id) FUNC_LEAVE2(ac_to_string(id))
#define FUNC_ENTRY FUNC_ENTRY2("");
#define FUNC_LEAVE FUNC_LEAVE2("");
#else
#define FUNC_ENTRY
#define FUNC_LEAVE
#define FUNC_ENTRYID(id)
#define FUNC_LEAVEID(id)
#endif
#ifdef STRACE_RELEASE
#define DFUNC_ENTRY FUNC_ENTRY
#define DFUNC_LEAVE FUNC_LEAVE
#define DFUNC_ENTRYID FUNC_ENTRYID
#define DFUNC_LEAVEID FUNC_LEAVEID
#elif defined(NDEBUG)
#define DFUNC_ENTRY
#define DFUNC_LEAVE
#define DFUNC_ENTRYID(x)
#define DFUNC_LEAVEID(x)
#else
#define DFUNC_ENTRY FUNC_ENTRY
#define DFUNC_LEAVE FUNC_LEAVE
#define DFUNC_ENTRYID FUNC_ENTRYID
#define DFUNC_LEAVEID FUNC_LEAVEID
#endif
