#pragma once

#include "log.hxx"

#include <assert.h>

#include <iostream>

template <typename T> auto ac_to_string(T &&t);

#if ENABLE_STRACE

#include <sstream>
#include <string>

namespace anycubic::tracer {

template <bool entry, typename... Args>
std::string strace_format(Args &&...args) {

  constexpr int STACKTRACE_SYMBOL_MIN_LENGTH = 9; // 最小符号长度
  std::stringstream ss;

  if constexpr (entry) {
    ss << ">>>>>>>";
  } else {
    ss << "<<<<<<<<";
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
  LOG_CORE(MODULE_NAME, anycubic::logger::trace,                               \
           anycubic::tracer::strace_format<true>, __VA_ARGS__)
#define FUNC_LEAVE2(...)                                                       \
  LOG_CORE(MODULE_NAME, anycubic::logger::trace,                               \
           anycubic::tracer::strace_format<false>, __VA_ARGS__)

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
