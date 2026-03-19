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

#include "symbol.hpp"
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <stdio.h>
#include <string.h>

std::string Symbols::module_name(void *address) {
  Dl_info info;
  if (dladdr(address, &info) && info.dli_fname) {
    // 提取模块文件名（去掉路径）
    const char *fileName = strrchr(info.dli_fname, '/');
    if (fileName)
      fileName++;
    else
      fileName = info.dli_fname;

    return std::string(fileName);
  }

  return "unknown";
}
std::string Symbols::function_name(void *func) {
  Dl_info info;
  if (dladdr(func, &info) && info.dli_sname) {
    // 对C++名称进行demangle，获取易读格式
    int status;
    char *demangled =
        abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);

    if (status == 0 && demangled) {
      std::string result = demangled;
      free(demangled);
      return result;
    } else {
      // 如果demangle失败，返回原始符号名
      return std::string(info.dli_sname);
    }
  }

  // 如果查找失败，返回地址
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "0x%p", func);
  return std::string(buffer);
}

std::string Symbols::source_file(void *addr) {
  Dl_info info;
  if (dladdr(addr, &info) && info.dli_sname) {
    // 尝试获取符号的源文件信息
    // 注意：dladdr 本身不提供源文件信息，但我们可以通过其他方式获取

    // 首先尝试使用 backtrace 和 backtrace_symbols 获取更详细的信息
    void *buffer[1];
    buffer[0] = addr;
    char **symbols = backtrace_symbols(buffer, 1);

    if (symbols) {
      std::string symbol_info(symbols[0]);
      free(symbols);

      // 解析 backtrace_symbols 的输出格式，通常包含源文件信息
      // 格式通常是：module(function+offset) [地址]
      // 或者：源文件:行号 (函数) [地址]

      size_t paren_pos = symbol_info.find('(');
      if (paren_pos != std::string::npos) {
        // 检查是否有源文件信息在括号前
        std::string before_paren = symbol_info.substr(0, paren_pos);

        // 如果包含路径分隔符，可能是源文件路径
        if (before_paren.find('/') != std::string::npos ||
            before_paren.find('.') != std::string::npos) {
          return before_paren;
        }
      }

      // 尝试查找冒号（可能是文件:行号格式）
      size_t colon_pos = symbol_info.find(':');
      if (colon_pos != std::string::npos) {
        size_t space_before = symbol_info.rfind(' ', colon_pos);
        if (space_before != std::string::npos) {
          return symbol_info.substr(space_before + 1,
                                    colon_pos - space_before - 1);
        }
      }
    }
  }

  return "unknown";
}