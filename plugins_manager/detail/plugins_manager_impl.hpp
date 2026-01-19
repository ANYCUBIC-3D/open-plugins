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
#include "router.hpp"

#include <wx/fs_mem.h>
#include <wx/panel.h>
#include <wx/string.h>
#include <wx/window.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

class LibraryBase;
class MemoryFSHandler;
class acTranslationsLoader;
class PluginsManagerImpl : public PluginsManager,
                           public Anycubic::Plugins::PluginHost {
public:
  PluginsManagerImpl(const char *plugins, const std::string &tmp_dir,
                     CreateWebView_t CreateWebView, const char *domain);
  ~PluginsManagerImpl() override;
  bool CheckPackage();
  // PluginsManager
private:
  bool AddWidget(const class wxString &position, wxWindow *widget) override;
  bool AddStaticPlugins(create_library_t *create, size_t count) override;

  bool SetConfig(class PMConfig *config) override;

  size_t Plugins(void) const override;

  void EmitEvent(EventType event) override;

  size_t Package(void) const override { return plugin_packages_.size(); }
  bool PackagePath(size_t index, wxString &path) override {
    if (index >= plugin_packages_.size()) {
      return false;
    }
    path = plugin_packages_[index];
    return true;
  }

  void SetPackageVisitor(PluginsVisitor_t visitor,
                         void *ctx = nullptr) override;
  bool ExecuteFunction(const char *plugin, const char *fname,
                       Anycubic::Plugins::IStream *data,
                       Anycubic::Plugins::OStream *result) override;

private:
  size_t LoadPlugins(void);
  bool CreateInstances(void);

private:
  // Anycubic::Plugins::PluginHost
  Anycubic::Plugins::PluginRouter *Router(void) override;
  bool HasPlugin(const char *name) override;
  Anycubic::Plugins::Plugin *GetPlugin(const char *name) override;
  class wxWindow *GetWindow(const char *postion = nullptr) override;
  bool WatchWindow(const char *postion, void *context,
                   bool (*callback)(void *context, wxWindow *window)) override;
  class wxPanel *CreatePanel(const class wxString &position,
                             const class wxString &xrcName,
                             const class wxString &xrc) override;
  wxPanel *CreatePanel(class wxWindow *parent, const class wxString &xrcName,
                       const class wxString &xrc) override;
  wxString Language(void) override;
  bool LoadTranslationFromData(const wxString &domain, void *data,
                               size_t bytes) override;
  bool LoadTranslationFromFile(const wxString &domain,
                               const wxString &path) override;
  bool AddFS(const wxString &name, const wxString &xrc) override;
  bool DelFS(const wxString &name) override;
  bool AddFS(const wxString &name, void *data, size_t length) override;
  bool GetValue(const class wxString &key, class wxString &value) override;
  bool SetValue(const class wxString &key, const class wxString &value,
                bool persistent = true) override;
  bool GetEncryptValue(const class wxString &key,
                       class wxString &value) override;
  bool SetEncryptValue(const class wxString &key, const class wxString &value,
                       bool persistent = true) override;
  void Free(const char *value) override;

private:
  bool InitPlugin(std::shared_ptr<LibraryBase> lib);

private:
  // 递归遍历目录获取插件
  void TraverseDirectory(const wxString &dirPath,
                         std::vector<wxString> &plugins);

private:
  std::string plugins_;                       ///< 插件压缩包
  std::string tmp_dir_;                       ///< 插件解压目录
  std::string domain_;                        ///< 主程序翻译名
  acTranslationsLoader *translations_loader_; ///< 翻译加载器
  PMConfig *config_;                          ///< 插件配置
  struct WidgetsNode {
    wxWindow *widget;  ///< 主程序窗口
    wxString position; ///< 位置标记
  };
  std::vector<WidgetsNode> widgets_;                    ///< 插件可用窗口列表
  std::vector<std::shared_ptr<LibraryBase>> libraries_; ///< 动态插件库列表
  std::vector<
      std::pair<wxString, std::shared_ptr<struct Anycubic::Plugins::Plugin>>>
      instances_;                                ///< 插件实例列表
  std::shared_ptr<EventRouter> router_;          ///< 调用路由
  std::shared_ptr<MemoryFSHandler> fs_handler_;  ///< 内存文件系统处理器
  CreateWebView_t create_webview_;               ///< 创建webview的函数指针
  std::vector<create_library_t> static_plugins_; ///< 动态增加的静态插件列表
  std::vector<wxString> plugin_packages_;        ///< 插件包列表
  int is_inited_;                                ///< 是否初始化完成
  std::map<wxString,
           std::function<bool(wxWindow *window)>>
      watchers_; ///< 窗口监听列表
  std::function<bool(const wxString &package)>
      package_visitor_; ///< 插件包加载观察函数
};