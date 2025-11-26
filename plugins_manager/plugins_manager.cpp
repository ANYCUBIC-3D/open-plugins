#include "plugins_manager.hxx"

#include "detail/package.hxx"
#include "detail/plugins_manager_impl.hpp"

#include <utility/codec/bin2ascii.hxx>
#include <utility/codec/md5.hxx>

#include <easy_log/stackstrace.hxx>

#include <filesystem>

bool GetPluginsPackageInfo(const char *plugins, PluginsPackageInfo *info) {
  FUNC_ENTRY;
  if (!plugins || !info) {
    FUNC_LEAVE;
    return false;
  }
  std::vector<char> buffer;
  if (!LoadSignture(plugins, buffer)) {
    FUNC_LEAVE2("load signture failed");
    return false;
  }

  if (!Decode(buffer)) {
    FUNC_LEAVE2("decode failed");
    return false;
  }
  Package p;
  if (!ParseInfo(buffer, &p)) {
    FUNC_LEAVE2("parse info failed");
    return false;
  }
  char md5[MD5LEN] = {0};
  if (!md5File(plugins, md5)) {
    FUNC_LEAVE2("md5 file failed");
    return false;
  }

  bin2hex(info->md5, md5, MD5LEN);

  info->version = p.version;
  info->build_time = p.build_time;
  FUNC_LEAVE;
  return true;
}

PluginsManager *SetupPM(const char *plugins, CreateWebView_t CreateWebView,
                        const char *tmp_dir) {
  assert(wxIsMainThread());
  REGISTER_LOGGER(false);

  FUNC_ENTRY;
  std::string tmp;
  if (tmp_dir == nullptr) {
    tmp = std::filesystem::temp_directory_path().string();
  } else {
    tmp = tmp_dir;
  }

  auto p = new PluginsManagerImpl(plugins, tmp, CreateWebView);
  if (!p->CheckPackage()) {
    FUNC_LEAVE2("check package failed");
    delete p;
    return nullptr;
  }
  FUNC_LEAVE;
  return p;
}
void ShutdownPM(PluginsManager *pm) {
  dynamic_cast<PluginsManagerImpl *>(pm)->~PluginsManagerImpl();
  UNREGISTER_LOGGER();
}