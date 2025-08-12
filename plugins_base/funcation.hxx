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
typedef std::function<void(struct IStream *data, struct OStream *result)>
    FuncationType;
class FuncationWrapper : public RequestHandler {
public:
  FuncationWrapper(const FuncationType &func) : func_(func) {}
  void Execute(IStream *data, struct OStream *result) override {
    func_(data, result);
  }
  void Destroy() override { delete this; }

private:
  FuncationType func_;
};

template <typename Function, typename Self>
RequestHandler *make_call(const Function &func, Self *self) {
  typedef function_traits<Function> func_traits;
  auto h = [func, self](struct IStream *data, struct OStream *result) {
    using ret_type = typename func_traits::return_type;
    typename func_traits::bare_tuple_type args;
    if constexpr (std::tuple_size_v<typename func_traits::bare_tuple_type> >
                  0) {
      unpack_args(args, data);
    }

    if constexpr (std::is_void_v<ret_type>) {
      std::apply(func, args);
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
  auto bytes = get_bytes(args...);
  std::vector<char> argsData(bytes);
  {
    OStream os(argsData.data(), argsData.size());
    (os.Write(args), ...);
  }
  IStream is(argsData.data(), argsData.size());

  OStream os;
  router->ExecuteFunction(plugin, fname, &is, &os);
  if constexpr (!std::is_void_v<ret_type>) {
    ret_type ret;
    unpack_result(ret, os);
    return ret;
  }
}

} // namespace Anycubic::Plugins