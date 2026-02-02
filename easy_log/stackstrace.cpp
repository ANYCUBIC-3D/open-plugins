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

#include "easy_log_export.hxx"

#include <assert.h>

#include <atomic>
#include <utility>
#include <stack>

thread_local std::atomic_int g_stack_depth = 0;
#ifndef NDEBUG
#define NAME_MAX_LENGTH 128
thread_local char g_module_name[NAME_MAX_LENGTH] = {0};
thread_local  std::stack<void*> func_address_stack;
thread_local void*              last_func_address = nullptr;
thread_local  const char* last_func_name = nullptr;
extern "C" {
LOG_API int get_stack_depth(bool entry) {
  int result = 0;
  if (entry) {
    func_address_stack.push(last_func_address);
    last_func_address       = nullptr;
    result                  = g_stack_depth.fetch_add(1) + 1;
  } else {
    assert(g_stack_depth.load() > 0);
    assert(!func_address_stack.empty());
    void* tmp =func_address_stack.top();
    func_address_stack.pop();
    assert(last_func_address!=nullptr && last_func_address==tmp && "Address mismatch");
    result = g_stack_depth.fetch_sub(1);
  }
  return result;
}

LOG_API const char *get_module_name(const char *name,const char* funcName, void *funcAddress) {
  assert(name != nullptr && "Module name should not be null");
  assert(strlen(name) > 0 && "Module name should not be empty");
  last_func_name = funcName;
  last_func_address = funcAddress;
  auto result = g_stack_depth.load();
  if (result == 0) {
      strncpy(g_module_name, name, NAME_MAX_LENGTH);
    }
  assert(strlen(g_module_name) > 0);
  return g_module_name;
}
}
#else
extern "C" {
LOG_API int get_stack_depth(bool entry) {
  if (entry) {
    return g_stack_depth.fetch_add(1) + 1;
  } else {
    assert(g_stack_depth.load() > 0);
    return g_stack_depth.fetch_sub(1);
  }
}
}
#endif // NDEBUG
