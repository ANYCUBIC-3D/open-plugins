#include "utility_plugin.hpp"

#include <plugins_base/funcation.hxx>

#include <wx/filename.h>

UtilityPlugin::UtilityPlugin(Anycubic::Plugins::PluginHost *host)
    : host_(host) {
  assert(host_ != nullptr);
  auto router = host_->Router();
  assert(router != nullptr);
  router->REGISTER_FUNCATION(UtilityPlugin, pcid);
  router->REGISTER_FUNCATION(UtilityPlugin, is_test_env);
  router->REGISTER_FUNCATION(UtilityPlugin, is_china_env);
  router->REGISTER_FUNCATION(UtilityPlugin, language);
  router->REGISTER_FUNCATION(UtilityPlugin, get_temp_path);
  router->REGISTER_FUNCATION(UtilityPlugin, get_download_path);
  router->REGISTER_FUNCATION(UtilityPlugin, set_download_path);
  router->REGISTER_FUNCATION(UtilityPlugin, set_access_token);
  router->REGISTER_FUNCATION(UtilityPlugin, get_access_token);
  router->REGISTER_FUNCATION(UtilityPlugin, is_auto_login);
  router->REGISTER_FUNCATION(UtilityPlugin, set_auto_login);
  router->REGISTER_FUNCATION(UtilityPlugin, get_login_token);
  router->REGISTER_FUNCATION(UtilityPlugin, set_login_token);
  router->REGISTER_FUNCATION(UtilityPlugin, get_user_info);
  router->REGISTER_FUNCATION(UtilityPlugin, set_user_info);
}

UtilityPlugin::~UtilityPlugin() {}

std::string UtilityPlugin::pcid(void) const { return ""; }
bool UtilityPlugin::is_test_env(void) const { return true; }
bool UtilityPlugin::is_china_env(void) const { return language() == "zh_CN"; }
std::string UtilityPlugin::language(void) const { return "zh_CN"; }
std::string UtilityPlugin::get_temp_path(void) const { return ""; }

std::string UtilityPlugin::get_download_path() const { return download_path_; }

void UtilityPlugin::set_download_path(const std::string &path) {
  download_path_ = path;
}
std::string UtilityPlugin::get_access_token(void) const {
  return access_token_;
}
void UtilityPlugin::set_access_token(const std::string &token) {
  access_token_ = token;
}
bool UtilityPlugin::is_auto_login(void) const { return false; }
void UtilityPlugin::set_auto_login(bool auto_login) { return; }
bool UtilityPlugin::get_login_token(wxString *token) {
  assert(token != nullptr);
  *token = wxEmptyString;
  return true;
}
void UtilityPlugin::set_login_token(const wxString *token) { return; }
void UtilityPlugin::get_user_info(wxString *username, wxString *password) {
  return;
}
void UtilityPlugin::set_user_info(const wxString *username,
                                  const wxString *password) {
  return;
}

bool UtilityPlugin::AttachEvt(wxEvtHandler *) { return false; }

bool UtilityPlugin::DetachEvt(wxEvtHandler *) { return false; }

bool UtilityPlugin::CreatePanel(wxPanel *panel, wxWindow *parent,
                                wxString *bmp) {
  return false;
}

bool UtilityPlugin::CreateWebview(wxWebView *view, wxWindow *parent,
                                  wxString *bmp) {
  return false;
}

void UtilityPlugin::Destroy(void) { delete this; }
