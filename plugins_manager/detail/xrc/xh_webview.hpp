// Copyright (c) [Year] [name of copyright holder]
// Open-Plugin is licensed under Mulan PSL v2.
// You can use this software according to the terms and conditions of the Mulan
// PSL v2.
// You may obtain a copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
// See the Mulan PSL v2 for more details.

#pragma once
#include "plugins_manager.hxx"

#include <wx/xrc/xmlres.h>

#include <functional>

namespace Anycubic::Plugins {
struct Plugin;
}

class WebviewHandler : public wxXmlResourceHandler {
public:
  WebviewHandler();
  WebviewHandler(
      CreateWebView_t CreateWebView,
      const std::function<Anycubic::Plugins::Plugin *(const wxString &)>
          &getPlugin);

private:
  wxObject *DoCreateResource() override;
  bool CanHandle(wxXmlNode *node) override;
  DECLARE_DYNAMIC_CLASS(WebviewHandler)
public:
  std::function<Anycubic::Plugins::Plugin *(const wxString &)> getPlugin_;
  CreateWebView_t CreateWebView_;
};