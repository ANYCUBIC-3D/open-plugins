#pragma once
#include "plugins_manager.hxx"
#include "router.hpp"

#include <wx/fs_mem.h>
#include <wx/string.h>
#include <wx/window.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

class LibraryBase;
class PluginsManagerImpl : public PluginsManager,
                           public Anycubic::Plugins::PluginHost {
public:
  PluginsManagerImpl(const char *plugins, const char *tmp_dir,
                     CreateWebView_t CreateWebView);
  ~PluginsManagerImpl();
  bool CheckPackage();
  // PluginsManager
private:
  bool AddWidget(const class wxString &position,
                 class wxWindow *widget) override;

  bool AddStaticPlugins(create_library_t *create, size_t count) override;

  bool SetConfig(class PMConfig *config) override;

  size_t Plugins(void) const override;

  void EmitEvent(EventType event) override;

private:
  size_t LoadPlugins(void);
  bool CreateInstances(void);

private:
  // Anycubic::Plugins::PluginHost
  Anycubic::Plugins::PluginRouter *Router(void) override;
  bool HasPlugin(const char *name) override;
  Anycubic::Plugins::Plugin *GetPlugin(const char *name) override;
  class wxWindow *GetWindow(const char *postion = nullptr) override;
  bool AddFS(const wxString &name, const wxString &xrc) override;
  bool DelFS(const wxString &name) override;
  bool AddFS(const wxString &name, void *data, size_t length) override;
  bool SaveConfig(const char *name, const char *value) override;
  bool LoadConfig(const char *name, const char **value) override;
  void Free(const char *value) override;

private:
  std::string plugins_; ///< 插件压缩包
  std::string tmp_dir_; ///< 插件解压目录
  PMConfig *config_;    ///< 插件配置
  struct WidgetsNode {
    wxWindow *widget;  ///< 主程序窗口
    wxString position; ///< 位置标记
  };
  std::vector<WidgetsNode> widgets_;                    ///< 插件可用窗口列表
  std::vector<std::shared_ptr<LibraryBase>> libraries_; ///< 动态插件库列表
  std::map<wxString, std::shared_ptr<struct Anycubic::Plugins::Plugin>>
      instances_;                                ///< 插件实例列表
  std::shared_ptr<EventRouter> router_;          ///< 调用路由
  wxMemoryFSHandler fs_handler_;                 ///< 内存文件系统处理器
  CreateWebView_t CreateWebView_;                ///< 创建webview的函数指针
  std::vector<create_library_t> static_plugins_; ///< 动态增加的静态插件列表
};