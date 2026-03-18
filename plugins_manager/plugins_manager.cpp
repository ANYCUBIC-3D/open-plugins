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
  info->name = strdup(p.name.c_str());
  FUNC_LEAVE;
  return true;
}

void FreePluginsPackageInfo(PluginsPackageInfo *info) {
  if (info->name) {
    free(info->name);
  }
}

PluginsManager *SetupPM(const char *plugins, CreateWebView_t CreateWebView,
                        const char *domain, const char *tmp_dir) {
  assert(wxIsMainThread());
  REGISTER_LOGGER(false);

  FUNC_ENTRY;
  std::string tmp;
  if (tmp_dir == nullptr) {
    tmp = std::filesystem::temp_directory_path().string();
  } else {
    tmp = tmp_dir;
  }

  auto p = new PluginsManagerImpl(plugins, tmp, CreateWebView, domain);
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