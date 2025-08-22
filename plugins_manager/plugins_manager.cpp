#include "plugins_manager.hxx"

#include "detail/package.hxx"
#include "detail/plugins_manager_impl.hpp"

#include <utility/codec/bin2ascii.hxx>
#include <utility/codec/md5.hxx>

bool GetPluginsPackageInfo(const char *plugins, PluginsPackageInfo *info) {
  if (!plugins || !info)
    return false;
  std::vector<char> buffer;
  if (!LoadSignture(plugins, buffer)) {
    return false;
  }

  if (!Decode(buffer)) {
    return false;
  }
  Package p;
  if (!ParseInfo(buffer, &p)) {
    return false;
  }
  char md5[MD5LEN] = {0};
  if (!md5File(plugins, md5)) {
    return false;
  }

  bin2hex(info->md5, md5, MD5LEN);

  info->version = p.version;
  info->build_time = p.build_time;

  return true;
}

PluginsManager *SetupPM(const char *plugins, CreateWebView_t CreateWebView,
                        const char *tmp_dir) {
  assert(wxIsMainThread());
  auto p = new PluginsManagerImpl(plugins, tmp_dir, CreateWebView);
  if (p->CheckPackage())
    return p;
  delete p;
  return nullptr;
}
void ShutdownPM(PluginsManager *pm) {
  dynamic_cast<PluginsManagerImpl *>(pm)->~PluginsManagerImpl();
}