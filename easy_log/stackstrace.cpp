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
