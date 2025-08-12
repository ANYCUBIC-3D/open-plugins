#pragma once
#include <wx/xrc/xmlres.h>

#include <functional>

namespace Anycubic::Plugins {
struct Plugin;
}

class WebviewHandler : public wxXmlResourceHandler {
public:
  WebviewHandler();
  WebviewHandler(
      const std::function<Anycubic::Plugins::Plugin *(const wxString &)>
          &getPlugin);

private:
  wxObject *DoCreateResource() override;
  bool CanHandle(wxXmlNode *node) override;
  DECLARE_DYNAMIC_CLASS(WebviewHandler)
public:
  std::function<Anycubic::Plugins::Plugin *(const wxString &)> getPlugin_;
};