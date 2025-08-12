#include "webview_api_handler.hxx"
#include "detail/router.hxx"

#include <wx/log.h>
namespace Anycubic::Plugins::SDK {
WebviewApiHandler::WebviewApiHandler(const wxString &scheme,
                                     ExecuteBase *executer)
    : wxWebViewHandler(scheme), router_(new Router()), executer_(executer) {
  assert(executer_ != nullptr);
#if __WXMSW__
  SetVirtualHostName("ac.localhost");
#endif
}

WebviewApiHandler::~WebviewApiHandler() {
  for (auto &instance : instances_) {
    delete instance;
  }
  instances_.clear();

  delete router_;
  router_ = nullptr;
  executer_ = nullptr;
}

void WebviewApiHandler::Reload(void) {
  for (auto &instance : instances_) {
    instance->Reload();
    instance->RegisterApi(router_, executer_);
  }
}

void WebviewApiHandler::StartRequest(
    wxWebViewHandlerRequest &request,
    wxSharedPtr<wxWebViewHandlerResponse> response) {
  // 尝试使用 Router 处理请求
  if (router_->dispatch(request, response)) {
    return;
  }

  // 如果 Router 无法处理，则返回 404 错误
  response->SetStatus(404);
  response->SetContentType("application/json;charset=UTF-8");
  response->SetHeader("X-Content-Type-Options", "nosniff");
  wxString resBody = R"({"code":404,"msg":"NotFound"})";
  response->Finish(wxSharedPtr<wxWebViewHandlerResponseData>(
      new handlerStringResponseData(resBody)));
}
} // namespace Anycubic::Plugins::SDK