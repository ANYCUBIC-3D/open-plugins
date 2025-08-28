#pragma once

#include <plugins_base/function_traits.hxx>

#include <webview.h>

#include <utility/utils/query.hxx>

#include <iguana/json.hpp>
#include <utility/json/reader.hxx>

#include <functional>
#include <string>
#include <vector>

#include <stdint.h>

#include <wx/sstream.h>
#include <wx/uri.h>

#include <boost/lexical_cast.hpp>

#define ADD(method, type, module, func, ...)                                   \
  method("/api/v1/" #module "/" #func, &type::func, this, __VA_ARGS__)
namespace Anycubic::Plugins::SDK {
class ExecuteBase {
public:
  virtual void execute_script(const wxString &script) = 0;
  virtual ~ExecuteBase() = default;
};

class ExecuteScript : public ExecuteBase {
public:
  std::function<void(const wxString &)> execute;
  void execute_script(const wxString &script) override {
    return execute(script);
  }
};

enum StatusCode {
  // Information responses
  Continue_100 = 100,
  SwitchingProtocol_101 = 101,
  Processing_102 = 102,
  EarlyHints_103 = 103,

  // Successful responses
  OK_200 = 200,
  Created_201 = 201,
  Accepted_202 = 202,
  NonAuthoritativeInformation_203 = 203,
  NoContent_204 = 204,
  ResetContent_205 = 205,
  PartialContent_206 = 206,
  MultiStatus_207 = 207,
  AlreadyReported_208 = 208,
  IMUsed_226 = 226,

  // Redirection messages
  MultipleChoices_300 = 300,
  MovedPermanently_301 = 301,
  Found_302 = 302,
  SeeOther_303 = 303,
  NotModified_304 = 304,
  UseProxy_305 = 305,
  unused_306 = 306,
  TemporaryRedirect_307 = 307,
  PermanentRedirect_308 = 308,

  // Client error responses
  BadRequest_400 = 400,
  Unauthorized_401 = 401,
  PaymentRequired_402 = 402,
  Forbidden_403 = 403,
  NotFound_404 = 404,
  MethodNotAllowed_405 = 405,
  NotAcceptable_406 = 406,
  ProxyAuthenticationRequired_407 = 407,
  RequestTimeout_408 = 408,
  Conflict_409 = 409,
  Gone_410 = 410,
  LengthRequired_411 = 411,
  PreconditionFailed_412 = 412,
  PayloadTooLarge_413 = 413,
  UriTooLong_414 = 414,
  UnsupportedMediaType_415 = 415,
  RangeNotSatisfiable_416 = 416,
  ExpectationFailed_417 = 417,
  ImATeapot_418 = 418,
  MisdirectedRequest_421 = 421,
  UnprocessableContent_422 = 422,
  Locked_423 = 423,
  FailedDependency_424 = 424,
  TooEarly_425 = 425,
  UpgradeRequired_426 = 426,
  PreconditionRequired_428 = 428,
  TooManyRequests_429 = 429,
  RequestHeaderFieldsTooLarge_431 = 431,
  UnavailableForLegalReasons_451 = 451,

  // Server error responses
  InternalServerError_500 = 500,
  NotImplemented_501 = 501,
  BadGateway_502 = 502,
  ServiceUnavailable_503 = 503,
  GatewayTimeout_504 = 504,
  HttpVersionNotSupported_505 = 505,
  VariantAlsoNegotiates_506 = 506,
  InsufficientStorage_507 = 507,
  LoopDetected_508 = 508,
  NotExtended_510 = 510,
  NetworkAuthenticationRequired_511 = 511,
};
class MatcherBase {
public:
  virtual ~MatcherBase() = default;

  // Match request path and populate its matches and
  virtual bool match(const wxString &path) const = 0;
};

class RegexMatcher final : public MatcherBase {
public:
  explicit RegexMatcher(const wxString &pattern) : regex_(pattern) {}

  bool match(const wxString &request) const override;

private:
  wxString regex_;
};

class handlerStringResponseData : public wxWebViewHandlerResponseData {
public:
  explicit handlerStringResponseData(wxString &data);

  wxInputStream *GetStream() override;

private:
  wxStringInputStream m_stream;
};

using Handler = std::function<bool(wxWebViewHandlerRequest &request,
                                   wxSharedPtr<wxWebViewHandlerResponse>)>;

#if defined(DELETE) && defined(__WXMSW__)
// windows下有个宏定义 DELETE
#undef DELETE
#endif // DELETE
class Router {
public:
#define GET(pattern, func, self, ...) Get(pattern, func, self, {__VA_ARGS__})
#define POST(pattern, func, self, ...) Post(pattern, func, self, {__VA_ARGS__})
#define DELETE(pattern, func, self, ...)                                       \
  Delete(pattern, func, self, {__VA_ARGS__})
#define PUT(pattern, func, self, ...) Put(pattern, func, self, {__VA_ARGS__})

  template <typename Function, typename Self>
  bool Get(const wxString &pattern, const Function &func, Self *self,
           const std::vector<wxString> &param_names) {
    return add_handler(
        MethodGET, pattern,
        make_handler<true>(func, self, param_names, [](auto &req) {
          wxURI uri(req.GetRawURI());
          return uri.GetQuery();
        }));
  }

  template <typename Function, typename Self>
  bool Post(const wxString &pattern, const Function &func, Self *self,
            std::vector<wxString> &&param_names) {
    return add_handler(
        MethodPOST, pattern,
        make_handler(func, self, std::move(param_names),
                     [](auto &req) { return req.GetDataString(); }));
  }
  template <typename Function, typename Self>
  bool Delete(const wxString &pattern, const Function &func, Self *self,
              std::vector<wxString> &&param_names) {
    return add_handler(
        MethodDELETE, pattern,
        make_handler(func, self, std::move(param_names),
                     [](auto &req) { return req.GetDataString(); }));
  }
  template <typename Function, typename Self>
  bool Put(const wxString &pattern, const Function &func, Self *self,
           std::vector<wxString> &&param_names) {
    return add_handler(
        MethodPUT, pattern,
        make_handler(func, self, std::move(param_names),
                     [](auto &req) { return req.GetDataString(); }));
  }

public:
  bool dispatch(wxWebViewHandlerRequest &request,
                wxSharedPtr<wxWebViewHandlerResponse> response);

public:
  Router(void);
  ~Router(void);

private:
  enum METHOD {
    MethodGET = 0,
    MethodPOST,
    MethodDELETE,
    MethodPUT,
    MethodOptions
  };
  bool add_handler(METHOD method, const wxString &pattern, Handler &&handler);
  MatcherBase *make_matcher(const wxString &pattern);
  template <typename _Ty>
  static std::enable_if_t<iguana::is_reflection_v<_Ty>, std::string>
  object_to_json(const _Ty &obj) {
    iguana::string_stream ss;
    iguana::json::to_json(ss, obj);
    return ss.str();
  }
  template <typename _Ty> static wxString create_response(const _Ty &val) {
    ac::json::reader writer;
    writer["code"] = 200;
    writer["msg"] = "OK";

    using data_type = std::decay_t<_Ty>;
    if constexpr (std::is_same_v<data_type, wxString>) {
      writer["data"] = val.utf8_string();
    } else if constexpr (std::is_same_v<data_type, std::string> ||
                         std::is_arithmetic_v<data_type>) {
      writer["data"] = val;
    } else if constexpr (ac::json::is_reflection_v<data_type>) {
      writer["data"] = ac::json::reader::parse(object_to_json(val));
    } else {
      assert(false);
    }
    auto str = writer.dump();
    return wxString::FromUTF8(str.data(), str.size());
  }
  template <typename _Ty> static _Ty transform_query(const std::string &val) {
    using ret_type = std::decay_t<_Ty>;
    if constexpr (std::is_same_v<std::string, ret_type>) {
      return val;
    } else if constexpr (std::is_same_v<wxString, ret_type>) {
      return wxString::FromUTF8(val.data(), val.size());
    } else if constexpr (std::is_arithmetic_v<ret_type>) {
      try {
        return boost::lexical_cast<ret_type>(val);
      } catch (...) {
        return ret_type(0);
      }
    } else {
      assert(false);
      return ret_type();
    }
  }
  template <typename _Ty>
  static void transform_json(_Ty &ret,
                             const ac::json::reader::value_type &val) {
    using ret_type = std::decay_t<_Ty>;
    if constexpr (std::is_same_v<bool, ret_type>) {
      ret = val;
    } else if constexpr (std::is_floating_point_v<ret_type>) {
      ret = val;
    } else if constexpr (std::is_arithmetic_v<ret_type>) {
      ret = val;
    } else if constexpr (std::is_same_v<std::string, ret_type>) {
      ret = val;
    } else if constexpr (std::is_same_v<wxString, ret_type>) {
      std::string str = val;
      ret = wxString::FromUTF8(str.data(), str.size());
    } else {
      assert(false);
      ret = L"77dk";
    }
  }
  template <int32_t I = 0, typename... Args>
  static void assign_query(std::tuple<Args...> &tuple,
                           const std::vector<wxString> &names,
                           const query_type &list) {
    if constexpr (I < sizeof...(Args)) {
      assert(sizeof...(Args) <= names.size());
      using arg_type = decltype(std::get<I>(tuple));
      static_assert(!(std::is_const_v<arg_type>), "");
      auto pair = list.find(names[I].ToStdString());
      assert(pair != list.end());

      std::get<I>(tuple) =
          transform_query<std::decay_t<arg_type>>(pair->second);
      assign_query<I + 1>(tuple, names, list);
    }
  }
  template <int32_t I = 0, typename... Args>
  static void assign_json(std::tuple<Args...> &tuple,
                          const std::vector<wxString> &names,
                          ac::json::reader &read) {
    if constexpr (I < sizeof...(Args)) {
      assert(sizeof...(Args) <= names.size());
      using arg_type = decltype(std::get<I>(tuple));
      static_assert(!(std::is_const_v<arg_type>));
      auto pair = read.find(names[I].ToStdString());
      assert(pair != read.end());
      if (pair != read.end()) {
        transform_json(std::get<I>(tuple), *pair);
      }
      assign_json<I + 1>(tuple, names, read);
    }
  }
  template <bool query, typename _Ty>
  static void decode(_Ty &val, const wxString &str,
                     const std::vector<wxString> &names) {
    std::string jsonstr;
    if constexpr (query) {
      jsonstr = wxURI::Unescape(str).utf8_string();
    } else {
      jsonstr = str.utf8_string();
    }

    if constexpr (query) {
      auto args = ::parse_query(jsonstr);
      assign_query(val, names, args);
    } else if (ac::json::reader reader;
               ac::json::parse_json(reader, jsonstr.data(), jsonstr.size())) {
      assign_json(val, names, reader);
    }
  }
  template <bool query = false, typename Function, typename Self,
            typename ParamFunc>
  Handler make_handler(const Function &func, Self *self,
                       const std::vector<wxString> &param_names,
                       const ParamFunc &get_str) const {
    using func_traits = Anycubic::Plugins::function_traits<Function>;
    Handler h = [param_names, func, self, get_str](auto &req, auto res) {
      typename func_traits::bare_tuple_type args;
      using ret_type = typename func_traits::return_type;
      wxString resBody = R"({"code":500,"msg":"Internal Server Error"})";
      int32_t code = InternalServerError_500;
      try {
        auto body = get_str(req);
        Router::decode<query>(args, body, param_names);
        resBody = Router::create_response(
            std::apply(func, std::tuple_cat(std::make_tuple(self), args)));
        code = OK_200;
      } catch (...) {
      }
      res->SetContentType("application/json;charset=UTF-8");

      res->SetStatus(code);
      res->Finish(wxSharedPtr<wxWebViewHandlerResponseData>(
          new handlerStringResponseData(resBody)));
      return true;
    };
    return h;
  }

private:
  struct Node {
    MatcherBase *matcher_;
    Handler handler_;
  };
  std::vector<Node> &get_handler(METHOD method);
  std::vector<Node> m_get_handlers;
  std::vector<Node> m_post_handlers;
  std::vector<Node> m_delete_handlers;
  std::vector<Node> m_put_handlers;
};
} // namespace Anycubic::Plugins::SDK
