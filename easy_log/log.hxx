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
#include "easy_log_export.hxx"

#include "to_string.hxx"

#include <stdio.h>

#include <fmt/format.h>

#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/if.hpp>

namespace anycubic::logger {
enum level_enum : int {
  trace = 0,
  debug,
  info,
  warn,
  err,
  critical,
  off,
  n_levels
};
#ifndef NDEBUG
#define DEFAULT_PATTERN "%Y%m%d%H%M.%e %l %t %s:%# %! %v"
#else
#define DEFAULT_PATTERN "%Y%m%d%H%M %n %l %t %s:%# %v"
#endif // NDEBUG
struct source_loc {
  source_loc() = default;
  source_loc(const char *filename_in, int line_in, const char *funcname_in)
      : filename{filename_in}, line{line_in}, funcname{funcname_in} {}

  bool empty() const noexcept { return line <= 0; }
  const char *filename{nullptr};
  int line{0};
  const char *funcname{nullptr};
};

bool LOG_API setup_log(const char *path, level_enum default_level,
                       bool enalbe_console = false);
bool LOG_API enable_debug(void);
bool LOG_API register_logger(const char *mname, bool compression = true);
bool LOG_API unregister_logger(const char *mname);
bool LOG_API set_level(const char *mname, level_enum level);

// clang-format off
/*************************************************************************
 * 自定格式说明
 * %c Date and time representation (Thu Aug 23 15:35:46 2014)
 * %C year - 2 digit
 * %D Short MM/DD/YY date, equivalent to %m/%d/%y 08/23/01
 * %x Short MM/DD/YY date, equivalent to %m/%d/%y 08/23/01
 * %Y year - 4 digit
 * %m month 1-12
 * %d day of month 1-31
 * %H hours in 24 format  0-23
 * %I hours in 12 format  1-12
 * %M minutes 0-59
 * %S seconds 0-59
 * %e milliseconds
 * %f microseconds
 * %F nanoseconds
 * %p AM/PM
 * %r 12 hour clock 02:55:02 pm
 * %R 24-hour HH:MM time, equivalent to %H:%M
 * %T ISO 8601 time format (HH:MM:SS), equivalent to %H:%M:%S
 * %X ISO 8601 time format (HH:MM:SS), equivalent to %H:%M:%S
 * %z ISO 8601 offset from UTC in timezone (+-HH:MM)
 * %t Thread id
 * %A Full weekday name  { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" }
 * %a Abbreviated weekday name { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" }
 * %b Abbreviated month { "Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sept", "Oct", "Nov", "Dec" }
 * %h Abbreviated month { "Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sept", "Oct", "Nov", "Dec" }
 * %B Full month name { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" }
 * %n name -- sign name
 * %l level { "trace", "debug", "info",  "warning", "error", "critical", "off" }
 * %L short level { "T", "D", "I", "W", "E", "C", "O" }
 * %v user fmt str
 * %@ source location
 * %! func name
 * %+ Full info formatter pattern: [%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v
 * ***********************************************************************/
bool LOG_API set_pattern(const char *mname, const char *pattern=DEFAULT_PATTERN);
// clang-format on

/**
 * @brief 判断是否需要输出日志
 *
 * @param mname 日志模块名
 * @param level 日志级别
 * @return true 需要输出
 * @return false 不需要输出
 */
bool LOG_API should_log(const char *mname, level_enum level);

/**
 * @brief 输出日志
 *
 * @param mname 日志模块名
 * @param loc 日志位置
 * @param level 日志级别
 * @param msg 日志消息
 * @return true 输出成功
 * @return false 输出失败
 */
bool LOG_API log_(const char *mname, const source_loc &loc, level_enum level,
                  const char *msg);

template <typename fmt_type, typename... Args>
std::string ac_format(const fmt_type &fmt, Args &&...args) {
  if constexpr (sizeof...(args) == 0) {
    return fmt;
  } else {
    return fmt::format(fmt::runtime(fmt),
                       ac_to_string_if_need(std::forward<Args>(args))...);
  }
}

} // namespace anycubic::logger

#ifndef DEBUG_CHECK_ARGS
#define CHECK_ARGS(...)
#endif // DEBUG_CHECK_ARGS

#define LOG_CORE(mname, level, func, ...)                                      \
  {                                                                            \
    CHECK_ARGS(__VA_ARGS__);                                                   \
    auto log_module_name = mname;                                              \
    if (anycubic::logger::should_log(log_module_name, level)) {                \
      /*提前过滤不输出的日志，避免消耗 */                                      \
      anycubic::logger::log_(                                                  \
          log_module_name,                                                     \
          anycubic::logger::source_loc{__FILE__, __LINE__, __func__}, level,   \
          func(__VA_ARGS__).c_str());                                          \
    }                                                                          \
  }

// 日志宏定义
#define LOG_TRACE_(mname, ...)                                                 \
  LOG_CORE(mname, anycubic::logger::trace, anycubic::logger::ac_format,        \
           __VA_ARGS__)
#define LOG_DEBUG_(mname, ...)                                                 \
  LOG_CORE(mname, anycubic::logger::debug, anycubic::logger::ac_format,        \
           __VA_ARGS__)
#define LOG_INFO_(mname, ...)                                                  \
  LOG_CORE(mname, anycubic::logger::info, anycubic::logger::ac_format,         \
           __VA_ARGS__)
#define LOG_WARN_(mname, ...)                                                  \
  LOG_CORE(mname, anycubic::logger::warn, anycubic::logger::ac_format,         \
           __VA_ARGS__)
#define LOG_ERROR_(mname, ...)                                                 \
  LOG_CORE(mname, anycubic::logger::err, anycubic::logger::ac_format,          \
           __VA_ARGS__)
#define LOG_CRITICAL_(mname, ...)                                              \
  LOG_CORE(mname, anycubic::logger::critical, anycubic::logger::ac_format,     \
           __VA_ARGS__)
// clang-format off

// 条件日志宏定义
#define LOG_TRACE_IF_(cond, mname, ...)                                         \
  if (cond)LOG_TRACE_(mname, __VA_ARGS__)
#define LOG_DEBUG_IF_(cond, mname, ...)                                         \
  if (cond)LOG_DEBUG_(mname, __VA_ARGS__)
#define LOG_INFO_IF_(cond, mname, ...)                                          \
  if (cond)LOG_INFO_(mname, __VA_ARGS__)
#define LOG_WARN_IF_(cond, mname, ...)                                          \
  if (cond)LOG_WARN_(mname, __VA_ARGS__)
#define LOG_ERROR_IF_(cond, mname, ...)                                         \
  if (cond)LOG_ERROR_(mname, __VA_ARGS__)
#define LOG_CRITICAL_IF_(cond, mname, ...)                                      \
  if (cond)LOG_CRITICAL_(mname, __VA_ARGS__)


#ifdef MODULE_NAME

// 模块日志宏定义
#define LOG_TRACE(...) LOG_TRACE_(MODULE_NAME, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_DEBUG_(MODULE_NAME, __VA_ARGS__)
#define LOG_INFO(...) LOG_INFO_(MODULE_NAME, __VA_ARGS__)
#define LOG_WARN(...) LOG_WARN_(MODULE_NAME, __VA_ARGS__)
#define LOG_ERROR(...) LOG_ERROR_(MODULE_NAME, __VA_ARGS__)
#define LOG_CRITICAL(...) LOG_CRITICAL_(MODULE_NAME, __VA_ARGS__)


// 模块条件日志宏定义
#define LOG_TRACE_IF(cond, ...) LOG_TRACE_IF_(cond, MODULE_NAME, __VA_ARGS__)
#define LOG_DEBUG_IF(cond, ...) LOG_DEBUG_IF_(cond, MODULE_NAME, __VA_ARGS__)
#define LOG_INFO_IF(cond, ...) LOG_INFO_IF_(cond, MODULE_NAME, __VA_ARGS__)
#define LOG_WARN_IF(cond, ...) LOG_WARN_IF_(cond, MODULE_NAME, __VA_ARGS__)
#define LOG_ERROR_IF(cond, ...) LOG_ERROR_IF_(cond, MODULE_NAME, __VA_ARGS__)
#define LOG_CRITICAL_IF(cond, ...)    LOG_CRITICAL_IF_(cond, MODULE_NAME, __VA_ARGS__)

// 模块日志管理宏定义
#define REGISTER_LOGGER(x) anycubic::logger::register_logger(MODULE_NAME,x)
#define UNREGISTER_LOGGER() anycubic::logger::unregister_logger(MODULE_NAME)
#define SET_LEVEL(level) anycubic::logger::set_level(MODULE_NAME, level)
#define SET_PATTERN(pattern) anycubic::logger::set_pattern(MODULE_NAME, pattern)
#define SET_OUTPUT(file) anycubic::logger::set_output(MODULE_NAME, file)

#define SetupEnvName(name, empty) REGISTER_LOGGER(false)
#endif // MODULE_NAME

// clang-format on
