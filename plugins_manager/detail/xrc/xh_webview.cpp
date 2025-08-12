#include "xh_webview.hpp"

#include <slic3r/GUI/Widgets/WebView.hpp>

#include <plugins_base/plugins.hxx>

wxIMPLEMENT_DYNAMIC_CLASS(WebviewHandler, wxXmlResourceHandler);

WebviewHandler::WebviewHandler() : WebviewHandler(nullptr) {}

WebviewHandler::WebviewHandler(
    const std::function<struct Anycubic::Plugins::Plugin *(const wxString &)>
        &getPlugin)
    : wxXmlResourceHandler(), getPlugin_(getPlugin) {}

wxObject *WebviewHandler::DoCreateResource() {
  wxString url = GetParamValue(wxASCII_STR("url"));
  wxString plugins_name = GetParamValue(wxASCII_STR("name"));
  bool clear = GetBool(wxASCII_STR("clear"), false);
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
      WebView::CreateWebView(nullptr, url, &confg, [plugin](wxWebView *view) {
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
