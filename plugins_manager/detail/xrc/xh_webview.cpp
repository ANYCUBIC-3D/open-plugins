#include "xh_webview.hpp"

#include <plugins_base/plugins.hxx>

#include <webview.h>

wxIMPLEMENT_DYNAMIC_CLASS(WebviewHandler, wxXmlResourceHandler);

WebviewHandler::WebviewHandler() : WebviewHandler(nullptr, nullptr) {}

WebviewHandler::WebviewHandler(
    CreateWebView_t CreateWebView,
    const std::function<struct Anycubic::Plugins::Plugin *(const wxString &)>
        &getPlugin)
    : wxXmlResourceHandler(), getPlugin_(getPlugin),
      CreateWebView_(CreateWebView) {}

wxObject *WebviewHandler::DoCreateResource() {
  wxString url = GetParamValue(wxASCII_STR("url"));
  if (url.IsEmpty()) {
    // 在windows下，使用虚拟服务来实施拦截处理
    // 其他平台是scheme处理，逻辑不同
#if __WXMSW__
    url = GetParamValue(wxASCII_STR("win_url"));
#elif __WXOSX__
    url = GetParamValue(wxASCII_STR("osx_url"));
#elif __WXGTK__
    url = GetParamValue(wxASCII_STR("gtk_url"));
#else
#error "Unsupported platform"
#endif
  }
  assert(!url.IsEmpty());
  // 获取回调插件名
  wxString plugins_name = GetParamValue(wxASCII_STR("plugin_name"));
  bool clear = GetBool(wxASCII_STR("clear_before"), false);
  Anycubic::Plugins::Plugin *plugin;
  if (getPlugin_ == nullptr) {
    plugin = getPlugin_(plugins_name);
  }
  wxWebViewConfiguration confg =
      wxWebView::NewConfiguration(wxWebViewBackendDefault);
  if (clear) {
    confg.ClearCookies();
  }
  wxWebView *webview =
      CreateWebView_(nullptr, url, &confg, [plugin](wxWebView *view) {
        if (plugin == nullptr) {
          return;
        }
        plugin->CreateWebview(view);
      });
  webview->SetId(GetID());
  webview->SetName(GetName());
  return webview;
}

bool WebviewHandler::CanHandle(wxXmlNode *node) {
  return IsOfClass(node, wxT("wxWebView"));
}
