#pragma once
#include "function_traits.hxx"
#include "pack.hxx"
#include "plugins.hxx"

#include <easy_log/stackstrace.hxx>

#include <functional>

#include <boost/preprocessor/stringize.hpp>

#define REGISTER_FUNCATION(type, func)                                         \
  AddFunction(BOOST_PP_STRINGIZE(func), Anycubic::Plugins::make_call(          \
                                            &type::func, this))

namespace Anycubic::Plugins {
using FuncationType = std::function<void(IStream *data, OStream *result)>;

/**
 * @brief 函数包装器类
 * @note 用于将函数包装为请求处理器
 *
 */
class FuncationWrapper : public RequestHandler {
public:
  void Execute(IStream *data, OStream *result) override {
    FUNC_ENTRY;
    func_(data, result);
    FUNC_LEAVE;
  }
  void Destroy(void) override {
    FUNC_ENTRY2("object pointer:{}", (intptr_t)this);
    delete this;
    FUNC_LEAVE;
  }

  static RequestHandler *Create(FuncationType &&func) {
    FUNC_ENTRY;
    auto v = new FuncationWrapper(std::move(func));
    FUNC_LEAVE2("object pointer:{}", (intptr_t)v);
    return v;
  }

private:
  FuncationWrapper(FuncationType &&func) : func_(func) {
    FUNC_ENTRY;
    FUNC_LEAVE;
  }
  virtual ~FuncationWrapper() {
    FUNC_ENTRY;
    FUNC_LEAVE;
  }

private:
  FuncationType func_;
};

/**
 * @brief 创建函数包装器
 *
 * @param func 函数指针
 * @param self 类实例指针
 * @return RequestHandler* 函数包装器实例
 */
template <typename Function, typename Self>
RequestHandler *make_call(const Function &func, Self *self) {
  using func_traits = function_traits<Function>;
  auto h = [func, self](IStream *data, OStream *result) {
    using ret_type = std::decay_t<typename func_traits::return_type>;
    using arg_types = typename func_traits::bare_tuple_type;
    FUNC_ENTRY;
    arg_types args;
    constexpr size_t arg_count = std::tuple_size_v<arg_types>;
    if constexpr (arg_count > 0) {
      unpack_args(args, data);
    }

    if constexpr (std::is_void_v<ret_type>) {
      std::apply(func, std::tuple_cat(std::make_tuple(self), args));
      FUNC_LEAVE;
    } else {
      ret_type ret =
          std::apply(func, std::tuple_cat(std::make_tuple(self), args));
      pack_result(result, ret);
      FUNC_LEAVE;
    }
    pack_free(args);
  };
  return FuncationWrapper::Create(std::move(h));
}

/**
 * @brief 调用插件函数
 *
 * @param router 插件路由器
 * @param plugin 插件名称
 * @param fname 函数名称
 * @param args 函数参数
 * @return ret_type 函数返回值
 */
template <typename ret_type, typename router_type, typename... Args>
ret_type dispatch_call(router_type *router, const char *plugin,
                       const char *fname, Args &&...args) {
  FUNC_ENTRY;
  std::vector<char> argsData;
  constexpr auto args_size = sizeof...(Args);
  if constexpr (args_size > 0) {
    auto bytes = get_bytes(args...);
    argsData.resize(bytes);
    OStream os(argsData.data(), bytes);
    pack_result(&os, std::forward<Args>(args)...);
  }

  IStream is(argsData.empty() ? nullptr : argsData.data(), argsData.size());
  OStream os;
  auto result = router->ExecuteFunction(plugin, fname, &is, &os);
  assert(result); // 未执行的直接标记为失败
  if (!result) {
    FUNC_LEAVE2("{}::{} not found or execute failed", plugin, fname);
    return ret_type();
  }

  if constexpr (!std::is_void_v<ret_type>) {
    IStream rs(os.Data(), os.Size());
    ret_type ret;
    unpack_args_read(&rs, ret);
    FUNC_LEAVE;
    return ret;
  } else {
    FUNC_LEAVE;
  }
}

/**
 * @brief 调用插件函数
 *
 * @param host 插件宿主
 * @param plugin 插件名称
 * @param fname 函数名称
 * @param args 函数参数
 * @return ret_type 函数返回值
 */
template <typename ret_type, typename... Args>
ret_type dispatch_call(PluginHost *host, const char *plugin, const char *fname,
                       Args &&...args) {
  FUNC_ENTRY;
  auto router = host->Router();
  assert(router != nullptr);
  auto result = dispatch_call<ret_type>(router, plugin, fname,
                                        std::forward<Args>(args)...);
  FUNC_LEAVE;
  return result;
}
} // namespace Anycubic::Plugins