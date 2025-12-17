#include "tabbook_plugin.hpp"

#include <plugins_base/funcation.hxx>

#include <assert.h>

#include <wx/string.h>

TabBookPlugin::TabBookPlugin(Anycubic::Plugins::PluginHost *host)
    : host_(host) {
  assert(host_ != nullptr);
  auto router = host_->Router();
  assert(router != nullptr);
  router->REGISTER_FUNCATION(TabBookPlugin, CreateTab);
  router->REGISTER_FUNCATION(TabBookPlugin, RemoveTab);
  router->REGISTER_FUNCATION(TabBookPlugin, GetTabTitle);
  router->REGISTER_FUNCATION(TabBookPlugin, GetTabCount);
}
static wxString RandomString(int length) {
  wxString str;
  for (int i = 0; i < length; ++i) {
    str += (wxChar)('a' + rand() % 26);
  }
  return str;
}
TabBookPlugin::~TabBookPlugin() {}

int32_t TabBookPlugin::CreateTab(int idx, const std::string &title,
                                 const std::string &icon,
                                 const std::string &xrcName,
                                 const std::string &xrc) {
  return 0;
}

int32_t TabBookPlugin::RemoveTab(int idx) { return 0; }
int32_t TabBookPlugin::GetTabCount(void) const { return 1; }
std::string TabBookPlugin::GetTabTitle(int idx) const { return "test"; }
bool TabBookPlugin::AttachEvt(wxEvtHandler *) { return false; }

bool TabBookPlugin::DetachEvt(wxEvtHandler *) { return false; }

bool TabBookPlugin::BindEvt(wxPanel *panel, wxWindow *parent, wxString *bmp) {
  return false;
}

bool TabBookPlugin::CreateWebview(wxWebView *view, wxWindow *parent,
                                  wxString *bmp) {
  return false;
}

void TabBookPlugin::Destroy(void) { delete this; }
