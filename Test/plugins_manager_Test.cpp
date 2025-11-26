#include "gtest/gtest.h"

#include <plugins_manager/plugins_manager.hxx>

#include <easy_log/log.hxx>

#include <WebView/include/webview.h>

#include <filesystem>

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

class PMEnv : public testing::Environment {
  void SetUp() override {

    // 当前工作目录
    std::string current_dir = std::filesystem::current_path().string();
    anycubic::logger::setup_log(current_dir.c_str(),
                                anycubic::logger::level_enum::trace);
  }

  void TearDown() override {}
};

static auto env_ = testing::AddGlobalTestEnvironment(new PMEnv);
class PMBaseTest : public ::testing::Test {
protected:
  void SetUp() override {
    const char *plugins = std::getenv("PLUGINS");
    if (plugins == nullptr) {
      throw std::runtime_error("PLUGINS environment variable is not set");
    }
    append_env(ENV_NAME, plugins);

    pm_ = ::SetupPM(plugins, CreateWebView, plugins);
    assert(pm_);
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

protected:
  VConfig config_;
  PluginsManager *pm_ = nullptr;
};
TEST_F(PMBaseTest, base) {
  for (auto event : {
           EventType::kEventInitByApp,
           EventType::kEventInitByGUI,
           EventType::kEventFinishedByGUI,
           EventType::kEventExitByGUI,
           EventType::kEventExitByApp,
       }) {
    pm_->EmitEvent(event);
  }
}

// enum class EventType : int8_t {
//    ///< 应用程序初始化时触发
//    ///< GUI框架初始化时触发
//    ///< GUI初始化完成时触发
//    ///< GUI退出时触发
//    ///< 应用程序退出时触发
// };