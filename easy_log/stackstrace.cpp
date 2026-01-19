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

thread_local std::atomic_int g_stack_depth = 0;
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
