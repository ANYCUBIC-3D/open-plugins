﻿// Copyright (c) 2026 深圳市纵维立方科技有限公司
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
#include <dbghelp.h>
#include <mutex>
#include <psapi.h>
#include <windows.h>

// 共享的符号初始化函数
static bool ensure_sym_initialized() {
  static std::once_flag init_flag;
  static bool sym_initialized = false;

  std::call_once(init_flag, []() {
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
    sym_initialized = SymInitialize(GetCurrentProcess(), NULL, TRUE);
  });

  return sym_initialized;
}

std::string Symbols::module_name(void *address) {
  HMODULE hModule = NULL;

  // 获取包含该地址的模块句柄
  if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                        (LPCTSTR)address, &hModule)) {

    char modulePath[MAX_PATH];
    if (GetModuleFileNameA(hModule, modulePath, MAX_PATH)) {
      // 提取模块文件名（去掉路径）
      char *fileName = strrchr(modulePath, '\\');
      if (!fileName)
        fileName = strrchr(modulePath, '/');
      if (fileName)
        fileName++;
      else
        fileName = modulePath;

      return std::string(fileName);
    }
  }

  return "unknown";
}

std::string Symbols::function_name(void *func) {
  if (!ensure_sym_initialized()) {
    return "unknown";
  }

  char symbol_buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
  PSYMBOL_INFO symbol_info = (PSYMBOL_INFO)symbol_buffer;
  symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
  symbol_info->MaxNameLen = MAX_SYM_NAME;

  DWORD64 displacement = 0;

  if (SymFromAddr(GetCurrentProcess(), (DWORD64)func, &displacement,
                  symbol_info)) {
    // Windows 的 SymFromAddr 已经自动进行了 undname 处理
    // 直接返回易读的函数名
    return std::string(symbol_info->Name);
  } else {
    // 如果符号查找失败，返回地址的十六进制表示
    char buffer[32];
    sprintf_s(buffer, sizeof(buffer), "0x%p", func);
    return std::string(buffer);
  }
}

std::string Symbols::source_file(void *addr) {
  if (!ensure_sym_initialized()) {
    return "unknown";
  }

  // 尝试获取源文件和行号信息
  IMAGEHLP_LINE64 line_info;
  DWORD displacement = 0;
  line_info.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

  if (SymGetLineFromAddr64(GetCurrentProcess(), (DWORD64)addr, &displacement,
                           &line_info)) {
    // 成功获取到源文件信息
    std::string file_path(line_info.FileName);

    // 提取文件名（去掉路径）
    size_t last_backslash = file_path.find_last_of('\\');
    size_t last_slash = file_path.find_last_of('/');
    size_t last_sep =
        (last_backslash != std::string::npos) ? last_backslash : last_slash;

    if (last_sep != std::string::npos) {
      return file_path.substr(last_sep + 1);
    } else {
      return file_path;
    }
  }

  // 如果无法获取行号信息，尝试获取模块信息
  HMODULE hModule = NULL;
  if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                        (LPCTSTR)addr, &hModule)) {

    char modulePath[MAX_PATH];
    if (GetModuleFileNameA(hModule, modulePath, MAX_PATH)) {
      // 提取模块文件名（去掉路径）
      char *fileName = strrchr(modulePath, '\\');
      if (!fileName)
        fileName = strrchr(modulePath, '/');
      if (fileName)
        fileName++;
      else
        fileName = modulePath;

      return std::string(fileName);
    }
  }

  return "unknown";
}