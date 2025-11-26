#include "gtest/gtest.h"

#include <plugins_manager/plugins_manager.hxx>

#include <WebView/include/webview.h>

#ifdef __WXMAC__
#define ENV_NAME "DYLD_LIBRARY_PATH"
#elif defined(__WXGTK__)
#define ENV_NAME "LD_LIBRARY_PATH"
#else
#define ENV_NAME "PATH"
#endif

wxWebView *(CreateWebView)(wxWindow * parent, const wxString &url,
                           wxWebViewConfiguration *conf,
                           const std::function<void(wxWebView *)> &visitor) {
  return nullptr;
}

class VConfig : public PMConfig {
  bool GetValue(const class wxString &, class wxString &value) override {
    return true;
  }
  bool SetValue(const class wxString &, const class wxString &value,
                bool persistent = true) override {
    return true;
  }
  bool GetEncryptValue(const class wxString &, class wxString &value) override {
    return true;
  }
  bool SetEncryptValue(const class wxString &, const class wxString &value,
                       bool persistent = true) override {
    return true;
  }
};

class PMBaseTest : public ::testing::Test {
protected:
  void SetUp() override {
    const char *plugins = std::getenv("PLUGINS");
    if (plugins == nullptr) {
      throw std::runtime_error("PLUGINS environment variable is not set");
    }
    append_env(ENV_NAME, plugins);

    pm_ = ::SetupPM(plugins, CreateWebView);
    pm_->SetConfig(&config_);
  }
  void TearDown() override { ::ShutdownPM(pm_); }

  bool append_env(const std::string &env_name, const std::string &path) {
    const char *env = ::getenv(env_name.c_str());
    if (env == nullptr) {
      ::setenv(env_name.c_str(), path.c_str(), 1);
      return true;
    }
    std::string env_path = env;
    if (env_path.find(path) != std::string::npos) {
      return false;
    }
    env_path += ":" + path;
    ::setenv(env_name.c_str(), env_path.c_str(), 1);
    return true;
  }

private:
  VConfig config_;
  PluginsManager *pm_ = nullptr;
};
TEST_F(PMBaseTest, base) {}
