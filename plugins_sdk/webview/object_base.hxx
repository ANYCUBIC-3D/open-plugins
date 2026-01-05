#pragma once
#ifdef BUILD_WEBVIEW_API
#include <WebView/include/webview.h>
#else
#include <webview/webview.h>
#endif // BUILD_WEBVIEW_API

#ifdef __WXMSW__
#undef DELETE
#endif // __WXMSW__

namespace Anycubic::Plugins::SDK {

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
class ObjectBase {
public:
  virtual ~ObjectBase() = default;
  virtual void Reload(void) = 0;
  virtual void Release(void) = 0;
};

class HandlerBase : public ObjectBase {
public:
  virtual ~HandlerBase() = default;
  virtual bool Execute(const wxWebViewHandlerRequest &request,
                       wxSharedPtr<wxWebViewHandlerResponse> resp) const = 0;
};

class RouterBase : public HandlerBase {
public:
  virtual ~RouterBase() = default;
  enum METHOD_TYPE { MethodGET = 0, MethodPOST, MethodDELETE, MethodPUT };
  virtual bool RegisterHandler(METHOD_TYPE method, const wxString &pattern,
                               HandlerBase *handler) = 0;
};

class ExecuteBase : public ObjectBase {
public:
  virtual void ExecuteScript(const wxString &script) = 0;
  virtual ~ExecuteBase() = default;
};

class MatcherBase : public ObjectBase {
public:
  virtual ~MatcherBase() = default;
  // Match request path and populate its matches and
  virtual bool Match(const wxString &path) const = 0;
};

class InstanceBase : public ObjectBase {
public:
  virtual ~InstanceBase() = default;
  virtual bool Init(RouterBase &router) = 0;
};

class StringResponseData : public wxWebViewHandlerResponseData {
public:
  /**
   * @brief 构造函数
   * @param data 要响应的字符串数据
   */
  StringResponseData(const wxString &data) : m_stream(data) {}

  /**
   * @brief 获取输入流
   * @return 字符串输入流指针
   */
  wxInputStream *GetStream() override { return &m_stream; }

private:
  wxStringInputStream m_stream; ///< 字符串输入流
};
} // namespace Anycubic::Plugins::SDK
