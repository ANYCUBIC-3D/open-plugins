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
#include "stack.hpp"
#include "symbol.hpp"

#include <fmt/format.h>

#include <easy_log/log.hxx>

class Instrumentation {
public:
  static void enter(void *addr) {
    std::string line;
    if (auto r = add_stack(); r == 0) {
      // 获取详细栈信息
      LOG_INFO(Symbols::strace_trace(r));
    } else {
      // 获取调用函数详细信息
      LOG_INFO("{}{} {} {}", std::string(r, '\t'), Symbols::function_name(addr),
               Symbols::source_file(addr), Symbols::module_name(addr));
    }
  }
  static void exit(void *addr) {
    auto r = sub_stack();
    LOG_INFO("{}{} exit", std::string(r, '\t'), Symbols::function_name(addr));
    // 记录日志
  }
};