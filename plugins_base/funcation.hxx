#pragma once
#include "function_traits.hxx"
#include "pack.hxx"
#include "plugins.hxx"

#include <functional>

#include <boost/preprocessor/stringize.hpp>

#define REGISTER_FUNCATION(type, func)                                         \
  AddFunction(BOOST_PP_STRINGIZE(func), Anycubic::Plugins::make_call(          \
                                            &type::func, this))

namespace Anycubic::Plugins {
using FuncationType = std::function<void(IStream *data, OStream *result)>;
class FuncationWrapper : public RequestHandler {
public:
  FuncationWrapper(const FuncationType &func) : func_(func) {}
  void Execute(IStream *data, OStream *result) override { func_(data, result); }
  void Destroy() override { delete this; }

private:
  FuncationType func_;
};

template <typename Function, typename Self>
RequestHandler *make_call(const Function &func, Self *self) {
  using func_traits = function_traits<Function>;
  auto h = [func, self](IStream *data, OStream *result) {
    using ret_type = std::decay_t<typename func_traits::return_type>;
    typename func_traits::bare_tuple_type args;
    if constexpr (std::tuple_size_v<typename func_traits::bare_tuple_type> >
                  0) {
      unpack_args(args, data);
    }

    if constexpr (std::is_void_v<ret_type>) {
      std::apply(func, std::tuple_cat(std::make_tuple(self), args));
    } else {
      ret_type ret =
          std::apply(func, std::tuple_cat(std::make_tuple(self), args));
      pack_result(result, ret);
    }
  };
  return new FuncationWrapper(std::move(h));
}

template <typename ret_type, typename... Args>
ret_type dispatch_call(PluginRouter *router, const char *plugin,
                       const char *fname, Args &&...args) {
  std::vector<char> argsData;

  if constexpr (sizeof...(Args) > 0) {
    auto bytes = get_bytes(args...);
    argsData.resize(bytes);
    OStream os(argsData.data(), bytes);
    pack_result(&os, std::forward<Args>(args)...);
  }

  IStream is(argsData.empty() ? nullptr : argsData.data(), argsData.size());
  OStream os;
  router->ExecuteFunction(plugin, fname, &is, &os);

  if constexpr (!std::is_void_v<ret_type>) {
    IStream rs(os.Data(), os.Size());
    ret_type ret;
    unpack_args_read(&rs, ret);
    return ret;
  }
}

} // namespace Anycubic::Plugins