// Copyright (c) 2026 深圳市纵维立方科技有限公司
// Open-Plugin is licensed under Mulan PSL v2.
// You can use this software according to the terms and conditions of the Mulan
// PSL v2.
// You may obtain a copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
// See the Mulan PSL v2 for more details.

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
#if defined(__WXMSW__)
    url = GetParamValue(wxASCII_STR("win_url"));
#elif defined(__WXOSX__)
    url = GetParamValue(wxASCII_STR("osx_url"));
#elif defined(__WXGTK__)
    url = GetParamValue(wxASCII_STR("gtk_url"));
#else
#error "Unsupported platform"
#endif
  }
  assert(!url.IsEmpty());
  // 获取回调插件名
  wxString plugins_name = GetParamValue(wxASCII_STR("plugin_name"));
  bool clear = GetBool(wxASCII_STR("clear_before"), false);
  Anycubic::Plugins::Plugin *plugin = nullptr;
  if (getPlugin_ != nullptr) {
    plugin = getPlugin_(plugins_name);
  }
  wxWebViewConfiguration confg =
      wxWebView::NewConfiguration(wxWebViewBackendDefault);
  if (clear) {
    confg.ClearCookies();
  }
  wxWebView *webview = CreateWebView_(
      m_parentAsWindow, url, &confg, [plugin, this](wxWebView *view) {
        view->SetId(GetID());
        view->SetName(GetName());
        if (plugin == nullptr) {
          return;
        }
        plugin->CreateWebview(view, m_parentAsWindow);
      });

  return webview;
}

bool WebviewHandler::CanHandle(wxXmlNode *node) {
  return IsOfClass(node, wxT("wxWebView"));
}
